/*
 * Copyright (C) 2017 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <gtest/gtest.h>
#include <QtTest/QtTest>
#include <gz/common/Console.hh>
#include <gz/math/Color.hh>
#include <gz/math/Pose3.hh>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)
#include "../helpers/TestHelper.hh"
#include "gz/gui/Application.hh"
#include "gz/gui/GuiEvents.hh"
#include "gz/gui/Plugin.hh"
#include "gz/gui/MainWindow.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./Scene3d_events_TEST")),
};

using namespace gz;
using namespace gui;

/////////////////////////////////////////////////
TEST(Scene3DTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Events))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(common::joinPaths(PROJECT_BINARY_PATH, "lib"));

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"Scene3D\">"
      "<engine>ogre</engine>"
      "<scene>banana</scene>"
      "<ambient_light>1.0 0 0</ambient_light>"
      "<background_color>0 1 0</background_color>"
      "<camera_pose>1 2 3 0 0 1.57</camera_pose>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("Scene3D",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Show, but don't exec, so we don't block
  win->QuickWindow()->show();

  // Flags to check if events were received
  bool receivedRenderEvent{false};
  bool receivedRightEvent{false};
  bool receivedLeftEvent{false};
  bool receivedRightAltEvent{false};
  bool receivedRightControlEvent{false};
  bool receivedRightShiftEvent{false};
  bool receivedLeftAltEvent{false};
  bool receivedLeftControlEvent{false};
  bool receivedLeftShiftEvent{false};
  bool receivedHoverEvent{false};
  bool receivedKeyPressEvent{false};
  bool receivedKeyPressEventAlt{false};
  bool receivedKeyPressEventControl{false};
  bool receivedKeyPressEventShift{false};
  bool receivedKeyReleaseEvent{false};
  bool receivedKeyReleaseEventAlt{false};
  bool receivedKeyReleaseEventControl{false};
  bool receivedKeyReleaseEventShift{false};

  // Position vectors reported by click events
  math::Vector3d leftClickPoint, rightClickPoint;
  // key pressed or released
  int keyPressedValue, keyReleasedValue;

  // Helper to filter events
  auto testHelper = std::make_unique<TestHelper>();
  testHelper->forwardEvent = [&](QEvent *_event)
  {
    if (_event->type() == events::Render::kType)
    {
      receivedRenderEvent = true;
    }
    else if (_event->type() == events::RightClickToScene::kType)
    {
      receivedRightEvent = true;
      auto rightClickToScene = static_cast<events::RightClickToScene*>(_event);
      rightClickPoint = rightClickToScene->Point();
    }
    else if (_event->type() == events::RightClickOnScene::kType)
    {
      auto rightClickOnScene = static_cast<events::RightClickOnScene*>(_event);
      receivedRightAltEvent = rightClickOnScene->Mouse().Alt();
      receivedRightControlEvent = rightClickOnScene->Mouse().Control();
      receivedRightShiftEvent = rightClickOnScene->Mouse().Shift();
    }
    else if (_event->type() == events::LeftClickToScene::kType)
    {
      receivedLeftEvent = true;
      auto leftClickToScene = static_cast<events::LeftClickToScene*>(_event);
      leftClickPoint = leftClickToScene->Point();
    }
    else if (_event->type() == events::LeftClickOnScene::kType)
    {
      auto leftClickOnScene = static_cast<events::LeftClickOnScene*>(_event);
      receivedLeftAltEvent = leftClickOnScene->Mouse().Alt();
      receivedLeftControlEvent = leftClickOnScene->Mouse().Control();
      receivedLeftShiftEvent = leftClickOnScene->Mouse().Shift();
    }
    else if (_event->type() == events::HoverToScene::kType)
    {
      receivedHoverEvent = true;
    }
    else if (_event->type() == events::KeyReleaseOnScene::kType)
    {
      receivedKeyReleaseEvent = true;
      auto keyReleased = static_cast<events::KeyReleaseOnScene*>(_event);
      keyReleasedValue = keyReleased->Key().Key();
      receivedKeyReleaseEventAlt = keyReleased->Key().Alt();
      receivedKeyReleaseEventControl = keyReleased->Key().Control();
      receivedKeyReleaseEventShift = keyReleased->Key().Shift();
    }
    else if (_event->type() == events::KeyPressOnScene::kType)
    {
      receivedKeyPressEvent = true;
      auto keyPress = static_cast<events::KeyPressOnScene*>(_event);
      keyPressedValue = keyPress->Key().Key();
      receivedKeyPressEventAlt = keyPress->Key().Alt();
      receivedKeyPressEventControl = keyPress->Key().Control();
      receivedKeyPressEventShift = keyPress->Key().Shift();
    }
  };

  int sleep = 0;
  int maxSleep = 30;
  while (!receivedRenderEvent && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }
  sleep = 0;
  while (!receivedHoverEvent && sleep < maxSleep)
  {
    QTest::mouseMove(win->QuickWindow(), QPoint(70, 100), -1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }
  sleep = 0;
  while (!receivedRightEvent && sleep < maxSleep)
  {
    QTest::mouseClick(win->QuickWindow(), Qt::RightButton, Qt::ShiftModifier);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }
  sleep = 0;
  while (!receivedLeftEvent && sleep < maxSleep)
  {
    QTest::mouseClick(win->QuickWindow(), Qt::LeftButton, Qt::AltModifier);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }
  sleep = 0;
  while (!receivedKeyPressEvent && sleep < maxSleep)
  {
    QTest::keyPress(win->QuickWindow(), Qt::Key_A, Qt::AltModifier);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }
  sleep = 0;
  while (!receivedKeyReleaseEvent && sleep < maxSleep)
  {
    QTest::keyRelease(win->QuickWindow(), Qt::Key_Escape, Qt::NoModifier);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  EXPECT_TRUE(receivedRenderEvent);
  EXPECT_TRUE(receivedLeftEvent);
  EXPECT_TRUE(receivedRightEvent);
  EXPECT_TRUE(receivedHoverEvent);
  EXPECT_TRUE(receivedLeftAltEvent);
  EXPECT_FALSE(receivedLeftControlEvent);
  EXPECT_FALSE(receivedLeftShiftEvent);
  EXPECT_FALSE(receivedRightAltEvent);
  EXPECT_FALSE(receivedRightControlEvent);
  EXPECT_TRUE(receivedRightShiftEvent);

  EXPECT_EQ(leftClickPoint, rightClickPoint);
  EXPECT_NEAR(1.0, leftClickPoint.X(), 1e-3);
  EXPECT_NEAR(11.942695, leftClickPoint.Y(), 1e-1);
  EXPECT_NEAR(4.159424, leftClickPoint.Z(), 0.5);

  EXPECT_TRUE(receivedKeyReleaseEvent);
  EXPECT_FALSE(receivedKeyReleaseEventAlt);
  EXPECT_FALSE(receivedKeyReleaseEventControl);
  EXPECT_FALSE(receivedKeyReleaseEventShift);
  EXPECT_EQ(Qt::Key_Escape, keyReleasedValue);
  EXPECT_TRUE(receivedKeyPressEvent);
  EXPECT_TRUE(receivedKeyPressEventAlt);
  EXPECT_FALSE(receivedKeyPressEventControl);
  EXPECT_FALSE(receivedKeyPressEventShift);
  EXPECT_EQ(Qt::Key_A, keyPressedValue);

  // Cleanups
  auto plugins = win->findChildren<Plugin *>();
  for (auto & p : plugins)
  {
    auto pluginName = p->CardItem()->objectName();
    app.RemovePlugin(pluginName.toStdString());
  }
  win->QuickWindow()->close();
}