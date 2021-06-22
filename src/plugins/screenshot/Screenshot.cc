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

#include <string>

#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/common/Image.hh>
#include <ignition/plugin/Register.hh>

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <ignition/rendering/Camera.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/Scene.hh>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <ignition/transport/Node.hh>

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
    /// \brief Node for communication
    public: ignition::transport::Node node;

    /// \brief Screenshot service name
    public: std::string screenshotService;

    /// \brief Directory to save screenshots
    public: std::string directory;

    /// \brief Whether a screenshot has been requested but not processed yet.
    public: bool dirty{false};

    /// \brief Pointer to the user camera.
    public: ignition::rendering::CameraPtr userCamera{nullptr};

    /// \brief Saved screenshot filepath
    public: QString savedScreenshotPath = "";
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
  this->dataPtr->directory =
      common::joinPaths(home, ".ignition", "gui", "pictures");

  if (!common::exists(this->dataPtr->directory))
  {
    if (!common::createDirectories(this->dataPtr->directory))
    {
      std::string defaultDir = common::joinPaths(home, ".ignition", "gui");
      ignerr << "Unable to create directory [" << this->dataPtr->directory
             << "]. Changing default directory to: " << defaultDir
             << std::endl;

      this->dataPtr->directory = defaultDir;
    }
  }

  this->DirectoryChanged();
}

/////////////////////////////////////////////////
Screenshot::~Screenshot() = default;

/////////////////////////////////////////////////
void Screenshot::LoadConfig(const tinyxml2::XMLElement *)
{
  if (this->title.empty())
    this->title = "Screenshot";

  // Screenshot service
  this->dataPtr->screenshotService = "/gui/screenshot";
  this->dataPtr->node.Advertise(this->dataPtr->screenshotService,
      &Screenshot::ScreenshotService, this);
  ignmsg << "Screenshot service on ["
         << this->dataPtr->screenshotService << "]" << std::endl;

  App()->findChild<MainWindow *>()->installEventFilter(this);
}

/////////////////////////////////////////////////
bool Screenshot::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == events::Render::kType && this->dataPtr->dirty)
  {
    this->SaveScreenshot();
  }

  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}

/////////////////////////////////////////////////
bool Screenshot::ScreenshotService(const msgs::StringMsg &_msg,
  msgs::Boolean &_res)
{
  if (!_msg.data().empty())
    this->dataPtr->directory = _msg.data();
  this->dataPtr->dirty = true;
  _res.set_data(true);
  return true;
}

/////////////////////////////////////////////////
void Screenshot::SaveScreenshot()
{
  this->FindUserCamera();

  if (nullptr == this->dataPtr->userCamera)
    return;

  unsigned int width = this->dataPtr->userCamera->ImageWidth();
  unsigned int height = this->dataPtr->userCamera->ImageHeight();

  auto cameraImage = this->dataPtr->userCamera->CreateImage();
  this->dataPtr->userCamera->Copy(cameraImage);
  auto formatStr =
      rendering::PixelUtil::Name(this->dataPtr->userCamera->ImageFormat());
  auto format = common::Image::ConvertPixelFormat(formatStr);

  std::string time = common::systemTimeISO() + ".png";
  std::string savePath = common::joinPaths(this->dataPtr->directory, time);

  common::Image image;
  image.SetFromData(cameraImage.Data<unsigned char>(), width, height, format);
  image.SavePNG(savePath);

  igndbg << "Saved image to [" << savePath << "]" << std::endl;

  this->dataPtr->dirty = false;

  this->SetSavedScreenshotPath(QString::fromStdString(savePath));
}

/////////////////////////////////////////////////
void Screenshot::FindUserCamera()
{
  if (nullptr != this->dataPtr->userCamera)
    return;

  // Get first scene
  auto scene = rendering::sceneFromFirstRenderEngine();

  for (unsigned int i = 0; i < scene->NodeCount(); ++i)
  {
    auto cam = std::dynamic_pointer_cast<rendering::Camera>(
        scene->NodeByIndex(i));
    if (nullptr != cam)
    {
      this->dataPtr->userCamera = cam;
      igndbg << "Screnshot plugin taking pictures of camera ["
             << this->dataPtr->userCamera->Name() << "]" << std::endl;
      break;
    }
  }
}

/////////////////////////////////////////////////
void Screenshot::OnScreenshot()
{
  this->dataPtr->dirty = true;
}

/////////////////////////////////////////////////
QString Screenshot::Directory() const
{
  return QString::fromStdString(this->dataPtr->directory);
}

/////////////////////////////////////////////////
void Screenshot::SetDirectory(const QString &_dirUrl)
{
  QString newDir = QUrl(_dirUrl).toLocalFile();
  this->dataPtr->directory = newDir.toStdString();
  this->DirectoryChanged();
}

/////////////////////////////////////////////////
QString Screenshot::SavedScreenshotPath() const
{
  return this->dataPtr->savedScreenshotPath;
}

/////////////////////////////////////////////////
void Screenshot::SetSavedScreenshotPath(const QString &_filename)
{
  this->dataPtr->savedScreenshotPath = _filename;
  this->SavedScreenshotPathChanged();
  this->savedScreenshot();
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::Screenshot,
                    ignition::gui::Plugin)
