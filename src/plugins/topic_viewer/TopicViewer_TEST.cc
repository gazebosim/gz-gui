/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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

#include <gz/msgs/collision.pb.h>
#include <gz/msgs/int32.pb.h>

#include <gz/common/Console.hh>
#include <gz/transport/Node.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)
#include "gz/gui/Application.hh"
#include "gz/gui/Plugin.hh"
#include "gz/gui/MainWindow.hh"
#include "TopicViewer.hh"

#define NAME_ROLE 51
#define TYPE_ROLE 52
#define TOPIC_ROLE 53
#define PATH_ROLE 54
#define PLOT_ROLE 55


int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./TopicViewer_TEST")),
};

using namespace gz;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
// See https://github.com/gazebosim/gz-gui/issues/75
TEST(TopicViewerTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Load))
{
    common::Console::SetVerbosity(4);

    Application app(g_argc, g_argv);
    app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_TRUE(app.LoadPlugin("TopicViewer"));

    // Get main window
    auto win = app.findChild<MainWindow *>();
    ASSERT_NE(nullptr, win);

    // Get plugin
    auto plugins = win->findChildren<Plugin *>();
    EXPECT_EQ(plugins.size(), 1);

    auto plugin = plugins[0];
    EXPECT_EQ(plugin->Title(), "Topic Viewer");

    // Cleanup
    plugins.clear();
}

/////////////////////////////////////////////////
// See https://github.com/gazebosim/gz-gui/issues/75
TEST(TopicViewerTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Model))
{
    setenv("GZ_PARTITION", "gz-sim-test", 1);

    // =========== Publish =================
    transport::Node node;

    // int
    auto pubInt = node.Advertise<msgs::Int32>("/int_topic");
    msgs::Int32 msgInt;

    // collision
    auto pub = node.Advertise<msgs::Collision> ("/collision_topic");
    msgs::Collision msg;

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    pub.Publish(msg);
    pubInt.Publish(msgInt);

    // ========== Load the Plugin ============
    common::Console::SetVerbosity(4);

    Application app(g_argc, g_argv);
    app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    // Load plugin
    const char *pluginStr =
      "<plugin filename=\"TopicViewer\">"
        "<gz-gui>"
          "<title>Topic Viewer</title>"
        "</gz-gui>"
      "</plugin>";

    tinyxml2::XMLDocument pluginDoc;
    EXPECT_EQ(tinyxml2::XML_SUCCESS, pluginDoc.Parse(pluginStr));

    EXPECT_TRUE(app.LoadPlugin("TopicViewer",
        pluginDoc.FirstChildElement("plugin")));

    // Get main window
    auto win = app.findChild<MainWindow *>();
    EXPECT_NE(nullptr, win);

    // Get plugin
    auto plugins = win->findChildren<TopicViewer *>();
    ASSERT_EQ(plugins.size(), 1);

    auto plugin = plugins[0];
    ASSERT_NE(nullptr, plugin);

    // ============= Model =====================
    auto model = plugin->Model();
    ASSERT_NE(model, nullptr);

    auto root = model->invisibleRootItem();
    ASSERT_NE(model, nullptr);

    ASSERT_EQ(root->hasChildren(), true);

    bool foundCollision = false;
    bool foundInt = false;

    EXPECT_GE(root->rowCount(), 2);

    // check plotable items
    for (int i = 0; i < root->rowCount(); ++i)
    {
        auto child = root->child(i);

        if (child->data(NAME_ROLE) == "/collision_topic")
        {
            foundCollision = true;

            EXPECT_EQ(child->data(TYPE_ROLE), "gz.msgs.Collision");
            EXPECT_EQ(child->rowCount(), 8);

            auto pose = child->child(5);
            auto position = pose->child(3);
            auto x = position->child(1);

            EXPECT_EQ(x->data(NAME_ROLE), "x");
            EXPECT_EQ(x->data(TYPE_ROLE), "double");
            EXPECT_EQ(x->data(PATH_ROLE), "pose-position-x");
            EXPECT_EQ(x->data(TOPIC_ROLE), "/collision_topic");
            EXPECT_TRUE(x->data(PLOT_ROLE).toBool());
        }
        else if (child->data(NAME_ROLE) == "/int_topic")
        {
            foundInt = true;

            EXPECT_EQ(child->data(TYPE_ROLE), "gz.msgs.Int32");
            EXPECT_EQ(child->rowCount(), 2);

            auto data = child->child(1);

            EXPECT_EQ(data->data(NAME_ROLE), "data");
            EXPECT_EQ(data->data(TYPE_ROLE), "int32");
            EXPECT_EQ(data->data(PATH_ROLE), "data");
            EXPECT_EQ(data->data(TOPIC_ROLE), "/int_topic");
            EXPECT_TRUE(data->data(PLOT_ROLE).toBool());
        }
        else
        {
            EXPECT_TRUE(false);
        }
    }

    EXPECT_TRUE(foundCollision);
    EXPECT_TRUE(foundInt);

    // =========== Dynamic Adding / Removing =============

    // Add
    auto pubEcho = node.Advertise<msgs::Collision> ("/echo_topic");
    msgs::Collision msgEcho;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    pubEcho.Publish(msgEcho);
    // Remove
    node.Unsubscribe("/int_topic");
    // wait for update timeout
    std::this_thread::sleep_for(std::chrono::milliseconds(700));

    root = plugin->Model()->invisibleRootItem();

    EXPECT_EQ(root->rowCount(), 2);
}
