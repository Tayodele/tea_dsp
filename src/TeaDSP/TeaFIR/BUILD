cc_library(
    name = "TeaFIRDynamic",
    srcs = glob(["*Dynamic.cpp"]),
    hdrs = glob(["*Dynamic.h"]),
    deps = ["//TeaCommon:TeaCommon","@fftw3//:fftw3.so"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "TeaFIRStatic",
    srcs = glob(["*Static.cpp"]),
    hdrs = glob(["*Static.h"]),
    deps = ["//TeaCommon:TeaCommon"],
    visibility = ["//visibility:public"],
)
