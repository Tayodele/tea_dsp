//! Latency test using a bank engine.

use chunk::dsp::{
    Chunk, ChunkArgs, ControlParam, SampleFrame, SampleFrameArgs, TestControl, TestControlArgs,
};
use core::time::Duration;
use engine::io::{ChunkMessage, ChunkMessageBuilder};
use flatbuffers::FlatBufferBuilder;
use std::io::{Read, Write};
use std::net::{Ipv4Addr, SocketAddr, SocketAddrV4, TcpListener};
use std::process::Command;
use std::thread::{spawn, JoinHandle};

fn main() -> anyhow::Result<()> {
    tracing_subscriber::fmt::init();

    let source_addr = SocketAddr::V4(SocketAddrV4::new(Ipv4Addr::new(127, 0, 0, 1), 0));
    let commander_sock = TcpListener::bind(source_addr)?;
    let source_addr = commander_sock.local_addr()?;
    log::info!("Connecting to {source_addr}");

    log::info!("Starting Commander");
    let command_handle = run_commander(commander_sock);
    let test_engine = std::env::var("TEST_ENGINE").unwrap();
    let mut engine_handle = run_blank_engine(test_engine, source_addr).spawn()?;
    log::info!("Starting Engine");
    if let Ok(result) = command_handle.join() {
        if let Err(e) = result {
            log::error!("{}", e);
            return Err(e);
        }
    } else {
        log::error!("Failed to spawn commander");
    }
    match engine_handle.wait() {
        Err(e) => {
            log::error!("{}", e);
            return Err(e.into());
        }
        Ok(_) => (),
    }
    log::info!("Engine exited");

    Ok(())
}

fn run_commander(commander_sock: TcpListener) -> JoinHandle<anyhow::Result<()>> {
    spawn(move || {
        let (mut stream, source_addr) = commander_sock.accept()?;
        log::info!("Connected to {source_addr}");

        // stream_rx.set_read_timeout(Some(Duration::from_millis(10)))?;
        stream.set_nonblocking(false)?;
        // stream_rx.set_nodelay(true)?;
        stream.set_ttl(64)?;

        let mut builder = ChunkMessageBuilder::from(FlatBufferBuilder::with_capacity(2048));
        let mut buffer: Vec<u8> = vec![0; 3 * 1024 * 1024];
        let mut buf_idx: usize = 0;
        loop {
            builder.reset();
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
            builder.finish_minimal(chunk);
            let chunk_buf = builder.finish_message();
            stream.write_all(chunk_buf)?;

            while let Ok(size) = stream.read(&mut buffer[buf_idx..]) {
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

fn run_blank_engine(test_engine: String, source_addr: SocketAddr) -> Command {
    let mut command = Command::new(test_engine);
    command
        .arg(format!("--source-addr={source_addr}"))
        .stdout(std::process::Stdio::inherit())
        .stderr(std::process::Stdio::inherit());
    command
}
