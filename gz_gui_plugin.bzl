load("@com_justbuchanan_rules_qt//:qt.bzl", "gencpp", "genqrc", "qt_cc_library", "qt_ui_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

def gz_gui_plugin(
        name,
        srcs,
        hdrs,
        qrc_file,
        qrc_resources,
        normal_headers = [],
        deps = [],
        test_srcs = [],
        test_deps = []):
    gencpp(
        name = name + "_resources",
        resource_name = name,
        qrc = qrc_file,
        files = qrc_resources,
        cpp = name + "_resources.cpp",
    )

    qt_cc_library(
        name = name,
        srcs = srcs + [name + "_resources.cpp"],
        hdrs = hdrs,
        normal_hdrs = normal_headers,
        deps = deps,
        alwayslink = 1,
        defines = [
            "GZ_PLUGIN_STATIC_ONLY=1",
        ],
    )

    if len(test_srcs):
        cc_test(
            name = "%s_TEST" % name,
            srcs = test_srcs,
            deps = deps + test_deps + [
                "@gtest",
                "@gtest//:gtest_main",
                ":" + name,
            ],
        )
