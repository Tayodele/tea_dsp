use dasp_sample::Sample;
use std::io;
use tea_gain::process;

fn main() {
    println!("This is the teatime amp gain module");
    let mut test_sample = 50.0_f32.to_float_sample();
    loop {
        println!("The sample value is currently {}", test_sample);

        let mut fader_in = String::new();

        println!("enter fader value: ");
        io::stdin()
            .read_line(&mut fader_in)
            .expect("Failed to read fader");

        let fader_value: f32 = fader_in.trim().parse().unwrap_or(0.0);

        test_sample = process::<f32>(test_sample, fader_value);
    }
}
