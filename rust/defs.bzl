load("@rules_rust//rust:defs.bzl", _rust_binary = "rust_binary", _rust_library = "rust_library", _rust_test = "rust_test")

def rust_library(name, srcs = None, test_deps = None, **kwargs):
    """Helper function for rust_library rule

    Args:
      name: Name of the crate.
      srcs: .rs files to compile with crate.
      test_deps: Dependencies that should only be considered for the test.
      **kwargs: additional parameters of _rust_library
    """

    if srcs == None:
        srcs = native.glob(["*.rs"])
    if test_deps == None:
        test_deps = []

    _rust_library(
        name = name,
        srcs = srcs,
        **kwargs
    )

    _rust_test(
        name = "{}.test".format(name),
        crate = ":{}".format(name),
        deps = kwargs.get("deps", []) + test_deps,
    )

def rust_binary(name, srcs = None, **kwargs):
    """Helper function for rust_binary rule

    Args:
      name: Name of the crate.
      srcs: .rs files to compile with crate.
      **kwargs: additional parameters of _rust_library
    """

    if srcs == None:
        srcs = ["{}.rs".format(name)]

    _rust_binary(
        name = name,
        srcs = srcs,
        deps = kwargs.get("deps", []),
    )
