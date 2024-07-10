use clap::Parser;
use engine::run_engine;
use engine::sine_generator::SineGeneratorComponent;
use io::{TcpSink, TcpSource};
use log::info;
use std::net;

#[derive(Parser)]
struct Args {
    #[clap(long)]
    source_addr: net::SocketAddrV4,
    #[clap(long)]
    sink_addr: net::SocketAddrV4,
}

fn main() -> anyhow::Result<()> {
    let args = Args::parse();

    tracing_subscriber::fmt::init();
    info!("Starting simple sine engine");

    let mut source = TcpSource::open(net::SocketAddr::V4(args.sink_addr))?;
    let mut sink = TcpSink::open(net::SocketAddr::V4(args.sink_addr))?;
    let mut components = vec![SineGeneratorComponent::default()];

    run_engine(
        &mut source,
        &mut sink,
        components.as_mut_slice(),
        441000.0,
        2,
    )?;

    Ok(())
}
