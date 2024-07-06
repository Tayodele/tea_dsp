//! Library for an out-of-process DSP engine that communicates via flatbuffers over pipes.

use chunk::dsp::{root_as_chunk, Chunk, ControlParam};
use core::sync::atomic::AtomicBool;
use num::complex::Complex;
use std::sync::atomic::Ordering;
use std::sync::Arc;

pub mod error;

use crate::error::AudioResult;

type SampleItem = f32;

#[derive(Clone)]
pub struct Frame {
    samples: Box<[SampleItem]>,
    valid_samples: usize,
    index: usize,
    channels: u16,
    sample_rate: u32,
}

impl Frame {
    pub fn new(sample_rate: u32, channels: u16, capacity: usize) -> Self {
        Frame {
            samples: Vec::with_capacity(capacity).into_boxed_slice(),
            valid_samples: 0,
            index: 0,
            channels,
            sample_rate,
        }
    }

    pub fn parse_chunk(&mut self, chunk: &Chunk<'_>) {}
}

/// Implementations for a source of data messages to the engine.
pub trait EngineSource {
    fn open() -> AudioResult<Self>
    where
        Self: Sized;

    fn read_buffer<'chunk>(&'chunk self) -> AudioResult<Chunk<'chunk>>;
}

/// Implementations for a sink of data messages to the engine.
pub trait EngineSink {
    fn open() -> AudioResult<Self>
    where
        Self: Sized;

    fn send_buffer(&self, frame: &Frame) -> AudioResult<()>;
}

pub trait Component {
    fn process_chunk(&mut self, chunk: &Chunk<'_>, frame: &mut Frame) -> AudioResult<()>;
}

fn run_engine<T: EngineSource, U: EngineSink>(
    stream_input: T,
    control_rx: U,
    components: &mut [impl Component + Sized],
    sample_rate: u32,
    channels: u16,
) -> AudioResult<()> {
    let signal = Arc::new(AtomicBool::new(true));
    let ctrlc_signal = signal.clone();

    ctrlc::set_handler(move || {
        ctrlc_signal.store(true, Ordering::Relaxed);
    })
    .expect("Error setting Ctrl-C handler");

    let mut frame = Frame::new(sample_rate, channels, 256);
    loop {
        if signal.load(Ordering::Relaxed) {
            break;
        }
        let chunk = stream_input.read_buffer()?;
        frame.parse_chunk(&chunk);
        for component in components.iter_mut() {
            component.process_chunk(&chunk, &mut frame)?;
        }
        control_rx.send_buffer(&frame)?;
    }

    Ok(())
}
