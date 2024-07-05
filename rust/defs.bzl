load("@rules_rust//rust:defs.bzl", _rust_library = "rust_library")

def rust_library(name, **kwargs):
  """Helper function for rust_library rule

  Args:
    name: Name of the crate.
    **kwargs: additional parameters of _rust_library
  """

  srcs_ = []
  if "srcs" in kwargs.keys():
    srcs_ = kwargs["srcs"]
  else:
    srcs_ = ["lib.rs"]
  
  _rust_library (
     name = name,
     srcs = srcs_,
     **kwargs,
  )