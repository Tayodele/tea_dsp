//! Library for an out-of-process DSP engine that communicates via flatbuffers over pipes.

type SampleItem = f32;
pub type RawSample = i16;

/// Error in the engine.
#[derive(thiserror::Error, Debug)]
pub enum AudioError {
    #[error("Failed to decode audio file: {0}")]
    Decode(#[from] rodio::decoder::DecoderError),
    #[error("Failed to play audio: {0}")]
    PlayError(#[from] rodio::PlayError),
}

type AudioResult<T> = Result<T, AudioError>;

/// Implementations for a source of data messages to the engine.
pub trait EngineSource {
  fn open() -> Self;

  fn read_buffers(buf: &[u8]) -> AudioResult;

  
}

fn run_engine<T: EngineSource, U: EngineSink>(
    stream_input: EngineSource,
    control_rx: EngineSink,
    analysis: &mut [(String, impl AnalysisBuffer<FRAME_LEN>)],
) -> AudioResult<()> {
    // Get a output stream handle to the default physical sound device
    let (_stream, stream_handle) = OutputStream::try_default()?;
    let sink = Sink::try_new(&stream_handle)?;
    // Load a sound from a file, using a path relative to Cargo.toml
    let audio_raw = Cursor::new(std::fs::read(
        std::env::var("TEST_FILE").context("Finding test file")?,
    )?);
    // Decode that sound file into a source
    let mut source: Track<FRAME_LEN> = Track::decode_into_track(Decoder::new(audio_raw)?);
    // Analyze track ahead of time
    source.anaylze_track(analysis)?;
    source.write_parquet(&PathBuf::from("data.parquet"))?;
    info!(
        "Track is {} samples and {} channels",
        source.samples.len(),
        source.channels
    );
    sink.play();
    let mut wait_duration = Duration::ZERO;
    let mut latency = Duration::ZERO;
    loop {
        let now = Instant::now();
        while now.elapsed() < wait_duration.checked_sub(latency).unwrap_or(Duration::ZERO) {}
        let now = Instant::now();

        if let Some(frame) = source.get_next_frame() {
            let mut frame_analysis = FrameAnalysis::default();
            let len = frame.samples.len() as f32;
            let samples_sum = frame.samples.iter().map(|x| x.abs()).sum::<f32>();
            let samples_squared_sum = frame.samples.iter().map(|x| x.abs().powf(2.0)).sum::<f32>();
            for (_, analyzer) in analysis.iter_mut() {
              analyzer.process_frame(&frame, &mut frame_analysis);
            }
            frame_analysis.average_amplitude = samples_sum / len;
            frame_analysis.rms_amplitude = (samples_squared_sum / len).sqrt();
            frame_analysis.duration = frame.duration();
            // Skip a frame if the engine is lagging;
            wait_duration = frame_analysis.duration;
            if let Err(_) = weights_tx.try_send(frame_analysis) {
                error!("Frame dropped!");
            }
            sink.append(frame);
            latency = now.elapsed();
        }
        if let Ok(control) = control_rx.try_recv() {
            match control {
                AudioControl::Die => {
                    break;
                }
            }
        }
    }
    Ok(())
}

// Anaysis Tools //
trait AnalysisBuffer<const N: usize>: Send {
    fn get_complex_frame(&mut self) -> &mut [Complex<f32>];
    fn process_track(&mut self, name: &str, track: &mut Track<N>) -> Series;
    fn process_frame(&mut self, frame: &TrackFrame<N>, analysis: &mut FrameAnalysis<N>);
}

struct FrequencyAnalyzer<const FRAME_LEN: usize> {
    complex_frame: Box<[Complex<f32>]>,
    fft: Radix4<f32>,
}

impl<const FRAME_LEN: usize> FrequencyAnalyzer<FRAME_LEN> {
    fn new() -> Self {
        info!("FFT size: {FRAME_LEN}");
        Self {
            fft: Radix4::new(FRAME_LEN, FftDirection::Forward),
            complex_frame: vec![Complex::<f32>::default(); FRAME_LEN].into_boxed_slice(),
        }
    }

    fn calculate_real_fft(&self, samples: &[f32]) -> Vec<Series> {
      let mut series: Vec<Series> = Vec::new();
      let mut data = vec![Complex{re: 0.0, im: 0.0}; FRAME_LEN];
      for (idx, sample) in samples.iter().enumerate() {
        data[idx % FRAME_LEN] = Complex { re: *sample, im: 0.0 };
        if (idx + 1) % FRAME_LEN != 0 {
          continue;
        }
        self.fft.process(&mut data);
        series.push(data.iter().map(|x| x.norm()).collect());
        data = vec![Complex{re: 0.0, im: 0.0}; FRAME_LEN];
      }
      self.fft.process(&mut data);
      series.push(data.iter().map(|x| x.norm()).collect());
      series
    }
}

impl<const N: usize> AnalysisBuffer<N> for FrequencyAnalyzer<FRAME_LEN> {
    fn get_complex_frame(&mut self) -> &mut [Complex<f32>] {
        self.complex_frame.as_mut()
    }

    fn process_track(&mut self, name: &str, track: &mut Track<N>) -> Series {
        let data = self.calculate_real_fft(track.samples.as_slice());
        Series::new(name, data)
    }

    fn process_frame(&mut self, frame: &TrackFrame<N>, analysis: &mut FrameAnalysis<N>) {
      frame.get_complex_frame(self);
      self.fft.process(self.complex_frame.as_mut());
      for (idx, db) in analysis.window_db.iter_mut().enumerate() {
        *db = ((self.complex_frame[idx].norm() * 2.0) / (N as f32)).as_db();
      }
    }
}

pub trait AsDb {
    const SAMPLE_MAX: f32;
    fn as_db(&self) -> f32;
    fn attenuate(&self, ref_amp: f32) -> f32;
}

impl AsDb for f32 {
    const SAMPLE_MAX: f32 = SAMPLE_CEILING;

    fn as_db(&self) -> f32 {
        let db = 10.0 * (self / Self::SAMPLE_MAX).log10();
        if db.is_nan() {
            0.0
        } else {
            db
        }
    }

    fn attenuate(&self, ref_amp: f32) -> f32 {
        let db = if self.is_nan() { 0.0 } else { *self };
        10.0_f32.powf(db / 10.0) * ref_amp
    }
}

struct Track<const FRAME_LEN: usize> {
    samples: Vec<SampleItem>,
    index: usize,
    channels: u16,
    sample_rate: u32,
    analysis_data: DataFrame,
}

impl<const N: usize> Track<N> {
    fn decode_into_track<R: std::io::Read + std::io::Seek + Send + Sync + 'static>(
        value: Decoder<R>,
    ) -> Self {
        let channels = value.channels();
        let sample_rate = value.sample_rate();
        Self {
            samples: value.map(|x| x as f32).collect(),
            index: 0,
            channels,
            sample_rate,
            analysis_data: DataFrame::empty(),
        }
    }

    fn reset(&mut self) {
      self.index = 0;
    }

    fn get_next_frame(&mut self) -> Option<TrackFrame<N>> {
        if self.index >= self.samples.len() {
            return None;
        }
        let value = if (self.index + N) >= self.samples.len() {
            Some(TrackFrame {
                samples: [0.0; N].map(|x| {
                    if self.index >= self.samples.len() {
                        return x;
                    }
                    let new_x = self.samples[self.index];
                    self.index += 1;
                    new_x
                }),
                valid_samples: self.samples.len() - self.index,
                index: 0,
                channels: self.channels,
                sample_rate: self.sample_rate,
            })
        } else {
            Some(TrackFrame {
                samples: [0.0; N].map(|_| {
                    let new_x = self.samples[self.index];
                    self.index += 1;
                    new_x
                }),
                valid_samples: N,
                index: 0,
                channels: self.channels,
                sample_rate: self.sample_rate,
            })
        };
        value
    }

    fn anaylze_track(
        &mut self,
        processes: &mut [(String, impl AnalysisBuffer<N>)],
    ) -> anyhow::Result<()> {
      let mut series = Vec::new();
      for (name, process) in processes.iter_mut() {
        series.push(process.process_track(name.as_str(), self));
        self.reset();
      }
      self.analysis_data = DataFrame::new(series)?;
      Ok(())
    }

    fn write_parquet(&mut self, output_path: &Path) -> anyhow::Result<()> {
      let file = std::fs::File::create(output_path)?;
      let writer = ParquetWriter::new(file).with_compression(ParquetCompression::Snappy);
      writer.finish(&mut self.analysis_data)?;
      Ok(())
    }
}

impl<const N: usize> Iterator for Track<N> {
    type Item = i16;

    fn next(&mut self) -> Option<Self::Item> {
        if self.index >= self.samples.len() {
            None
        } else {
            let value = self.samples[self.index] as i16;
            self.index += 1;
            Some(value)
        }
    }
}

#[derive(Copy, Clone)]
pub struct TrackFrame<const FRAME_LEN: usize> {
    samples: [SampleItem; FRAME_LEN],
    valid_samples: usize,
    index: usize,
    channels: u16,
    sample_rate: u32,
}

impl<const N: usize> TrackFrame<N> {
    /// Total duration is always Some(_), so this provides a concrete implementation.
    pub fn duration(&self) -> Duration {
        self.total_duration().unwrap()
    }

    fn get_complex_frame<T: AnalysisBuffer<N>>(&self, buffer: &mut T) {
        buffer
            .get_complex_frame()
            .iter_mut()
            .enumerate()
            .for_each(|(idx, x)| {
                *x = Complex {
                    re: self.samples[idx],
                    im: 0.0,
                }
            })
    }
}

impl<const FRAME_LEN: usize> Iterator for TrackFrame<FRAME_LEN> {
    type Item = i16;

    fn next(&mut self) -> Option<Self::Item> {
        if self.index >= self.valid_samples {
            None
        } else {
            let value = self.samples[self.index] as i16;
            self.index += 1;
            Some(value)
        }
    }
}

impl<const FRAME_LEN: usize> rodio::Source for TrackFrame<FRAME_LEN> {
    fn current_frame_len(&self) -> Option<usize> {
        None
    }

    fn channels(&self) -> u16 {
        self.channels
    }

    fn sample_rate(&self) -> u32 {
        self.sample_rate
    }

    fn total_duration(&self) -> Option<Duration> {
        let total_samples = self.valid_samples;
        let sample_len = (total_samples / (self.channels as usize)) as f32;
        let sample_rate = self.sample_rate as f32;
        Some(Duration::from_secs_f32(sample_len / sample_rate))
    }
}
