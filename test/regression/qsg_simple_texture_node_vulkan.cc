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

// Qt Vulkan capability probe: verifies the Qt build has Vulkan support
// and that QVulkanInstance can be created at runtime.
// GTEST_SKIPs cleanly when either condition is not met.

#include <gtest/gtest.h>
#include <gz/utils/ExtraTestMacros.hh>

#include <QtGlobal>
#include <qtgui-config.h>

#if QT_CONFIG(vulkan)

#include <QGuiApplication>
#include <QSGRendererInterface>
#include <QVulkanInstance>
#include <QQuickWindow>

// Verify Qt was built with Vulkan support and a VulkanInstance can be
// created, then confirm QQuickWindow accepts the Vulkan graphics API.
TEST(QsgSimpleTextureNodeVulkan,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(VulkanCapabilityCheck))
{
  int argc = 0;
  QGuiApplication app(argc, nullptr);

  QVulkanInstance inst;
  if (!inst.create())
    GTEST_SKIP() << "QVulkanInstance::create failed — no Vulkan driver";

  QQuickWindow::setGraphicsApi(QSGRendererInterface::VulkanRhi);
  EXPECT_EQ(QSGRendererInterface::VulkanRhi, QQuickWindow::graphicsApi());
}

#else

TEST(QsgSimpleTextureNodeVulkan, VulkanNotAvailable)
{
  GTEST_SKIP() << "Qt built without Vulkan support (QT_FEATURE_vulkan=OFF)";
}

#endif  // QT_CONFIG(vulkan)
