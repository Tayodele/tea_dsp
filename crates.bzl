load("@rules_rust//crate_universe:defs.bzl", "crate")

EXTERNAL_CRATES = {
    "flatbuffers": crate.spec(
        version = "24",
    ),
    "thiserror": crate.spec(
        version = "1",
    ),
    "rodio": crate.spec(
        version = "0",
    ),
    "log": crate.spec(
        version = "0",
    ),
    "ctrlc": crate.spec(
        version = "3",
    ),
    "num": crate.spec(
        version = "0",
    ),
}
