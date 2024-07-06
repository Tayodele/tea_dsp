use crate::error::{AudioError, AudioResult};
use crate::{AsDb, Component, Frame};
use chunk::dsp::Chunk;
use std::f32::consts::PI;

const CEILING: f32 = f32::MAX;

#[derive(Default)]
struct SineGeneratorComponent {
    gain_db: f32,
    freq_hz: f32,
    phase_rad: f32,
    channel_steps: Vec<usize>,
}

impl Component for SineGeneratorComponent {
    fn process_chunk(&mut self, chunk: &Chunk<'_>, frame: &mut Frame) -> AudioResult<()> {
        let Some(control_params) = chunk.control_as_sine_generator_control() else {
            return Err(AudioError::MissingControl(
                "SineGeneratorControl".to_string(),
            ));
        };
        self.gain_db = control_params.gain_db();
        self.freq_hz = control_params.freq_hz();
        self.phase_rad = control_params.phase_rad();
        self.channel_steps.clear();
        for ch in 0..frame.channels {
            self.channel_steps.push(0);
        }
        let wave = |step: usize| {
            let rad = (2.0 * PI) * ((step + 1) as f32) * (self.freq_hz / frame.sample_rate);
            rad.sin() * CEILING
        };
        // All samples start at 0 degrees
        let mut channel_mod = 0;
        for (idx, sample) in frame.samples.iter_mut().enumerate() {
            *sample = self.gain_db.attenuate(wave(
                self.channel_steps[channel_mod] + ((self.phase_rad / 2.0 * PI) as usize),
            ));
            self.channel_steps[channel_mod] += 1;
            channel_mod = (channel_mod + 1) % (frame.channels as usize);
        }
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use chunk::dsp::{
        root_as_chunk, Chunk, ChunkArgs, ControlParam, SampleFrame, SampleFrameArgs,
        SineGeneratorControl, SineGeneratorControlArgs,
    };
    use flatbuffers::FlatBufferBuilder;

    fn rad_to_step(freq: f32, sample_rate: f32, rad: f32) -> usize {
        let steps_per_pi = sample_rate / freq;
        ((rad / (2.0 * PI)) * steps_per_pi) as usize - 1
    }

    #[test]
    fn test_sine_generator_simple() -> anyhow::Result<()> {
        let mut builder = FlatBufferBuilder::with_capacity(1024);

        let control = SineGeneratorControl::create(
            &mut builder,
            &SineGeneratorControlArgs {
                gain_db: 0.0,
                freq_hz: 100.0,
                phase_rad: 0.0,
            },
        );
        let frame_buffer = vec![0.0_f32; 1200];
        let samples = builder.create_vector(frame_buffer.as_slice());
        let sample_frame = SampleFrame::create(
            &mut builder,
            &SampleFrameArgs {
                channels: 1,
                sample_rate: 1200.0,
                samples: Some(samples),
            },
        );
        let chunk = Chunk::create(
            &mut builder,
            &ChunkArgs {
                control_type: ControlParam::SineGeneratorControl,
                control: Some(control.as_union_value()),
                data: Some(sample_frame),
            },
        );
        builder.finish(chunk, None);

        let chunk = root_as_chunk(builder.finished_data())?;
        let mut component = SineGeneratorComponent::default();
        let mut frame = Frame::new(1200.0, 1, 1024);
        frame.parse_chunk(&chunk)?;

        component.process_chunk(&chunk, &mut frame)?;
        let expected_max_step = rad_to_step(component.freq_hz, frame.sample_rate, PI / 2.0);
        assert_eq!(frame.samples[expected_max_step], f32::MAX);

        Ok(())
    }
}
