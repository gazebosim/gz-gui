/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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
TEST(DialogTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(UpdateDialogConfig))
{
  ignition::common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv, ignition::gui::WindowType::kDialog);

  // Change default config path
  App()->SetDefaultConfigPath(kTestConfigFile);

  auto dialog = new Dialog;
  ASSERT_NE(nullptr, dialog);

  // Read attribute value when the default the config is not set
  {
    std::string allow = dialog->ReadConfigAttribute(app.DefaultConfigPath(),
      "allow");
    EXPECT_EQ(allow, "");
  }

  // Read a non existing attribute
  {
    dialog->setObjectName("quick_menu");
    dialog->SetDefaultConfig(std::string(
      "<dialog name=\"quick_menu\" show=\"true\"/>"));
    std::string allow = dialog->ReadConfigAttribute(app.DefaultConfigPath(),
      "allow");
    EXPECT_EQ(allow, "");
  }

  // Read an existing attribute
  {
    std::string show = dialog->ReadConfigAttribute(app.DefaultConfigPath(),
      "show");
    EXPECT_EQ(show, "true");
  }

  // Update a non existing attribute
  {
    dialog->UpdateConfigAttribute(app.DefaultConfigPath(), "allow", true);
    std::string allow = dialog->ReadConfigAttribute(app.DefaultConfigPath(),
      "allow");
    EXPECT_EQ(allow, "true");
  }

  // Update a existing attribute
  {
    dialog->UpdateConfigAttribute(app.DefaultConfigPath(), "allow", false);
    std::string allow = dialog->ReadConfigAttribute(app.DefaultConfigPath(),
      "allow");
    EXPECT_EQ(allow, "false");
  }

  delete dialog;
}
