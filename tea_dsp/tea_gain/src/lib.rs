use dasp_sample::Sample;

pub fn process<T: Sample<Float = f32> + dasp_sample::FromSample<f32>>(buffer: T, gain: f32) -> T {
    let db_gain = fader(gain).to_float_sample();
    buffer.to_float_sample().mul_amp(db_gain).to_sample::<T>()
}

fn fader(db: f32) -> f32 {
    (10.0_f32).powf(db / 20.0)
}

#[cfg(test)]
mod tests {
    use super::*;
    use rstest::rstest;

    #[rstest]
    #[case(-1.0,"lt")]
    #[case(1.0, "gt")]
    #[case(0.0, "eq")]
    fn change_amplitude(#[case] gain_db: f32, #[case] result: &str) {
        let sample: f32 = 100.0;
        let sample_proc = process(sample.to_float_sample(), gain_db);
        match result {
            "lt" => assert!(sample_proc < sample),
            "gt" => assert!(sample_proc > sample),
            "eq" => assert!(sample_proc == sample),
            _ => (),
        };
    }

    #[test]
    fn into_oblivion() {
        let sample: f32 = 100.0;
        let sample_proc = process(sample.to_float_sample(), -1_000_000.0);
        assert_eq!(sample_proc, 0.0);
    }
}
