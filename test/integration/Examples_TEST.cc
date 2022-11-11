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

#include <gz/common/Console.hh>
#include <gz/common/Filesystem.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)
#include "gz/gui/Application.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./Examples_TEST")),
};

using namespace gz;
using namespace gui;

// See https://github.com/gazebosim/gz-gui/issues/75
/////////////////////////////////////////////////
TEST(ExampleTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Configs))
{
  common::Console::SetVerbosity(4);
  auto exampleConfigPath = common::joinPaths(std::string(PROJECT_SOURCE_PATH),
      "examples", "config");

  // Load each config file
  common::DirIter endIter;
  for (common::DirIter file(exampleConfigPath); file != endIter; ++file)
  {
    gzdbg << *file << std::endl;

    Application app(g_argc, g_argv);
    app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_TRUE(app.LoadConfig(*file));
  }
}
