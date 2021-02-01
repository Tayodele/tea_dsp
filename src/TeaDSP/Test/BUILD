# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library
cc_library(
  name = "TeaGenerator",
  srcs = ["TeaGenerator.cpp"],
  hdrs = ["TeaGenerator.h"],
  deps = ["//TeaCommon:TeaCommon"],
)

cc_test(
  name = "test-lfo",
  size = "small",
  srcs = ["TeaLFO_unittest.cpp"],
  deps = ["@googletest//googletest-release-1.10.0:gtest",
          "@googletest//googletest-release-1.10.0:gtest_main",
          "//TeaLFO:TeaLFO",
          "//TeaCommon:TeaCommon"],
)

cc_test(
  name = "test-common",
  size = "small",
  srcs = ["TeaCommon_unittest.cpp"],
  deps = ["@googletest//googletest-release-1.10.0:gtest",
          "@googletest//googletest-release-1.10.0:gtest_main",
          "//TeaCommon:TeaCommon"],
)


cc_test(
  name = "test-fir",
  size = "small",
  srcs = ["TeaFIR_unittest.cpp"],
  deps = ["@googletest//googletest-release-1.10.0:gtest",
          "@googletest//googletest-release-1.10.0:gtest_main",
          "//TeaFIR:TeaFIR",
          "//TeaCommon:TeaCommon"],
)

