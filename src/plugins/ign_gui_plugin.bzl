load(
    "//ign_bazel:build_defs.bzl",
    "qt_cc_library",
    "qt_resource",
)

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary", "cc_test")

def ign_qt_plugin(
    folder_name, plugin_name, 
    qrc_file, qrc_resources, 
    srcs, hdrs, normal_headers=[], deps = [], test_srcs = [], test_deps= []):

    qt_resource(
        name = folder_name + "_resources",
        qrc_file = qrc_file,
        files = qrc_resources,
    )

    qt_cc_library(
        name = folder_name,
        srcs = srcs,
        hdrs = hdrs,
        normal_headers = normal_headers,
        deps = deps,
    )

    cc_binary(
        name = "lib%s.so" % plugin_name,
        srcs = ":%s" % folder_name,
        linkshared=True
    )

