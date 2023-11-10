/*
 * Copyright (C) 2023 Open Source Robotics Foundation
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

#include <gz/utils/ImplPtr.hh>
#include <list>
#include <string>

#include <gz/common/Console.hh>
#include <gz/plugin/Register.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/GuiEvents.hh"
#include "gz/gui/MainWindow.hh"

#include "CameraFps.hh"

namespace gz::gui::plugins
{
/// \brief Private data class for CameraFps
class CameraFps::Implementation
{
  /// \brief Previous camera update time
  public: std::optional<std::chrono::steady_clock::time_point>
      prevCameraUpdateTime;

  /// \brief A moving window of camera update times
  public: std::list<std::chrono::duration<double>> cameraUpdateTimes;

  /// \brief Sum of all update times in the moving window
  public: std::chrono::duration<double> cameraUpdateTimeSum;

  /// \brief Size of camera update time window
  /// \todo(anyone) make this configurable
  public: unsigned int cameraFPSWindowSize = 20u;

  /// \brief Camera FPS string value
  public: QString cameraFPSValue;
};

/////////////////////////////////////////////////
void CameraFps::OnRender()
{
  auto now = std::chrono::steady_clock::now();
  if (!this->dataPtr->prevCameraUpdateTime.has_value())
  {
    this->dataPtr->prevCameraUpdateTime = now;
    return;
  }

  const std::chrono::duration<double> dt =
    std::chrono::steady_clock::now() - *this->dataPtr->prevCameraUpdateTime;
  this->dataPtr->prevCameraUpdateTime = now;
  this->dataPtr->cameraUpdateTimeSum += dt;
  if (this->dataPtr->cameraUpdateTimes.size() >=
      this->dataPtr->cameraFPSWindowSize)
  {
    auto first = this->dataPtr->cameraUpdateTimes.front();
    this->dataPtr->cameraUpdateTimes.pop_front();
    this->dataPtr->cameraUpdateTimeSum -= first;
    double sum = this->dataPtr->cameraUpdateTimeSum.count();
    double avg = sum /
        static_cast<double>(this->dataPtr->cameraFPSWindowSize);
    this->SetCameraFpsValue(QString::fromStdString(std::to_string(1.0/avg)));
  }
  this->dataPtr->cameraUpdateTimes.push_back(dt);
}

/////////////////////////////////////////////////
CameraFps::CameraFps()
  : dataPtr(gz::utils::MakeUniqueImpl<Implementation>())
{
}

/////////////////////////////////////////////////
CameraFps::~CameraFps() = default;

/////////////////////////////////////////////////
void CameraFps::LoadConfig(const tinyxml2::XMLElement *)
{
  if (this->title.empty())
    this->title = "Camera FPS";

  App()->findChild<MainWindow *>()->installEventFilter(this);
}

/////////////////////////////////////////////////
bool CameraFps::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == events::Render::kType)
  {
    this->OnRender();
  }
  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}

/////////////////////////////////////////////////
QString CameraFps::CameraFpsValue() const
{
  return this->dataPtr->cameraFPSValue;
}

/////////////////////////////////////////////////
void CameraFps::SetCameraFpsValue(const QString &_value)
{
  this->dataPtr->cameraFPSValue = _value;
  this->CameraFpsValueChanged();
}
}  // namespace gz::gui::plugins

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::CameraFps,
              gz::gui::Plugin)
