# BUILD file for use with https://github.com/dejwk/roo_testing.

cc_library(
    name = "roo_threads",
    visibility = ["//visibility:public"],
    srcs = glob(
        [
            "src/**/*.h",
        ],
        exclude = ["test/**"],
    ),
    includes = [
        "src",
    ],
    defines = [
        "ROO_TESTING",
    ],
    deps = [
        "//roo_testing:arduino",
        "//roo_testing/sys",
        "//lib/roo_time",
    ],
)
