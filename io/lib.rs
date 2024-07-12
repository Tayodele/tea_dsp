//! Implementations of `EngineSource` and `EngineSink`

use chunk::dsp::{
    root_as_chunk, Chunk, ChunkArgs, ControlParam, SampleFrame, SampleFrameArgs, Telemetry,
    TelemetryArgs,
};
use engine::io::ChunkMessageBuilder;
use engine::{EngineSink, EngineSource, EngineTelemetry, Frame};
use std::io::{Read, Write};
use std::net;
use std::ops::DerefMut;

const MAX_CHUNK_SIZE: usize = 3 * 1024 * 1024;

pub struct TcpSource {
    _socket: net::TcpListener,
    stream: net::TcpStream,
    addr: net::SocketAddr,
    chunk_buffer: Box<[u8]>,
}

impl TcpSource {
    pub fn open(addr: impl net::ToSocketAddrs) -> anyhow::Result<Self> {
        let socket = net::TcpListener::bind(&addr)?;
        let (stream, addr) = socket.accept()?;
        Ok(Self {
            _socket: socket,
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
    _addr: net::SocketAddr,
}

impl TcpSink {
    pub fn open(addr: net::SocketAddr) -> anyhow::Result<Self> {
        let stream = net::TcpStream::connect(&addr)?;
        stream.set_nodelay(true)?;
        Ok(Self {
            stream,
            _addr: addr,
        })
    }
}

impl EngineSink for TcpSink {
    fn send_buffer(
        &mut self,
        builder: &mut ChunkMessageBuilder,
        frame: &Frame,
        telemetry: &EngineTelemetry,
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
            builder.create_vector(telemetry.per_component_latency.as_slice());
        let telemetry = Telemetry::create(
            builder.deref_mut(),
            &TelemetryArgs {
                per_component_latency: Some(per_component_latency),
                input_latency: telemetry.input_latency,
                output_latency: telemetry.output_latency,
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
        builder.finish_size_prefixed(chunk, None);
        let buf = builder.finish_message();

        self.stream.write_all(&buf)?;

        Ok(())
    }
}
