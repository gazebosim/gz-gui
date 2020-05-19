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

#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Application.hh"

int g_argc = 1;
char **g_argv = new char *[g_argc];

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(ExampleTest, Configs)
{
  common::Console::SetVerbosity(4);
  auto exampleConfigPath = common::joinPaths(std::string(PROJECT_SOURCE_PATH),
      "examples", "config");

  // Load each config file
  ignition::common::DirIter endIter;
  for (common::DirIter file(exampleConfigPath); file != endIter; ++file)
  {
    igndbg << *file << std::endl;

    // image.config is broken (issue #40)
    if ((*file).find("image") != std::string::npos)
    {
      ignerr << "skipping " << *file << std::endl;
      continue;
    }

    Application app(g_argc, g_argv);
    app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_TRUE(app.LoadConfig(*file));
  }
}

