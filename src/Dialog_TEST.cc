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

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./Dialog_TEST")),
};

using namespace ignition;
using namespace gui;

class DialogTest : public ::testing::Test
{
  // Documentation inherited
  protected: void SetUp() override
  {
    common::Console::SetVerbosity(4);

    // Remove the config file at the beginning of every test run
    common::removeAll(this->kTestConfigFile);
  }

  /// \brief Directory to act as $HOME for tests
  public: const std::string kTestConfigFile =
      common::joinPaths(PROJECT_BINARY_PATH, "test", "ign-gui-test.config");
};

/////////////////////////////////////////////////
TEST_F(DialogTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(ReadDialogConfig))
{
  Application app(g_argc, g_argv, WindowType::kDialog);

  // Change default config path
  app.SetDefaultConfigPath(this->kTestConfigFile);

  auto dialog = new Dialog();
  ASSERT_NE(nullptr, dialog);

  // Read dialog config
  {
    dialog->SetName("dialog");
    dialog->SetDefaultConfig(std::string(
      "<dialog name=\"dialog\" default=\"true\"/>"));
    std::string defaultValue = dialog->ReadAttribute(app.DefaultConfigPath(),
      "default");
    EXPECT_EQ(defaultValue, "true");
  }

  delete dialog;
}

/////////////////////////////////////////////////
TEST_F(DialogTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(ChangeDialogConfig))
{
  Application app(g_argc, g_argv, WindowType::kDialog);

  // Change default config path
  App()->SetDefaultConfigPath(this->kTestConfigFile);

  auto dialog = new Dialog();
  ASSERT_NE(nullptr, dialog);

  // Read dialog config
  {
    dialog->SetName("dialog");
    dialog->SetDefaultConfig(std::string(
      "<dialog name=\"dialog\" default=\"true\"/>"));
    std::string defaultValue = dialog->ReadAttribute(app.DefaultConfigPath(),
      "default");

    EXPECT_EQ(defaultValue, "true");
    dialog->WriteAttribute(app.DefaultConfigPath(), "default", false);
    defaultValue = dialog->ReadAttribute(app.DefaultConfigPath(), "default");
    EXPECT_NE(defaultValue, "true");
  }

  delete dialog;
}
