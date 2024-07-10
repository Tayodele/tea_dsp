//! This example illustrates how to create a button that changes color and text based on its
//! interaction state.

use egui;
use std::net::{Ipv4Addr, SocketAddr, SocketAddrV4, TcpListener, TcpStream};
use std::process::Command;
use std::env;
use std::thread;
use std::io::{Write, Read};
use chunk::dsp::{
    root_as_chunk, Chunk, ChunkArgs, ControlParam, SampleFrame, SampleFrameArgs,
    SineGeneratorControl, SineGeneratorControlArgs,
};
use flatbuffers::FlatBufferBuilder;

fn main() -> anyhow::Result<()> {
    tracing_subscriber::fmt::init();

    let ports = find_available_local_ports();
    let source_addr = SocketAddr::V4(SocketAddrV4::new(Ipv4Addr::new(127, 0, 0, 1), ports[0]));
    let sink_addr = SocketAddr::V4(SocketAddrV4(Ipv4Addr::new(127, 0, 0, 1), ports[1]));

    let engine = Command::new(env::var("ENGINE"))
    .args(["--source-addr", source_addr, "--sink-addr", sink_addr])
    .spawn()?;

    let stream_tx = TcpStream.connect(source_addr)?;
    let stream_rx_sock = TcpListener::bind(sink_addr)?;
    let (stream_rx, _addr) = stream_rx_sock.accept()?;

    let (command_tx, command_rx) = flume::bounded(512);
    let (response_tx, response_rx) = flume::bounded(512);

    let handle = thread::spawn(move || {
        stream_rx.set_read_timeout(Duration::from_millis(1));
        stream_rx.set_nodelay(true);
        stream_tx.set_write_timeout(Duration::from_millis(1));
        let mut builder = FlatBufferBuilder::with_capacity(2048);
        let buffer: Vec<u8> = vec![0; 3*1024*1024];
        let buf_idx: usize = 0;
        loop {
            if let Ok(app_data) = command_rx.try_recv() {
                AppState {
                    gain_db,
                    freq_hz,
                } = app_data;

                let control = SineGeneratorControl::create(
                    &mut builder,
                    &SineGeneratorControlArgs {
                        gain_db,
                        freq_hz,
                        phase_rad: 0.0,
                    },
                );
                let frame_buffer = vec![0.0_f32; 512];
                let samples = builder.create_vector(frame_buffer.as_slice());
                let sample_frame = SampleFrame::create(
                    &mut builder,
                    &SampleFrameArgs {
                        channels: 2,
                        sample_rate: 48_000.0,
                        samples: Some(samples),
                    },
                );
                let chunk = Chunk::create(
                    &mut builder,
                    &ChunkArgs {
                        control_type: ControlParam::SineGeneratorControl,
                        control: Some(control.as_union_value()),
                        data: Some(sample_frame),
                    },
                );
                builder.finish_size_prefixed(chunk, None);
                let buf = builder.finished_data();
                stream_rx.write_all(buf)?;
            }

            stream_tx.read(&buf)
        }
        Ok(())
    })?;

    let native_options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default()
            .with_inner_size([400.0, 300.0])
            .with_min_inner_size([300.0, 220.0]),
        ..Default::default()
    };
    let res = eframe::run_native(
        "eframe template",
        native_options,
        Box::new(|cc| Ok(Box::new(SimpleSineApp::new(cc, )))),
    );

    engine.kill();

    res
}

fn find_available_local_ports() -> [u16; 2] {
    let mut ports = [0; 2];
    let mut idx = 0;
    for port in 8000..10000 {
        if TcpListener::bind(("127.0.0.1", port)).is_ok() {
            ports[idx] = port;
            idx += 1;
        }
        if idx == 2 {
            break;
        }
    }
    ports
}

#[derive(Default)]
pub struct SimpleSineApp {
    gain_db: f32,
    freq_hz: f32,
    engine_stream: Option<TcpStream>,
}

struct AppState {
    gain_db: f32,
    freq_hz: f32,
}

impl SimpleSineApp {
    /// Called once before the first frame.
    pub fn new(_cc: &eframe::CreationContext<'_>, engine_stream: TcpStream) -> Self {
        Self {
            engine_stream: Some(engine_stream),
            Default..default()
        }
    }

    fn state(&self) -> AppState {
        AppState {
            gain_db: self.gain_db,
            freq_hz: self.freq_hz,
        }
    }
}

impl eframe::App for SimpleSineApp {
    /// Called each time the UI needs repainting, which may be many times per second.
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        // Put your widgets into a `SidePanel`, `TopBottomPanel`, `CentralPanel`, `Window` or `Area`.
        // For inspiration and more examples, go to https://emilk.github.io/egui

        egui::TopBottomPanel::top("top_panel").show(ctx, |ui| {
            // The top panel is often a good place for a menu bar:

            egui::menu::bar(ui, |ui| {
                // NOTE: no File->Quit on web pages!
                let is_web = cfg!(target_arch = "wasm32");
                if !is_web {
                    ui.menu_button("File", |ui| {
                        if ui.button("Quit").clicked() {
                            ctx.send_viewport_cmd(egui::ViewportCommand::Close);
                        }
                    });
                    ui.add_space(16.0);
                }

                egui::widgets::global_dark_light_mode_buttons(ui);
            });
        });

        egui::CentralPanel::default().show(ctx, |ui| {
            // The central panel the region left after adding TopPanel's and SidePanel's
            ui.heading("Simple Sine App");

            ui.add(egui::Slider::new(&mut self.value, 0.0..=10.0).text("value"));
            if ui.button("Fire").clicked() {
                play_sine_wave();
            }

            ui.with_layout(egui::Layout::bottom_up(egui::Align::LEFT), |ui| {
                powered_by_egui_and_eframe(ui);
                egui::warn_if_debug_build(ui);
            });
        });
    }
}

fn play_sine_wave() {}

fn powered_by_egui_and_eframe(ui: &mut egui::Ui) {
    ui.horizontal(|ui| {
        ui.spacing_mut().item_spacing.x = 0.0;
        ui.label("Powered by ");
        ui.hyperlink_to("egui", "https://github.com/emilk/egui");
        ui.label(" and ");
        ui.hyperlink_to(
            "eframe",
            "https://github.com/emilk/egui/tree/master/crates/eframe",
        );
        ui.label(".");
    });
}
