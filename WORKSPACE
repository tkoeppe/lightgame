workspace(name = "tknet")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_googletest",
    strip_prefix = "googletest-master",
    urls = ["https://github.com/google/googletest/archive/master.zip"],
)

http_archive(
    name = "com_google_benchmark",
    strip_prefix = "benchmark-master",
    urls = ["https://github.com/google/benchmark/archive/master.zip"],
)

http_archive(
    name = "bazel_rules_qt",
    strip_prefix="bazel_rules_qt-master",
    urls = ["https://github.com/justbuchanan/bazel_rules_qt/archive/master.zip"],
)

new_local_repository(
    name = "qt",
    build_file = "@bazel_rules_qt//:qt.BUILD",
    path = "/usr/include/x86_64-linux-gnu/qt5/"
)
