workspace(name = "tknet")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_googletest",
    strip_prefix = "googletest-main",
    urls = ["https://github.com/google/googletest/archive/main.zip"],
)

http_archive(
    name = "com_google_benchmark",
    strip_prefix = "benchmark-main",
    urls = ["https://github.com/google/benchmark/archive/main.zip"],
)

http_archive(
    name = "com_justbuchanan_rules_qt",
    strip_prefix="bazel_rules_qt-master",
    urls = ["https://github.com/justbuchanan/bazel_rules_qt/archive/master.zip"],
)

load("@com_justbuchanan_rules_qt//:qt_configure.bzl", "qt_configure")
qt_configure()

load("@local_config_qt//:local_qt.bzl", "local_qt_path")
new_local_repository(
    name = "qt",
    build_file = "@com_justbuchanan_rules_qt//:qt.BUILD",
    path = local_qt_path(),
)

load("@com_justbuchanan_rules_qt//tools:qt_toolchain.bzl", "register_qt_toolchains")
register_qt_toolchains()
