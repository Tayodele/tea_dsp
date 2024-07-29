workspace(name = "tea_dsp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

###### FlatBuffers ######

http_archive(
    name = "flatbuffers",
    integrity = "sha256-QVfFys21lzfF1ifkesJrFA6e4osRAvgSs2Boqrcowe0=",
    strip_prefix = "flatbuffers-24.3.25",
    urls = ["https://github.com/google/flatbuffers/archive/refs/tags/v24.3.25.tar.gz"],
)

load("//flatbuffers:deps.bzl", "flatbuffer_deps")

flatbuffer_deps()

register_toolchains(
    "//flatbuffers:flatc_mac_toolchain",
)

###### Rust ######

# To find additional information on this release or newer ones visit:
# https://github.com/bazelbuild/rules_rust/releases
http_archive(
    name = "rules_rust",
    integrity = "sha256-pt9MIrs/tDVzpMQIjvbQ+v44oOzQ+FrSH/2IiAAzcDA=",
    urls = ["https://github.com/bazelbuild/rules_rust/releases/download/0.44.0/rules_rust-v0.44.0.tar.gz"],
)

load("@rules_rust//rust:repositories.bzl", "rules_rust_dependencies", "rust_register_toolchains")

rules_rust_dependencies()

rust_register_toolchains(
    edition = "2021",
    versions = ["1.79.0"],
)

load("@rules_rust//crate_universe:repositories.bzl", "crate_universe_dependencies")

crate_universe_dependencies()

load("@rules_rust//crate_universe:defs.bzl", "crates_repository", "splicing_config")
load("crates.bzl", "EXTERNAL_CRATES")

crates_repository(
    name = "crates_io",
    cargo_lockfile = "//:Cargo.lock",
    isolated = False,
    lockfile = "//:Cargo.Bazel.lock",
    packages = EXTERNAL_CRATES,
    splicing_config = splicing_config(
        resolver_version = "2",
    ),
)

load("@crates_io//:defs.bzl", "crate_repositories")

crate_repositories()

###### Foreign Rules ######

http_archive(
    name = "rules_foreign_cc",
    sha256 = "4b33d62cf109bcccf286b30ed7121129cc34cf4f4ed9d8a11f38d9108f40ba74",
    strip_prefix = "rules_foreign_cc-0.11.1",
    url = "https://github.com/bazelbuild/rules_foreign_cc/releases/download/0.11.1/rules_foreign_cc-0.11.1.tar.gz",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

# This sets up some common toolchains for building targets. For more details, please see
# https://bazelbuild.github.io/rules_foreign_cc/0.11.1/flatten.html#rules_foreign_cc_dependencies
rules_foreign_cc_dependencies()

###### VST3 ######

load("//vst3:defs.bzl", "vst3_archive")

vst3_archive(
    name = "vst3_sdk",
    src = "//vst3:vst-sdk_3.7.11_build-10_2024-04-22.zip",
    build_file = "//vst3:vst3.BUILD.bazel",
)
