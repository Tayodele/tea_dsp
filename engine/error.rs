/// Error in the engine.
#[derive(thiserror::Error, Debug)]
pub enum AudioError {
    #[error("Missing expected value {0} from Chunk")]
    MissingField(String),
    #[error("Missing control parameter: {0}")]
    MissingControl(String),
    #[error("Error reading from sink or source: {0}")]
    IOError(#[from] anyhow::Error),
}

pub type AudioResult<T> = Result<T, AudioError>;
