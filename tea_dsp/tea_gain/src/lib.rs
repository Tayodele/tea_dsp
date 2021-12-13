use dasp_sample::Sample;

pub fn process<T: Sample<Float = f32> + dasp_sample::FromSample<f32>>(buffer: T, gain: f32) -> T {
    let db_gain = fader(gain).to_float_sample();
    buffer.to_float_sample().mul_amp(db_gain).to_sample::<T>()
}

fn fader(db: f32) -> f32 {
    (10.0_f32).powf(db / 20.0)
}
