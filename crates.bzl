load("@rules_rust//crate_universe:defs.bzl", "crate")

EXTERNAL_CRATES = {
    "anyhow": crate.spec(
        version = "1",
    ),
    "ctrlc": crate.spec(
        version = "3",
    ),
    "flatbuffers": crate.spec(
        version = "24",
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
    "thiserror": crate.spec(
        version = "1",
    ),
}
