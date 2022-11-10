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

#include <gz/common/Console.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)
#include "gz/gui/Application.hh"
#include "gz/gui/Dialog.hh"

std::string kTestConfigFile = "/tmp/gz-gui-test.config"; // NOLINT(*)
int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./Dialog_TEST")),
};

using namespace gz;
using namespace gui;

/////////////////////////////////////////////////
TEST(DialogTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(Accessors))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv, WindowType::kDialog);

  auto dialog = new Dialog;
  ASSERT_NE(nullptr, dialog);

  EXPECT_NE(nullptr, dialog->RootItem());
  EXPECT_NE(nullptr, dialog->QuickWindow());
}

/////////////////////////////////////////////////
TEST(DialogTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(UpdateDialogConfig))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv, WindowType::kDialog);

  auto dialog = new Dialog;
  ASSERT_NE(nullptr, dialog);
  dialog->setObjectName("quick_menu");

  // Call deprecated function for test coverage
  dialog->SetDefaultConfig("");

  // Start without a file
  std::remove(kTestConfigFile.c_str());

  // The file doesn't exist
  {
    EXPECT_FALSE(common::exists(kTestConfigFile));
    std::string allow = dialog->ReadConfigAttribute(kTestConfigFile,
      "allow");
    EXPECT_TRUE(allow.empty());

    // Config file still doesn't exist
    EXPECT_FALSE(common::exists(kTestConfigFile));

    EXPECT_FALSE(dialog->UpdateConfigAttribute(kTestConfigFile, "allow", true));

    // Config file still doesn't exist
    EXPECT_FALSE(common::exists(kTestConfigFile));
  }

  // Malformed file
  {
    EXPECT_FALSE(common::exists(kTestConfigFile));

    // Create file
    std::ofstream configFile(kTestConfigFile);
    configFile << "banana";
    configFile.close();
    EXPECT_TRUE(common::exists(kTestConfigFile));

    std::string allow = dialog->ReadConfigAttribute(kTestConfigFile,
      "allow");
    EXPECT_TRUE(allow.empty());

    EXPECT_FALSE(dialog->UpdateConfigAttribute(kTestConfigFile, "allow", true));

    // Delete file
    std::remove(kTestConfigFile.c_str());
  }

  // Read a non existing attribute
  {
    EXPECT_FALSE(common::exists(kTestConfigFile));

    // Create file
    std::ofstream configFile(kTestConfigFile);
    configFile << "<dialog name='quick_menu'/>";
    configFile.close();
    EXPECT_TRUE(common::exists(kTestConfigFile));

    std::string allow = dialog->ReadConfigAttribute(kTestConfigFile,
      "allow");
    EXPECT_TRUE(allow.empty());

    // Delete file
    std::remove(kTestConfigFile.c_str());
  }

  // Read an existing attribute
  {
    EXPECT_FALSE(common::exists(kTestConfigFile));

    // Create file
    std::ofstream configFile(kTestConfigFile);
    configFile << "<dialog name='quick_menu' show='true'/>";
    configFile.close();
    EXPECT_TRUE(common::exists(kTestConfigFile));

    std::string show = dialog->ReadConfigAttribute(kTestConfigFile,
      "show");
    EXPECT_EQ(show, "true");

    // Delete file
    std::remove(kTestConfigFile.c_str());
  }

  // Update a non existing attribute
  {
    EXPECT_FALSE(common::exists(kTestConfigFile));

    // Create file
    std::ofstream configFile(kTestConfigFile);
    configFile << "<dialog name='quick_menu'/>";
    configFile.close();
    EXPECT_TRUE(common::exists(kTestConfigFile));

    // Update value
    EXPECT_TRUE(dialog->UpdateConfigAttribute(kTestConfigFile, "allow", true));

    // Read value
    auto allow = dialog->ReadConfigAttribute(kTestConfigFile, "allow");
    EXPECT_EQ(allow, "true");

    // Delete file
    std::remove(kTestConfigFile.c_str());
  }

  // Update an existing attribute
  {
    EXPECT_FALSE(common::exists(kTestConfigFile));

    // Create file
    std::ofstream configFile(kTestConfigFile);
    configFile << "<dialog name='quick_menu' allow='true'/>";
    configFile.close();
    EXPECT_TRUE(common::exists(kTestConfigFile));

    // Update value
    EXPECT_TRUE(dialog->UpdateConfigAttribute(kTestConfigFile, "allow", false));

    // Read value
    auto allow = dialog->ReadConfigAttribute(kTestConfigFile, "allow");
    EXPECT_EQ(allow, "false");

    // Delete file
    std::remove(kTestConfigFile.c_str());
  }

  // Update a file with a different <dialog>
  {
    EXPECT_FALSE(common::exists(kTestConfigFile));

    // Create file
    std::ofstream configFile(kTestConfigFile);
    configFile << "<dialog name='banana' allow='false'/>";
    configFile.close();
    EXPECT_TRUE(common::exists(kTestConfigFile));

    // Update value
    EXPECT_TRUE(dialog->UpdateConfigAttribute(kTestConfigFile, "allow", true));

    // Read value
    auto allow = dialog->ReadConfigAttribute(kTestConfigFile, "allow");
    EXPECT_EQ(allow, "true");

    // Delete file
    std::remove(kTestConfigFile.c_str());
  }

  // Update a file without a <dialog>
  {
    EXPECT_FALSE(common::exists(kTestConfigFile));

    // Create file
    std::ofstream configFile(kTestConfigFile);
    configFile << "<banana/>";
    configFile.close();
    EXPECT_TRUE(common::exists(kTestConfigFile));

    // Update value
    EXPECT_TRUE(dialog->UpdateConfigAttribute(kTestConfigFile, "allow", true));

    // Read value
    auto allow = dialog->ReadConfigAttribute(kTestConfigFile, "allow");
    EXPECT_EQ(allow, "true");

    // Delete file
    std::remove(kTestConfigFile.c_str());
  }

  delete dialog;
}
