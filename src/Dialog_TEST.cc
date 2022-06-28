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

#include <QQmlProperty>

#include <ignition/common/Console.hh>
#include <ignition/utilities/ExtraTestMacros.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Application.hh"
#include "ignition/gui/Dialog.hh"

std::string kTestConfigFile = "/tmp/ign-gui-test.config"; // NOLINT(*)
int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./Dialog_TEST")),
};

using namespace ignition;
using namespace gui;
using namespace std::chrono_literals;

/////////////////////////////////////////////////
TEST(DialogTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(ReadDialogConfig))
{
  ignition::common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv, ignition::gui::WindowType::kDialog);

  // Change default config path
  App()->SetDefaultConfigPath(kTestConfigFile);

  auto dialog = new Dialog;
  ASSERT_NE(nullptr, dialog);

  // Read dialog config
  {
    dialog->SetName("dialog");
    dialog->SetDefaultConfig(std::string(
      "<dialog name=\"dialog\" default=\"true\"/>"));
    std::string defaultValue = dialog->ReadAttribute(app->DefaultConfigPath(),
      "default");
    EXPECT_TRUE(defaultValue, "true");
  }

  delete dialog;
}

/////////////////////////////////////////////////
TEST(DialogTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(ChangeDialogConfig))
{
  ignition::common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv, ignition::gui::WindowType::kDialog);

  // Change default config path
  App()->SetDefaultConfigPath(kTestConfigFile);

  auto dialog = new Dialog;
  ASSERT_NE(nullptr, dialog);

  // Read dialog config
  {
    dialog->SetName("dialog");
    dialog->SetDefaultConfig(std::string(
      "<dialog name=\"dialog\" default=\"true\"/>"));
    std::string defaultValue = dialog->ReadAttribute(app->DefaultConfigPath(),
      "default");

    EXPECT_TRUE(defaultValue, "true");
    dialog->WriteAttribute(app->DefaultConfigPath(), "default",
      false);
    defaultValue = dialog->ReadAttribute(app->DefaultConfigPath(),
      "default");
    EXPECT_FALSE(defaultValue, "true");
  }

  delete dialog;
}
