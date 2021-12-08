# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library
cc_library(
  name = "TeaTest",
  srcs = glob(["*.cpp"]),
  hdrs = glob(["*.h"]),
  deps = ["@googletest//googletest-release-1.10.0:gtest",
          "@googletest//googletest-release-1.10.0:gtest_main",
          "//TeaCommon:TeaCommon"],
  visibility = ["//Test:__pkg__"],
)

