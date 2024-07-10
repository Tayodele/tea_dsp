load("@rules_rust//crate_universe:defs.bzl", "crate")

EXTERNAL_CRATES = {
    "anyhow": crate.spec(
        version = "1",
    ),
    "byteorder": crate.spec(
        version = "1",
    ),
    "ctrlc": crate.spec(
        version = "3",
    ),
    "clap": crate.spec(
        version = "3",
        features = ["derive"],
    ),
    "eframe": crate.spec(
        version = "0",
        default_features = False,
        features = [
            "default_fonts",
            "glow",
        ],
    ),
    "egui": crate.spec(
        version = "0",
        default_features = False,
    ),
    "flatbuffers": crate.spec(
        version = "24",
    ),
    "flume": crate.spec(
        version = "0",
    ),
    "log": crate.spec(
        version = "0",
    ),
    "num": crate.spec(
        version = "0",
    ),
    "rodio": crate.spec(
        version = "0",
    ),
    "rstest": crate.spec(
        version = "0",
        default_features = False,
    ),
    "serde": crate.spec(
        version = "1",
        default_features = False,
        features = ["derive"],
    ),
    "thiserror": crate.spec(
        version = "1",
    ),
    "tracing-subscriber": crate.spec(
        version = "0",
    ),
}
