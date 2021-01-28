/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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
#include "Screenshot.hh"

#include <ignition/msgs/boolean.pb.h>
#include <ignition/msgs/stringmsg.pb.h>

#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/plugin/Register.hh>
#include <ignition/transport/Node.hh>

#include "ignition/gui/Application.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class ScreenshotPrivate
  {
    /// \brief Node for communication
    public: ignition::transport::Node node;

    /// \brief Screenshot service name
    public: std::string screenshotService;

    /// \brief Directory to save screenshots
    public: std::string directory;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
Screenshot::Screenshot()
  : ignition::gui::Plugin(),
  dataPtr(std::make_unique<ScreenshotPrivate>())
{
  // for screenshot requests
  this->dataPtr->screenshotService = "/gui/screenshot";

  std::string home;
  common::env(IGN_HOMEDIR, home);

  // default directory
  this->dataPtr->directory =
      common::joinPaths(home, ".ignition", "gazebo", "pictures");

  if (!common::exists(this->dataPtr->directory))
  {
    if (!common::createDirectory(this->dataPtr->directory))
    {
      std::string defaultDir = common::joinPaths(home, ".ignition", "gazebo");
      ignerr << "Unable to create directory [" << this->dataPtr->directory
             << "]. Changing default directory to: " << defaultDir
             << std::endl;

      this->dataPtr->directory = defaultDir;
    }
  }

  // Update tooltip text
  App()->Engine()->rootContext()->setContextProperty("ScreenshotDirectory",
      QString::fromStdString(this->dataPtr->directory));
}

/////////////////////////////////////////////////
Screenshot::~Screenshot() = default;

/////////////////////////////////////////////////
void Screenshot::LoadConfig(const tinyxml2::XMLElement *)
{
  if (this->title.empty())
    this->title = "Screenshot";
}

/////////////////////////////////////////////////
void Screenshot::OnScreenshot()
{
  std::function<void(const ignition::msgs::Boolean &, const bool)> cb =
      [](const ignition::msgs::Boolean &/*_rep*/, const bool _result)
  {
    if (!_result)
      ignerr << "Error sending move to request" << std::endl;
  };

  std::string time = common::systemTimeISO() + ".png";
  std::string savedPath = common::joinPaths(this->dataPtr->directory, time);

  ignition::msgs::StringMsg req;
  req.set_data(savedPath);
  this->dataPtr->node.Request(this->dataPtr->screenshotService, req, cb);
}

/////////////////////////////////////////////////
void Screenshot::OnChangeDirectory(const QString &_dirUrl)
{
  QString newDir = QUrl(_dirUrl).toLocalFile();
  this->dataPtr->directory = newDir.toStdString();

  // Update tooltip text
  App()->Engine()->rootContext()->setContextProperty("ScreenshotDirectory",
      newDir);
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::Screenshot,
                    ignition::gui::Plugin)
