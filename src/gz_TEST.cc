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
#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <gz/common/Filesystem.hh>
#include <gz/common/Util.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)

#ifdef _MSC_VER
#    define popen _popen
#    define pclose _pclose
#endif

/////////////////////////////////////////////////
std::string custom_exec_str(std::string _cmd)
{
  _cmd += " 2>&1";
  FILE *pipe = popen(_cmd.c_str(), "r");

  if (!pipe)
    return "ERROR";

  char buffer[128];
  std::string result = "";

  while (!feof(pipe))
  {
    if (fgets(buffer, 128, pipe) != NULL)
      result += buffer;
  }

  pclose(pipe);
  return result;
}

using namespace gz;

class CmdLine : public ::testing::Test
{
  // Documentation inherited
  protected: void SetUp() override
  {
    // Change environment variable so that test files aren't written to $HOME
    common::env(GZ_HOMEDIR, this->realHome);
    EXPECT_TRUE(common::setenv(GZ_HOMEDIR, this->kFakeHome.c_str()));
  }

  // Documentation inherited
  protected: void TearDown() override
  {
    // Restore $HOME
    EXPECT_TRUE(common::setenv(GZ_HOMEDIR, this->realHome.c_str()));
  }

  /// \brief Directory to act as $HOME for tests
  public: const std::string kFakeHome = common::joinPaths(PROJECT_BINARY_PATH,
      "test", "fake_home");

  /// \brief Store user's real $HOME to set it back at the end of tests.
  public: std::string realHome;
};

// See https://github.com/gazebosim/gz-gui/issues/75
TEST_F(CmdLine, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(list))
{
  // Clear home if it exists
  common::removeAll(this->kFakeHome);

  EXPECT_FALSE(common::exists(this->kFakeHome));

  std::string output = custom_exec_str("gz gui -l");
  EXPECT_NE(output.find("TopicEcho"), std::string::npos) << output;
  EXPECT_NE(output.find("Publisher"), std::string::npos) << output;

  EXPECT_TRUE(common::exists(common::joinPaths(this->kFakeHome, ".ignition",
      "gui")));
}
