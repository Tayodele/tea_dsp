pub mod gain {
  use num_traits::Pow;

  #[no_mangle] 
  pub extern "C" fn volume(sample: f32, decibels: f32) -> f32 {
    let step1: f32 = Pow::pow(10.0f32, decibels / 10.0f32);
    sample * step1
  }
}

#[cfg(test)]
mod tests {
  use super::gain::*;
  use assert_float_eq::*;

  #[test]
  fn test_volume() {
    assert_float_relative_eq!(volume(100.0, 0.0), 100.0);
    assert_float_relative_eq!(volume(100.0, -3.0), 50.11872);
    assert!(volume(100.0, -100.0) < 1e-6);
  }
}