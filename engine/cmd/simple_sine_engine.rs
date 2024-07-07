use engine::run_engine;
use engine::sine_generator::SineGeneratorComponent;
use io::{TcpSink, TcpSource};
use log::info;

fn main() -> anyhow::Result<()> {
    tracing_subscriber::fmt::init();
    info!("Starting simple sine engine");

    let mut source = TcpSource::open()?;
    let mut sink = TcpSink::open(source.get_addr())?;
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
