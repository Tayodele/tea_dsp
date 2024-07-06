/// Error in the engine.
#[derive(thiserror::Error, Debug)]
pub enum AudioError {}

pub type AudioResult<T> = Result<T, AudioError>;
