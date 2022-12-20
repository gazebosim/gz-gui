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
#include <string>

#include <gz/msgs/pointcloud_packed.pb.h>
#include <gz/msgs/PointCloudPackedUtils.hh>
#include <gz/msgs/Utility.hh>

#include <gz/common/Console.hh>
#include <gz/common/Filesystem.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>
#include <gz/transport/Node.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)
#include "gz/gui/Application.hh"
#include "gz/gui/GuiEvents.hh"
#include "gz/gui/MainWindow.hh"
#include "gz/gui/Plugin.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./PointCloud_TEST")),
};

using namespace std::chrono_literals;

using namespace gz;
using namespace gui;

class PointCloudTestFixture : public ::testing::Test
{
  public:
    transport::Node node;
    rendering::ScenePtr scene;
    transport::Node::Publisher pointcloudPub;
    transport::Node::Publisher flatPub;
    msgs::PointCloudPacked pcMsg;
    msgs::Float_V flatMsg;
    std::atomic<bool> receivedMsg;

    PointCloudTestFixture()
    {
      this->pointcloudPub = this->node.Advertise<msgs::PointCloudPacked>(
        "/point_cloud");
      this->flatPub = this->node.Advertise<msgs::Float_V>("/flat");

      this->InitMockData();

      this->node.Advertise("/marker",
        &PointCloudTestFixture::OnMarkerMsg, this);
      this->receivedMsg = false;
    }

    public: void InitMockData()
    {
      msgs::InitPointCloudPacked(this->pcMsg, "some_frame", true,
          {{"xyz", msgs::PointCloudPacked::Field::FLOAT32}});

      int numberOfPoints{1000};
      unsigned int dataSize{numberOfPoints * this->pcMsg.point_step()};
      this->pcMsg.mutable_data()->resize(dataSize);
      this->pcMsg.set_height(1);
      this->pcMsg.set_width(1000);

      // Populate messages
      msgs::PointCloudPackedIterator<float> xIter(this->pcMsg, "x");
      msgs::PointCloudPackedIterator<float> yIter(this->pcMsg, "y");
      msgs::PointCloudPackedIterator<float> zIter(this->pcMsg, "z");

      for (float x = 0.0, y = 0.0, z = 0.0;
          xIter != xIter.End();
          ++xIter, ++yIter, ++zIter)
      {
        *xIter = x;
        *yIter = y;
        *zIter = z;
        flatMsg.add_data(x);

        x += 1.0;
        if (x > 9)
        {
          x = 0.0;
          y += 1.0;
        }
        if (y > 9)
        {
          y = 0.0;
          z += 1.0;
        }
      }
    }

    public: void Publish()
    {
      this->pointcloudPub.Publish(this->pcMsg);
      this->flatPub.Publish(this->flatMsg);
    }

    /// \brief Callback that receives marker messages.
    /// \param[in] _req The marker message.
    public: void OnMarkerMsg(const msgs::Marker &_req)
    {
      if (_req.action() == msgs::Marker::ADD_MODIFY)
      {
        EXPECT_EQ(_req.id(), 1);
        EXPECT_EQ(_req.ns(), "/point_cloud/flat");
        EXPECT_EQ(_req.type(), msgs::Marker::POINTS);
        EXPECT_EQ(_req.visibility(), msgs::Marker::GUI);
        if (_req.point().size() != 0)
        {
          // We might receive empty packets as the sending process
          // is asynchronuous
          EXPECT_EQ(_req.point().size(), this->flatMsg.data().size());
          EXPECT_EQ(_req.materials().size(), this->flatMsg.data().size());
          this->receivedMsg = true;
        }
      }
      else if(_req.action() == gz::msgs::Marker::DELETE_ALL)
      {
        // Do nothing. Its ok to clear the screen.
      }
      else
      {
        ADD_FAILURE();
      }
    }
};

/////////////////////////////////////////////////
TEST_F(PointCloudTestFixture,
  GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(PointCloudTestFixture))
{
  common::Console::SetVerbosity(4);

  // Load the plugin
  Application app(g_argc, g_argv);
  app.AddPluginPath(
    common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));
  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"PointCloud\" name=\"Point Cloud\">"
      "<point_cloud_topic>/point_cloud</point_cloud_topic>"
      "<float_v_topic>/flat</float_v_topic>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  EXPECT_EQ(tinyxml2::XML_SUCCESS, pluginDoc.Parse(pluginStr));

  EXPECT_TRUE(app.LoadPlugin("PointCloud",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto window = app.findChild<MainWindow *>();
  ASSERT_NE(window, nullptr);

  // Get plugin
  auto plugins = window->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);

  // Show, but don't exec, so we don't block
  window->QuickWindow()->show();

  this->Publish();

  int sleep = 0;
  int maxSleep = 30;
  while (sleep < maxSleep && !this->receivedMsg)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    this->Publish();
    sleep++;
  }

  EXPECT_TRUE(this->receivedMsg);

  // Cleanup
  plugins.clear();
}
