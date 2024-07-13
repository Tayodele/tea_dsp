//! Library for an out-of-process DSP engine that communicates via flatbuffers over pipes.

use chunk::dsp::{Chunk, Telemetry};
use core::sync::atomic::AtomicBool;
use core::time::Duration;
use flatbuffers::FlatBufferBuilder;
use std::sync::atomic::Ordering;
use std::sync::Arc;
use std::time::Instant;

pub mod error;
pub mod io;
pub mod sine_generator;

use crate::error::{AudioError, AudioResult};
use crate::io::ChunkMessageBuilder;

type SampleItem = f32;

#[derive(Clone)]
pub struct Frame {
    pub samples: Box<[SampleItem]>,
    pub channels: u16,
    pub sample_rate: f32,
}

impl Frame {
    pub fn new(sample_rate: f32, channels: u16, capacity: usize) -> Self {
        Frame {
            samples: Vec::with_capacity(capacity).into_boxed_slice(),
            channels,
            sample_rate,
        }
    }

    pub fn parse_chunk(&mut self, chunk: &Chunk<'_>) -> AudioResult<()> {
        let sample_frame = chunk
            .data()
            .ok_or(AudioError::MissingField("SampleFrame".to_string()))?;
        self.channels = sample_frame.channels() as u16;
        self.sample_rate = sample_frame.sample_rate();
        let samples = sample_frame
            .samples()
            .ok_or(AudioError::MissingField("SampleFrame".to_string()))?;
        // Chunk `SampleFrame` are assumed to be homogenous in size for the most part,
        // so it's ok to take the hit in performance to resize ocasionally.
        if self.samples.len() != samples.len() {
            self.samples = vec![0.0; samples.len()].into();
        }
        // Copy the sample elements into the frame.
        for (idx, sample) in samples.iter().enumerate() {
            self.samples[idx] = sample;
        }
        Ok(())
    }
}

#[derive(Default)]
pub struct EngineTelemetry {
    pub per_component_latency: Vec<i32>,
    pub input_latency: i32,
    pub output_latency: i32,
}

/// Implementations for a source of data messages to the engine.
/// Implementations for a sink of data messages to the engine.
pub trait EngineIO {
    fn read_buffer<'chunk>(&'chunk mut self) -> anyhow::Result<Chunk<'chunk>>;

    fn send_buffer(
        &mut self,
        builder: &mut ChunkMessageBuilder,
        frame: &Frame,
        telemetry: &EngineTelemetry,
    ) -> anyhow::Result<()>;
}

pub trait Component {
    fn process_chunk(&mut self, chunk: &Chunk<'_>, frame: &mut Frame) -> AudioResult<()>;
}

pub fn run_engine<T: EngineIO>(
    stream: &mut T,
    components: &mut [impl Component + Sized],
    sample_rate: f32,
    channels: u16,
) -> AudioResult<()> {
    let signal = Arc::new(AtomicBool::new(false));
    let ctrlc_signal = signal.clone();

    ctrlc::set_handler(move || {
        ctrlc_signal.store(true, Ordering::Relaxed);
    })
    .expect("Error setting Ctrl-C handler");

    let mut frame = Frame::new(sample_rate, channels, 256);
    let mut output_latency = 0;
    let mut telemetry = EngineTelemetry {
        per_component_latency: Vec::with_capacity(components.len()),
        input_latency: 0,
        output_latency: 0,
    };
    let mut builder = ChunkMessageBuilder::from(FlatBufferBuilder::with_capacity(1024));
    loop {
        if signal.load(Ordering::Relaxed) {
            break;
        }
        let start = Instant::now();
        let chunk = match stream.read_buffer() {
            Ok(c) => c,
            Err(e) => {
                continue;
            }
        };
        if frame.parse_chunk(&chunk).is_ok() {
            telemetry.input_latency = start.elapsed().as_micros() as i32;
            telemetry.per_component_latency.clear();
            for component in components.iter_mut() {
                let start = Instant::now();
                component.process_chunk(&chunk, &mut frame)?;
                telemetry
                    .per_component_latency
                    .push(start.elapsed().as_micros() as i32);
            }
        }
        let start = Instant::now();
        stream.send_buffer(&mut builder, &frame, &telemetry)?;
        telemetry.output_latency = start.elapsed().as_micros() as i32;
    }

    Ok(())
}

pub trait AsDb {
    const SAMPLE_MAX: f32;
    fn as_db(&self) -> f32;
    fn attenuate(&self, ref_amp: f32) -> f32;
}

impl AsDb for f32 {
    const SAMPLE_MAX: f32 = f32::MAX;

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
