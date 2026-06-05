gz_get_sources(tests)

# qsg_simple_texture_node_vulkan exercises the QSGSimpleTextureNode +
# QSGVulkanTexture::fromNative path (see the test's header comment for
# context). It calls Vulkan
# APIs directly, so it needs the Vulkan loader library and the Qt Quick module
# for QQuickView. The test gracefully GTEST_SKIPs when Qt was built without
# Vulkan support, so the dependency is optional.
find_package(Vulkan QUIET)
set(_qsg_vk_libs "")
if (Vulkan_FOUND)
  list(APPEND _qsg_vk_libs Vulkan::Vulkan)
endif()

gz_build_tests(TYPE REGRESSION
               SOURCES ${tests}
               LIB_DEPS
                 Qt${QT_MAJOR_VERSION}::Quick
                 Qt${QT_MAJOR_VERSION}::Test
                 ${_qsg_vk_libs}
               ENVIRONMENT GZ_GUI_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})
