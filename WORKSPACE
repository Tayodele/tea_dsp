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
    integrity = "sha256-F8U7+AC5MvMtPKGdLLnorVM84cDXKfDRgwd7/dq3rUY=",
    urls = ["https://github.com/bazelbuild/rules_rust/releases/download/0.46.0/rules_rust-v0.46.0.tar.gz"],
)

load("@rules_rust//rust:repositories.bzl", "rules_rust_dependencies", "rust_register_toolchains")

rules_rust_dependencies()

rust_register_toolchains(
    edition = "2021",
    versions = ["1.79.0"],
)

load("@rules_rust//crate_universe:repositories.bzl", "crate_universe_dependencies")

crate_universe_dependencies()

load("@rules_rust//crate_universe:defs.bzl", "crates_repository", "render_config")
load("crates.bzl", "EXTERNAL_CRATES")

crates_repository(
    name = "crates_io",
    cargo_lockfile = "//:Cargo.lock",
    lockfile = "//:Cargo.Bazel.lock",
    packages = EXTERNAL_CRATES,
    # Setting the default package name to `""` forces the use of the macros defined in this repository
    # to always use the root package when looking for dependencies or aliases. This should be considered
    # optional as the repository also exposes alises for easy access to all dependencies.
    render_config = render_config(
        default_package_name = "",
    ),
)

load("@crates_io//:defs.bzl", "crate_repositories")

crate_repositories()
