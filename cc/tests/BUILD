# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library

cc_test(
  name = "test-common_conversions",
  size = "small",
  srcs = ["common_conversions/TeaCommon_unittest.cpp"],
  deps = ["@googletest//googletest-release-1.10.0:gtest",
          "@googletest//googletest-release-1.10.0:gtest_main",
          "//cc:common_conversions"],
)