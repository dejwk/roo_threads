# BUILD file for use with https://github.com/dejwk/roo_testing.

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

TEST_MAIN_DEPS = select({
    "@roo_testing//roo_testing/platforms:is_arduino": [
        "@roo_testing//:arduino_gtest_main",
    ],
    "@roo_testing//roo_testing/platforms:is_idf": [
        "@roo_testing//:esp_idf_gtest_main",
    ],
    "@roo_testing//roo_testing/platforms:is_pico": ["@roo_testing//:pico_gtest_main"],
    "//conditions:default": [],
})

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
        "@roo_testing//roo_testing/platforms:is_freertos": [
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
    target_compatible_with = select({
        "@roo_testing//roo_testing/platforms:is_bare_metal": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    deps = [":roo_threads"] + TEST_MAIN_DEPS,
)

cc_test(
    name = "condition_variable_test",
    srcs = ["test/condition_variable_test.cpp"],
    linkstatic = 1,
    target_compatible_with = select({
        "@roo_testing//roo_testing/platforms:is_bare_metal": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    deps = [":roo_threads"] + TEST_MAIN_DEPS,
)

cc_test(
    name = "thread_test",
    srcs = ["test/thread_test.cpp"],
    linkstatic = 1,
    target_compatible_with = select({
        "@roo_testing//roo_testing/platforms:is_bare_metal": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    deps = [":roo_threads"] + TEST_MAIN_DEPS,
)

cc_test(
    name = "backend_selection_test",
    srcs = ["test/backend_selection_test.cpp"],
    linkstatic = True,
    deps = [":roo_threads"] + TEST_MAIN_DEPS,
)

# Pure preprocessor contract: RP2040 identity must not force bare-metal support.
# Runtime integration is covered separately by pico_freertos_*_test.
cc_library(
    name = "rp2040_freertos_selection_probe",
    srcs = ["test/rp2040_freertos_selection_probe.cpp"],
    hdrs = ["src/roo_threads/impl/resolve.h"],
    includes = ["src"],
    local_defines = [
        "ROO_TESTING=1",
        "ROO_TESTING_RUNTIME_FREERTOS=1",
        "ARDUINO_ARCH_RP2040=1",
    ],
    target_compatible_with = select({
        "@roo_testing//roo_testing/platforms:is_bare_metal": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)

[
    cc_test(
        name = "pico_freertos_" + mode + "_test",
        size = "small",
        timeout = "short",
        srcs = ["test/pico_freertos_test.cpp"],
        linkstatic = True,
        target_compatible_with = [
            "@roo_testing//roo_testing/platforms:rp2040",
            "@roo_testing//roo_testing/platforms:freertos",
        ],
        deps = [
            ":roo_threads",
            "@roo_testing//:arduino_gtest_main",
            "@roo_testing//roo_testing/system:timer",
        ] + (["@roo_testing//roo_testing/system:manual_time_mode"] if mode == "manual" else []),
    )
    for mode in [
        "auto",
        "manual",
    ]
]
