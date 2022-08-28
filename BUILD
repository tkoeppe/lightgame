load("@com_justbuchanan_rules_qt//:qt.bzl", "qt_cc_library")

cc_library(
    name = "game",
    srcs = ["game.cc"],
    hdrs = ["game.h"],
    copts = ["-std=c++17"],
)

cc_binary(
    name = "game_cli",
    srcs = ["game_cli.cc"],
    copts = ["-std=c++17"],
    deps = [":game"],
)

cc_test(
    name = "game_test",
    srcs = ["game_test.cc"],
    deps = [
        ":game",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_binary(
    name = "game_benchmark",
    srcs = ["game_benchmark.cc"],
    deps = [
        ":game",
        "@com_google_benchmark//:benchmark_main",
    ],
)

qt_cc_library(
    name = "game_tile",
    srcs = ["game_tile.cc"],
    hdrs = ["game_tile.h"],
    copts = [
        "-std=c++17",
        "-fPIC",
    ],
    deps = [
        ":game",
        "@qt//:qt_gui",
        "@qt//:qt_widgets",
    ],
)

qt_cc_library(
    name = "game_keygrabber",
    srcs = ["game_keygrabber.cc"],
    hdrs = ["game_keygrabber.h"],
    copts = [
        "-std=c++17",
        "-fPIC",
    ],
    deps = [
        "@qt//:qt_core",
        "@qt//:qt_gui",
    ],
)

qt_cc_library(
    name = "game_window",
    srcs = ["game_window.cc"],
    hdrs = ["game_window.h"],
    copts = [
        "-std=c++17",
        "-fPIC",
    ],
    deps = [
        ":game",
        ":game_keygrabber",
        ":game_tile",
        "@qt//:qt_widgets",
    ],
)

cc_binary(
    name = "game_qt",
    srcs = ["game_qt.cc"],
    copts = [
        "-std=c++17",
        "-fPIC",
    ],
    deps = [
        ":game_window",
        "@qt//:qt_widgets",
    ],
)
