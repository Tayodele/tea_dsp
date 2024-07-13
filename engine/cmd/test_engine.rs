use clap::Parser;
use engine::run_engine;
use engine::sine_generator::SineGeneratorComponent;
use io::TcpSource;
use log::info;
use std::net;

#[derive(Parser)]
struct Args {
    #[clap(long)]
    source_addr: net::SocketAddrV4,
}

fn main() -> anyhow::Result<()> {
    let args = Args::parse();

    tracing_subscriber::fmt::init();
    info!("Starting test engine");

    log::info!("Connecting to source {}", args.source_addr);
    let mut stream = TcpSource::open(net::SocketAddr::V4(args.source_addr))?;

    run_engine(
        &mut stream,
        Vec::<SineGeneratorComponent>::new().as_mut_slice(),
        44_100.0,
        2,
    )?;

    Ok(())
}
