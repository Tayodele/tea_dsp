use byteorder::{LittleEndian, ReadBytesExt, WriteBytesExt};
use chunk::dsp::{root_as_chunk, Chunk};
use std::io::Write;

const MESSAGE_PREFIX_STRING: &'static str = "TEADSPCHUNK";
const MESSAGE_PREFIX_LEN: usize = MESSAGE_PREFIX_STRING.len() + core::mem::size_of::<u32>();

#[derive(Debug, PartialEq)]
pub struct ChunkMessage<'chunk> {
    raw: &'chunk [u8],
}

impl<'chunk> ChunkMessage<'chunk> {
    /// Traveses a buffer byte fro byte looking for a matching string, then if found returns a `ChunkMessage`.
    pub fn parse(buf: &'chunk [u8]) -> Option<Self> {
        let prefix = MESSAGE_PREFIX_STRING.as_bytes();
        for idx in 0..buf.len() {
            let prefix_end = idx+MESSAGE_PREFIX_STRING.len();
            if prefix_end > buf.len() {
              break;
            }
            let prefix_buf = &buf[idx..prefix_end];
            if prefix != prefix_buf {
                continue;
            }
            let size = (&buf[prefix_end..prefix_end + 4]).read_u32::<LittleEndian>().ok()? as usize;
            let body_start = idx + MESSAGE_PREFIX_LEN;
            if body_start + size > buf.len() {
                return None;
            }
            return Some(Self {
                raw: &buf[body_start..body_start+size],
            });
        }

        None
    }

    pub fn get_prefix(&self) -> [u8; MESSAGE_PREFIX_LEN] {
        let mut buf = [0; MESSAGE_PREFIX_LEN];
        let mut cur = std::io::Cursor::new(buf.as_mut_slice());
        // This should never fail.
        cur.write_all(MESSAGE_PREFIX_STRING.as_bytes()).unwrap();
        cur.write_u32::<LittleEndian>(self.raw.len() as u32)
            .unwrap();
        buf
    }

    pub fn get_chunk(&self) -> anyhow::Result<Chunk<'chunk>, flatbuffers::InvalidFlatbuffer> {
        root_as_chunk(self.raw)
    }
}

impl<'chunk> From<&'chunk [u8]> for ChunkMessage<'chunk> {
    fn from(raw: &'chunk [u8]) -> Self {
        Self { raw }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use rstest::rstest;

    #[test]
    fn test_get_prefix() {
        let buf = vec![1, 2, 3, 4];
        let message = ChunkMessage {
            raw: buf.as_slice(),
        };
        let prefix = message.get_prefix();
        assert_eq!(&prefix[prefix.len() - 4..prefix.len()], 4_u32.to_le_bytes());
    }

    #[rstest]
    #[case(
      vec![1, 2, 3, 4], None
    )]
    #[case(
      vec![MESSAGE_PREFIX_STRING.as_bytes().to_vec(), 4_u32.to_le_bytes().to_vec(), vec![1, 2, 3, 4]].concat(), 
      Some(ChunkMessage {
        raw: &[1, 2, 3, 4]
      })
    )]
    #[case(
      vec![vec![2,3,4], MESSAGE_PREFIX_STRING.as_bytes().to_vec(), 4_u32.to_le_bytes().to_vec(), vec![1, 2, 3, 4]].concat(), 
      Some(ChunkMessage {
        raw: &[1, 2, 3, 4]
      })
    )]
    fn test_parse(#[case] buf: Vec<u8>, #[case] expected_message: Option<ChunkMessage<'_>>) {
        assert_eq!(ChunkMessage::parse(buf.as_slice()), expected_message);
    }
}
