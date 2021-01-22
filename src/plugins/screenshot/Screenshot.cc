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

#include <ignition/common/Filesystem.hh>
#include <ignition/plugin/Register.hh>

#include "ignition/gui/Application.hh"
#include "ignition/gui/GuiEvents.hh"
#include "ignition/gui/MainWindow.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class ScreenshotPrivate
  {
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
  std::string home;
  common::env(IGN_HOMEDIR, home);

  // default directory
  this->dataPtr->directory = common::joinPaths(home, ".ignition", "gazebo");

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

  // For screenshot requests
  // ignition::gui::App()->findChild
    // <ignition::gui::MainWindow *>()->installEventFilter(this);
}

/////////////////////////////////////////////////
void Screenshot::OnScreenshot()
{
  std::cout << "SCREENSHOT REQUESTED" << std::endl;
  std::cout << "file will be saved to: " << this->dataPtr->directory << std::endl;

  std::string time = common::systemTimeISO();
  std::cout << "time: " << time << std::endl;
}

/////////////////////////////////////////////////
void Screenshot::OnChangeDirectory(const QString &_dirUrl)
{
  QString newDir = QUrl(_dirUrl).toLocalFile();
  std::cout << "newpath: " << newDir.toStdString() << std::endl;
  this->dataPtr->directory = newDir.toStdString();

  // Update tooltip text
  App()->Engine()->rootContext()->setContextProperty("ScreenshotDirectory",
      newDir);
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::Screenshot,
                    ignition::gui::Plugin)
