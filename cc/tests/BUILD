# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library

cc_test(
  name = "test-common",
  size = "small",
  srcs = ["common/TeaCommon_unittest.cpp"],
  deps = ["@googletest//googletest-release-1.10.0:gtest",
          "@googletest//googletest-release-1.10.0:gtest_main",
          "//common"],
)