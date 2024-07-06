load("@rules_rust//rust:defs.bzl", _rust_library = "rust_library")

def rust_library(name, srcs = None, **kwargs):
    """Helper function for rust_library rule

    Args:
      name: Name of the crate.
      srcs: .rs files to compile with crate.
      **kwargs: additional parameters of _rust_library
    """

    if srcs == None:
        srcs = native.glob(["*.rs"])
    _rust_library(
        name = name,
        srcs = srcs,
        **kwargs
    )
