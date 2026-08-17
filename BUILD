# BUILD file for use with https://github.com/dejwk/roo_testing.

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_library(
    name = "roo_threads",
    srcs = glob(
        [
            "src/**/*.h",
            "src/**/*.cpp",
        ],
        exclude = ["test/**"],
    ),
    includes = [
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@roo_time",
    ] + select({
        "@roo_testing//roo_testing/platforms:is_roo_testing": [
            "@roo_testing//roo_testing/sys",
        ],
        "//conditions:default": [],
    }) + select({
        "@roo_testing//roo_testing/platforms:is_arduino": [
            "@roo_testing//:arduino",
        ],
        "//conditions:default": [],
    }),
)

cc_test(
    name = "mutex_test",
    srcs = ["test/mutex_test.cpp"],
    linkstatic = 1,
    deps = [
        ":roo_threads",
        "@roo_testing//roo_testing:arduino_gtest_main",
    ],
)

cc_test(
    name = "condition_variable_test",
    srcs = ["test/condition_variable_test.cpp"],
    linkstatic = 1,
    deps = [
        ":roo_threads",
        "@roo_testing//roo_testing:arduino_gtest_main",
    ],
)

cc_test(
    name = "thread_test",
    srcs = ["test/thread_test.cpp"],
    linkstatic = 1,
    deps = [
        ":roo_threads",
        "@roo_testing//roo_testing:arduino_gtest_main",
    ],
)
