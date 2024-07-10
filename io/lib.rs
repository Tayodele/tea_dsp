//! Implementations of `EngineSource` and `EngineSink`

use chunk::dsp::{
    root_as_chunk, Chunk, ChunkArgs, ControlParam, SampleFrame, SampleFrameArgs, Telemetry,
    TelemetryArgs,
};
use engine::{EngineSink, EngineSource, Frame};
use std::io::{Read, Write};
use std::net;

const MAX_CHUNK_SIZE: usize = 3 * 1024 * 1024;

pub struct TcpSource {
    socket: net::TcpListener,
    stream: net::TcpStream,
    addr: net::SocketAddr,
    chunk_buffer: Box<[u8]>,
}

impl TcpSource {
    pub fn open(addr: impl net::ToSocketAddrs) -> anyhow::Result<Self> {
        let socket = net::TcpListener::bind(&addr)?;
        let (stream, addr) = socket.accept()?;
        Ok(Self {
            socket,
            stream,
            addr,
            chunk_buffer: vec![0; MAX_CHUNK_SIZE].into_boxed_slice(),
        })
    }

    pub fn get_addr(&self) -> net::SocketAddr {
        self.addr
    }
}

impl EngineSource for TcpSource {
    fn read_buffer<'chunk>(&'chunk mut self) -> anyhow::Result<Chunk<'chunk>> {
        let size = self.stream.read(&mut self.chunk_buffer)?;
        let raw_chunk = &self.chunk_buffer[..size];
        let chunk = root_as_chunk(raw_chunk)?;
        Ok(chunk)
    }
}

pub struct TcpSink {
    stream: net::TcpStream,
    addr: net::SocketAddr,
}

impl TcpSink {
    pub fn open(addr: net::SocketAddr) -> anyhow::Result<Self> {
        let stream = net::TcpStream::connect(&addr)?;
        stream.set_nodelay(true);
        Ok(Self { stream, addr })
    }
}

impl EngineSink for TcpSink {
    fn send_buffer(&mut self, frame: &Frame) -> anyhow::Result<()> {
        let mut builder = flatbuffers::FlatBufferBuilder::with_capacity(1024);

        let samples = builder.create_vector(&frame.samples);
        let sample_frame = SampleFrame::create(
            &mut builder,
            &SampleFrameArgs {
                channels: frame.channels as i16,
                sample_rate: frame.sample_rate,
                samples: Some(samples),
            },
        );
        let per_component_latency = builder.create_vector(&[0]);
        let telemetry = Telemetry::create(
            &mut builder,
            &TelemetryArgs {
                per_component_latency: Some(per_component_latency),
                input_latency: 0,
                output_latency: 0,
            },
        );

        let chunk = Chunk::create(
            &mut builder,
            &ChunkArgs {
                control_type: ControlParam::Telemetry,
                control: Some(telemetry.as_union_value()),
                data: Some(sample_frame),
            },
        );
        builder.finish_size_prefixed(chunk, None);
        let buf = builder.finished_data();

        self.stream.write_all(&buf)?;

        Ok(())
    }
}
