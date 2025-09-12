# BUILD file for use with https://github.com/dejwk/roo_testing.

cc_library(
    name = "roo_threads",
    srcs = glob(
        [
            "src/**/*.h",
            "src/**/*.cpp",
        ],
        exclude = ["test/**"],
    ),
    defines = [
        "ROO_TESTING",
    ],
    includes = [
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@roo_testing//roo_testing:arduino",
        "@roo_testing//roo_testing/sys",
        "@roo_time",
    ],
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
