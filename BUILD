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

load("@bazel_rules_qt//:qt.bzl", "qt_cc_library")

qt_cc_library(
    name = "game_tile",
    src = "game_tile.cc",
    copts = [
        "-std=c++17",
        "-fPIC",
    ],
    hdr = "game_tile.h",
    deps = [
        ":game",
        "@qt//:qt_gui",
        "@qt//:qt_widgets",
    ],
)

qt_cc_library(
    name = "game_keygrabber",
    src = "game_keygrabber.cc",
    copts = [
        "-std=c++17",
        "-fPIC",
    ],
    hdr = "game_keygrabber.h",
    deps = [
        "@qt//:qt_core",
        "@qt//:qt_gui",
    ],
)

qt_cc_library(
    name = "game_window",
    src = "game_window.cc",
    copts = [
        "-std=c++17",
        "-fPIC",
    ],
    hdr = "game_window.h",
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
