use math;

fn volume(sample: f32, decibels: f32) -> f32 {
    let step1: f32 = math::pow(10, decibels / 10.0);
    sample * step1
}
