load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def flatbuffer_deps():
    http_archive(
        name = "flatc_mac",
        integrity = "sha256-J3J09OEDfbtXsblXGXIf49WMhpg9Y0EDKErYwdnPGd0=",
        urls = ["https://github.com/google/flatbuffers/releases/download/v24.3.25/Mac.flatc.binary.zip"],
        build_file_content = """
load("@bazel_skylib//rules:native_binary.bzl", "native_binary")

package(default_visibility = ["//visibility:public"])

native_binary(
  name = "bin",
  src = "flatc",
  out = "flatc.bin"
)
        """,
    )
