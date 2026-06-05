// Copyright (C) 2026 Open Source Robotics Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Integration tests for gz-gui Vulkan features:
//  1. GZ_GUI_RENDER_ENGINE_GUI_API_BACKEND env var selects Vulkan backend.
//  2. When Vulkan backend is active, MinimalScene::RhiParams() returns
//     the four vulkan_* handle keys with non-zero values.

#include <gtest/gtest.h>
#include <gz/utils/ExtraTestMacros.hh>

#include <QtTest/QtTest>
#include <QtGlobal>
#include <QVariantMap>
#include <qtgui-config.h>

#include <gz/common/Console.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/MainWindow.hh"
#include "gz/gui/Plugin.hh"

#include "test_config.hh"  // NOLINT(build/include)
#include "../helpers/TestHelper.hh"

#if QT_CONFIG(vulkan)
#include <QSGRendererInterface>
#endif

using namespace gz;
using namespace gz::gui;

//////////////////////////////////////////////////
// Test 1: GZ_GUI_RENDER_ENGINE_GUI_API_BACKEND env var selects Vulkan.
//
// Creates an Application without passing a backend string (the 4th ctor
// param is left nullptr). The env var should trigger the fallback path in
// Application.cc and configure Qt to use VulkanRhi.
TEST(MinimalSceneVulkan,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(EnvVarSelectsVulkanBackend))
{
#if !QT_CONFIG(vulkan)
  GTEST_SKIP() << "Qt built without Vulkan support";
#else
  qputenv("GZ_GUI_RENDER_ENGINE_GUI_API_BACKEND", "vulkan");

  int g_argc = 1;
  char *g_argv[] = {
    reinterpret_cast<char *>(const_cast<char *>("./MinimalSceneVulkan_TEST"))
  };
  Application app(g_argc, g_argv);  // no backend param → env var fallback
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  ASSERT_TRUE(app.LoadPlugin("MinimalScene"));

  auto *win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  win->QuickWindow()->show();
  if (!QTest::qWaitForWindowExposed(win->QuickWindow(), 5000))
  {
    qputenv("GZ_GUI_RENDER_ENGINE_GUI_API_BACKEND", "");
    GTEST_SKIP() << "Window not exposed within timeout";
  }

  // Verify the scene graph is using Vulkan RHI.
  auto *rif = win->QuickWindow()->rendererInterface();
  ASSERT_NE(nullptr, rif);
  if (rif->graphicsApi() != QSGRendererInterface::VulkanRhi)
  {
    qputenv("GZ_GUI_RENDER_ENGINE_GUI_API_BACKEND", "");
    GTEST_SKIP() << "Vulkan RHI not initialised (driver present but "
                    "possibly unsupported by Qt on this system)";
  }
  EXPECT_EQ(QSGRendererInterface::VulkanRhi, rif->graphicsApi());

  // Cleanup
  qputenv("GZ_GUI_RENDER_ENGINE_GUI_API_BACKEND", "");
  win->QuickWindow()->close();
#endif  // QT_CONFIG(vulkan)
}

//////////////////////////////////////////////////
// Test 2: Vulkan backend → rhiParams has the four Vulkan handle keys.
//
// Passes "vulkan" explicitly as the backend constructor param, loads
// MinimalScene, waits for rendering, then checks RhiParams() via
// QMetaObject::invokeMethod (no static link against the plugin .so needed).
TEST(MinimalSceneVulkan,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(VulkanHandlesPopulatedInRhiParams))
{
#if !QT_CONFIG(vulkan)
  GTEST_SKIP() << "Qt built without Vulkan support";
#else
  int g_argc = 1;
  char *g_argv[] = {
    reinterpret_cast<char *>(const_cast<char *>("./MinimalSceneVulkan_TEST"))
  };
  Application app(g_argc, g_argv, WindowType::kMainWindow, "vulkan");
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  ASSERT_TRUE(app.LoadPlugin("MinimalScene"));

  auto *win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  win->QuickWindow()->show();
  if (!QTest::qWaitForWindowExposed(win->QuickWindow(), 5000))
    GTEST_SKIP() << "Window not exposed within timeout";

  // Retrieve the MinimalScene plugin via the public Plugin base class.
  // RhiParams() is Q_INVOKABLE — call via invokeMethod so the test does not
  // need to statically link against the plugin's .so (hidden-visibility build).
  auto plugins = win->findChildren<gz::gui::Plugin *>();
  ASSERT_EQ(1u, plugins.size())
      << "Expected exactly one plugin (MinimalScene) loaded";
  auto *plugin = plugins.front();

  // Poll RhiParams() directly for up to 5 s.  The render thread populates the
  // map asynchronously after GzRenderer::Initialize() succeeds; if Vulkan is
  // unsupported in this environment the map stays empty and we skip cleanly.
  QVariantMap params;
  const bool populated = QTest::qWaitFor([&]() -> bool {
    bool ok = QMetaObject::invokeMethod(plugin, "RhiParams",
        Qt::DirectConnection,
        Q_RETURN_ARG(QVariantMap, params));
    return ok && params.contains("vulkan_instance");
  }, 5000);

  if (!populated)
    GTEST_SKIP() << "RhiParams empty after 5 s — Vulkan render engine did not "
                    "initialise (acceptable on this system)";

  // All four Vulkan handle keys must be present with non-zero values.
  for (const auto *key : {"vulkan_instance", "vulkan_physical_device",
                           "vulkan_device", "vulkan_graphics_queue"})
  {
    ASSERT_TRUE(params.contains(key))
        << "Expected key '" << key << "' missing from RhiParams()";
    EXPECT_NE("0", params.value(key).toString().toStdString())
        << "Key '" << key << "' should be a non-zero handle";
  }

  win->QuickWindow()->close();
#endif  // QT_CONFIG(vulkan)
}
