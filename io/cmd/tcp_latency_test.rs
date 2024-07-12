//! Latency test using a bank engine.

use chunk::dsp::{
    Chunk, ChunkArgs, ControlParam, SampleFrame, SampleFrameArgs, TestControl, TestControlArgs,
};
use core::time::Duration;
use engine::io::{ChunkMessage, ChunkMessageBuilder};
use flatbuffers::FlatBufferBuilder;
use std::io::{Read, Write};
use std::net::{Ipv4Addr, SocketAddr, SocketAddrV4, TcpListener, TcpStream};
use std::process::Command;
use std::thread::{spawn, JoinHandle};

fn main() -> anyhow::Result<()> {
    tracing_subscriber::fmt::init();

    let ports = find_available_local_ports();
    let source_addr = SocketAddr::V4(SocketAddrV4::new(Ipv4Addr::new(127, 0, 0, 1), ports[0]));
    let sink_addr = SocketAddr::V4(SocketAddrV4::new(Ipv4Addr::new(127, 0, 0, 1), ports[1]));

    let _command_handle = run_commander(source_addr, sink_addr);
    for (key, val) in std::env::vars() {
        log::info!("{key}: {val}");
    }
    let test_engine = std::env::var("TEST_ENGINE").unwrap();
    let mut engine_handle = run_blank_engine(test_engine, source_addr, sink_addr);

    engine_handle.output()?;

    Ok(())
}

fn run_commander(source_addr: SocketAddr, sink_addr: SocketAddr) -> JoinHandle<anyhow::Result<()>> {
    spawn(move || {
        let mut stream_tx = TcpStream::connect(source_addr)?;
        let stream_rx_sock = TcpListener::bind(sink_addr)?;
        let (mut stream_rx, _addr) = stream_rx_sock.accept()?;

        stream_rx.set_read_timeout(Some(Duration::from_millis(10)))?;
        stream_rx.set_nodelay(true)?;
        stream_tx.set_write_timeout(Some(Duration::from_millis(10)))?;
        let mut builder = ChunkMessageBuilder::from(FlatBufferBuilder::with_capacity(2048));
        let mut buffer: Vec<u8> = vec![0; 3 * 1024 * 1024];
        let mut buf_idx: usize = 0;
        loop {
            let control = TestControl::create(&mut builder, &TestControlArgs {});
            let frame_buffer = vec![0.0_f32; 512];
            let samples = builder.create_vector(frame_buffer.as_slice());
            let sample_frame = SampleFrame::create(
                &mut builder,
                &SampleFrameArgs {
                    channels: 2,
                    sample_rate: 48_000.0,
                    samples: Some(samples),
                },
            );
            let chunk = Chunk::create(
                &mut builder,
                &ChunkArgs {
                    control_type: ControlParam::TestControl,
                    control: Some(control.as_union_value()),
                    data: Some(sample_frame),
                },
            );
            builder.finish_size_prefixed(chunk, None);
            let chunk_buf = builder.finish_message();
            stream_rx.write_all(chunk_buf)?;

            while let Ok(size) = stream_tx.read(&mut buffer[buf_idx..]) {
                let Some(message) = ChunkMessage::parse(buffer.as_slice()) else {
                    buf_idx += size;
                    continue;
                };
                buf_idx = 0;
                let chunk = message.get_chunk()?;
                let telem = chunk
                    .control_as_telemetry()
                    .ok_or(anyhow::anyhow!("Failed to get telemetry"))?;
                log::info!(
                    "input latency {}. output_latency {}",
                    telem.input_latency(),
                    telem.output_latency()
                );
                break;
            }
        }
    })
}

fn run_blank_engine(
    test_engine: String,
    source_addr: SocketAddr,
    sink_addr: SocketAddr,
) -> Command {
    let mut command = Command::new(test_engine);
    command
        .arg(format!("--source-addr={source_addr}"))
        .arg(format!("--sink-addr={sink_addr}"));
    command
}

fn find_available_local_ports() -> [u16; 2] {
    let mut ports = [0; 2];
    let mut idx = 0;
    for port in 8000..10000 {
        if TcpListener::bind(("127.0.0.1", port)).is_ok() {
            ports[idx] = port;
            idx += 1;
        }
        if idx == 2 {
            break;
        }
    }
    ports
}
