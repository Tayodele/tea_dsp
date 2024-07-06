load("@rules_rust//rust:defs.bzl", _rust_library = "rust_library", _rust_test = "rust_test")

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
