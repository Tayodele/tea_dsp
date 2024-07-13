//! Implementations of `EngineSource` and `EngineSink`

use chunk::dsp::{
    root_as_chunk, Chunk, ChunkArgs, ControlParam, SampleFrame, SampleFrameArgs, Telemetry,
    TelemetryArgs,
};
use core::time::Duration;
use engine::io::{ChunkMessage, ChunkMessageBuilder};
use engine::{EngineIO, EngineTelemetry, Frame};
use std::io::{Read, Write};
use std::net;
use std::ops::DerefMut;

const MAX_CHUNK_SIZE: usize = 3 * 1024 * 1024;

pub struct TcpSource {
    stream: net::TcpStream,
    chunk_buffer: Box<[u8]>,
}

impl TcpSource {
    pub fn open(addr: impl net::ToSocketAddrs) -> anyhow::Result<Self> {
        let stream = net::TcpStream::connect(&addr)?;
        stream.set_read_timeout(Some(Duration::from_millis(500)))?;
        stream.set_nonblocking(false)?;
        // stream.set_ttl(64)?;
        Ok(Self {
            stream,
            chunk_buffer: vec![0; MAX_CHUNK_SIZE].into_boxed_slice(),
        })
    }
}

impl EngineIO for TcpSource {
    fn read_buffer<'chunk>(&'chunk mut self) -> anyhow::Result<Chunk<'chunk>> {
        let size = self.stream.read(&mut self.chunk_buffer)?;
        let raw_chunk = &self.chunk_buffer[..size];
        let message = ChunkMessage::parse(raw_chunk).ok_or(anyhow::anyhow!("Not a chunk"))?;
        Ok(message.get_chunk()?)
    }

    fn send_buffer(
        &mut self,
        builder: &mut ChunkMessageBuilder,
        frame: &Frame,
        engine_telemetry: &EngineTelemetry,
    ) -> anyhow::Result<()> {
        builder.reset();

        let samples = builder.create_vector(&frame.samples);
        let sample_frame = SampleFrame::create(
            builder.deref_mut(),
            &SampleFrameArgs {
                channels: frame.channels as i16,
                sample_rate: frame.sample_rate,
                samples: Some(samples),
            },
        );
        let per_component_latency =
            builder.create_vector(engine_telemetry.per_component_latency.as_slice());
        let telemetry = Telemetry::create(
            builder.deref_mut(),
            &TelemetryArgs {
                per_component_latency: Some(per_component_latency),
                input_latency: engine_telemetry.input_latency,
                output_latency: engine_telemetry.output_latency,
            },
        );

        let chunk = Chunk::create(
            builder.deref_mut(),
            &ChunkArgs {
                control_type: ControlParam::Telemetry,
                control: Some(telemetry.as_union_value()),
                data: Some(sample_frame),
            },
        );
        builder.finish_minimal(chunk);
        let buf = builder.finish_message();

        self.stream.write_all(&buf)?;

        Ok(())
    }
}
