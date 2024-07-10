use io::{TcpSource, TcpSink}
use std::net::{TcpStream, SocketAddr};
use anyhoe::Context;
use std::thread::JoinHandle;

fn main() -> anyhow::Result<()> {

  tracing_subscriber::fmt::init();

  let ports = find_available_local_ports();
  let source_addr = SocketAddr::V4(SocketAddrV4::new(Ipv4Addr::new(127, 0, 0, 1), ports[0]));
  let sink_addr = SocketAddr::V4(SocketAddrV4(Ipv4Addr::new(127, 0, 0, 1), ports[1]));

  let engine = Command::new(env::var("ENGINE"))
  .args(["--source-addr", source_addr, "--sink-addr", sink_addr])
  .spawn()?;

  let stream_tx = TcpStream.connect(source_addr)?;
  let stream_rx_sock = TcpListener::bind(sink_addr)?;
  let (stream_rx, _addr) = stream_rx_sock.accept()?;

  let (command_tx, command_rx) = flume::bounded(512);
  let (response_tx, response_rx) = flume::bounded(512);

  let handle = thread::spawn(move || {
      stream_rx.set_read_timeout(Duration::from_millis(1));
      stream_rx.set_nodelay(true);
      stream_tx.set_write_timeout(Duration::from_millis(1));
      let mut builder = FlatBufferBuilder::with_capacity(2048);
      let buffer: Vec<u8> = vec![0; 3*1024*1024];
      let buf_idx: usize = 0;
      loop {
          if let Ok(app_data) = command_rx.try_recv() {
              AppState {
                  gain_db,
                  freq_hz,
              } = app_data;

              let control = SineGeneratorControl::create(
                  &mut builder,
                  &SineGeneratorControlArgs {
                      gain_db,
                      freq_hz,
                      phase_rad: 0.0,
                  },
              );
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
                      control_type: ControlParam::SineGeneratorControl,
                      control: Some(control.as_union_value()),
                      data: Some(sample_frame),
                  },
              );
              builder.finish_size_prefixed(chunk, None);
              let buf = builder.finished_data();
              stream_rx.write_all(buf)?;
          }

          stream_tx.read(&buf)
      }
      Ok(())
  })?;

  Ok(())
}

fn run_commander() -> anyhow::Result<JoinHandle> {

  Ok(handle)
}

fn run_blank_engine()  -> anyhow::Result<JoinHandle> {

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