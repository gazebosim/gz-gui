/*
 * Copyright (C) 2023 Open Source Robotics Foundation
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
#ifndef GZ_GUI_RENDERENGINEHELPER_HH_
#define GZ_GUI_RENDERENGINEHELPER_HH_

#include <gtest/gtest.h>

#include <memory>
#include <string>

#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/RenderTypes.hh>

#include "gz/gui/GuiEvents.hh"

#include "TestHelper.hh"

namespace gz
{
namespace gui
{
namespace testing
{
  /// \brief Get the render engine
  /// This function should be called after the main window is
  /// shown (mainWindow.show()). It blocks until render events
  /// are received.
  /// \return A pointer to the render engine or nullptr if it's not available.
  static rendering::RenderEngine* getRenderEngine(
      const std::string &_engine)
  {
    // Filter events
    bool receivedPreRenderEvent{false};
    bool receivedRenderEvent{false};
    auto testHelper = std::make_unique<TestHelper>();
    testHelper->forwardEvent = [&](QEvent *_event)
    {
      if (_event->type() == gui::events::PreRender::kType)
      {
        receivedPreRenderEvent = true;
      }
      if (_event->type() == gui::events::Render::kType)
      {
        receivedRenderEvent = true;
      }
    };

    int sleep = 0;
    int maxSleep = 100;
    while (!receivedRenderEvent && sleep < maxSleep)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      QCoreApplication::processEvents();
      ++sleep;
    }
    EXPECT_TRUE(receivedPreRenderEvent);
    EXPECT_TRUE(receivedRenderEvent);

    // Check scene
    auto engine = rendering::engine(_engine);
    return engine;
  }
}
}
}

#endif
