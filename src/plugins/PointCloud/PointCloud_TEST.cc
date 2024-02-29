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

#include <gz/msgs/float_v.pb.h>
#include <gz/msgs/marker.pb.h>
#include <gz/msgs/pointcloud_packed.pb.h>
#include <gz/msgs/PointCloudPackedUtils.hh>
#include <gz/msgs/Utility.hh>

#include <gz/common/Console.hh>
#include <gz/common/Filesystem.hh>
#include <gz/transport/Node.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)
#include "gz/gui/Application.hh"
#include "gz/gui/MainWindow.hh"
#include "gz/gui/Plugin.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./PointCloud_TEST")),
};

using namespace gz;
using namespace gui;

class PointCloudTestFixture : public ::testing::Test
{
  public:
    transport::Node node;
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

    /// \brief Publish pointcloud packed data
    public: void Publish()
    {
      this->pointcloudPub.Publish(this->pcMsg);
      this->flatPub.Publish(this->flatMsg);
    }

    /// \brief Color for minimum value
    public: const math::Color minColor{1.0f, 0.0f, 0.0f, 1.0f};

    /// \brief Color for maximum value
    public: const math::Color maxColor{0.0f, 1.0f, 0.0f, 1.0f};

    public: bool indexRecieved[10][10][10] = {false};

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

          auto dR = maxColor.R() - minColor.R();
          auto dG = maxColor.G() - minColor.G();
          auto dB = maxColor.B() - minColor.B();
          auto dA = maxColor.A() - minColor.A();

          for (int idx = 0; idx < _req.point().size(); idx++)
          {
            // Check color correctness
            EXPECT_NEAR(dR * (_req.point()[idx].x() / 9) + minColor.R(),
             _req.materials()[idx].diffuse().r(), 1e-3);
            EXPECT_NEAR(dG * (_req.point()[idx].x() / 9) + minColor.G(),
             _req.materials()[idx].diffuse().g(), 1e-3);
            EXPECT_NEAR(dB * (_req.point()[idx].x() / 9) + minColor.B(),
             _req.materials()[idx].diffuse().b(), 1e-3);
            EXPECT_NEAR(dA * (_req.point()[idx].x() / 9) + minColor.A(),
             _req.materials()[idx].diffuse().a(), 1e-3);

            std::size_t x = round(_req.point()[idx].x());
            std::size_t y = round(_req.point()[idx].y());
            std::size_t z = round(_req.point()[idx].z());

            // Mark this voxel as occupied
            this->indexRecieved[x][y][z] = true;
          }

          // Check all points in the point cloud have been populated.
          for (std::size_t i = 0; i < 10; i++)
          {
            for (std::size_t j = 0; j < 10; j++)
            {
              for (std::size_t k = 0; k < 10; k++)
              {
                EXPECT_TRUE(this->indexRecieved[i][j][k]);
              }
            }
          }
          this->receivedMsg = true;
        }
      }
      else if (_req.action() != msgs::Marker::DELETE_ALL)
      {
        // if DELETE_ALL, its ok to clear the screen. Otherwise fail
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
  while (!this->receivedMsg && sleep < maxSleep)
  {
    this->Publish();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    ++sleep;
  }
  EXPECT_TRUE(this->receivedMsg);

  // Cleanup
  plugins.clear();
}
