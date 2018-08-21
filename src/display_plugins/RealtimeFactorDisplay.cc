/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#include <iomanip>

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/rendering/Text.hh>
#include <ignition/transport.hh>

#include "ignition/gui/NumberWidget.hh"
#include "ignition/gui/display_plugins/RealtimeFactorDisplay.hh"

namespace ignition
{
namespace gui
{
namespace display_plugins
{
  class RealtimeFactorDisplayPrivate
  {
    /// \brief Message holding latest world statistics
    public: ignition::msgs::WorldStatistics msg;

    /// \brief Mutex to protect msg
    public: std::recursive_mutex mutex;

    /// \brief Communication node
    public: ignition::transport::Node node;

    /// \brief The text display
    public: ignition::rendering::TextPtr realtimeFactorText = nullptr;

    /// \brief The camera to which the display is attached
    public: std::shared_ptr<rendering::Camera> cameraAttachedTo = nullptr;

    /// \brief Horizontal padding away from the image border
    public: int horizontalPadding = 20;

    /// \brief Vertical padding away from the image border
    public: int verticalPadding = 20;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace display_plugins;

/////////////////////////////////////////////////
RealtimeFactorDisplay::RealtimeFactorDisplay()
  : DisplayPlugin(), dataPtr(new RealtimeFactorDisplayPrivate)
{
  this->title = "Real time factor";
}

/////////////////////////////////////////////////
RealtimeFactorDisplay::~RealtimeFactorDisplay()
{
}

/////////////////////////////////////////////////
void RealtimeFactorDisplay::Initialize(
  const tinyxml2::XMLElement */*_pluginElem*/)
{
  // Subscribe to world_stats
  std::string topic = "/world_stats";
  if (!this->dataPtr->node.Subscribe(topic,
      &RealtimeFactorDisplay::OnWorldStatsMsg, this))
  {
    ignerr << "Failed to subscribe to [" << topic << "]" << std::endl;
  }

  if (auto scenePtr = this->Scene().lock())
  {
    this->dataPtr->realtimeFactorText = scenePtr->CreateText();
    // By default the Visual is attached to the RootVisual.
    // Remove it from the RootVisual so it can be attached to a Camera instead.
    auto root = scenePtr->RootVisual();
    root->RemoveChild(this->Visual());

    if (auto sensorPtr = scenePtr->SensorByName("user_camera"))
    {
      fprintf(stderr, "Found camera\n");
      auto cameraPtr = std::dynamic_pointer_cast<rendering::Camera>(sensorPtr);
      if (cameraPtr)
      {
        auto cameraVisual = std::dynamic_pointer_cast<rendering::Visual>(
          cameraPtr->Parent());
        if (cameraVisual)
        {
          cameraVisual->AddChild(this->Visual());
          this->dataPtr->cameraAttachedTo = cameraPtr;
        }
        else
        {
          ignerr << "Parent of camera with requested name [" << "user_camera" << "] is not a Visual." << std::endl;
        }
      }
      else
      {
        ignerr << "Sensor with requested name [" << "user_camera" << "] is not a camera." << std::endl;
      }
    }
    /*
    // Loop through the children looking for a Camera.
    for (unsigned int i = 0; i < root->ChildCount(); ++i)
    {
      auto camera = std::dynamic_pointer_cast<rendering::Camera>(
        root->ChildByIndex(i));
      if (!camera)
      {
        fprintf(stderr, "I am not a camera\n");
        continue;
      }
      auto cameraVisual = std::dynamic_pointer_cast<rendering::Visual>(
        camera->Parent());
      if (!cameraVisual)
      {
        fprintf(stderr, "I am not a visual\n");
        continue;
      }
      // Camera with parent visual found: attach to it if not already attached.
      if (!this->dataPtr->cameraAttachedTo)
      {
        cameraVisual->AddChild(this->Visual());
        this->dataPtr->cameraAttachedTo = camera;
        continue;
      }
      ignwarn << "Multiple cameras found for scene [" << scenePtr->Name() <<
        "]. Real time factor display will be attached to the first camera "
        "and may be visible from other cameras." << std::endl;
    }
    */
  }
  else
  {
    ignerr << "Scene invalid. Real time factor display not initialized."
      << std::endl;
    return;
  }
  if (!this->dataPtr->cameraAttachedTo)
  {
    ignerr << "Camera not found. Real time factor display not initialized."
      << std::endl;
    return;
  }
  this->dataPtr->realtimeFactorText->SetTextString("Real time factor: ? %");
  this->dataPtr->realtimeFactorText->SetShowOnTop(true);
  // TODO(dhood): Allow text to be aligned to different corners of image.
  // TODO(dhood): I don't think right alignment is working correctly,
  // so will focus on left-aligned for now.
  this->dataPtr->realtimeFactorText->SetTextAlignment(
    ignition::rendering::TextHorizontalAlign::LEFT,
    ignition::rendering::TextVerticalAlign::BOTTOM);

  // TODO(dhood): Configurable properties
  this->Visual()->AddGeometry(this->dataPtr->realtimeFactorText);

  this->UpdateTextPose();
}

/////////////////////////////////////////////////
QWidget *RealtimeFactorDisplay::CreateCustomProperties() const
{
  auto horizontalPaddingWidget = new NumberWidget("Horizontal padding",
      NumberType::INT);
  horizontalPaddingWidget->SetValue(
    QVariant::fromValue(this->dataPtr->horizontalPadding));
  horizontalPaddingWidget->setObjectName("horizontalPaddingWidget");
  this->connect(horizontalPaddingWidget, SIGNAL(ValueChanged(QVariant)), this,
      SLOT(OnChange(QVariant)));

  auto verticalPaddingWidget = new NumberWidget("Vertical padding",
      NumberType::INT);
  verticalPaddingWidget->SetValue(
    QVariant::fromValue(this->dataPtr->verticalPadding));
  verticalPaddingWidget->setObjectName("verticalPaddingWidget");
  this->connect(verticalPaddingWidget, SIGNAL(ValueChanged(QVariant)), this,
      SLOT(OnChange(QVariant)));

  auto layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(horizontalPaddingWidget);
  layout->addWidget(verticalPaddingWidget);
  auto widget = new QWidget();
  widget->setLayout(layout);

  return widget;
}

/////////////////////////////////////////////////
void RealtimeFactorDisplay::OnChange(const QVariant &_value)
{
  auto type = this->sender()->objectName().toStdString();

  if (type == "horizontalPaddingWidget")
    this->dataPtr->horizontalPadding = _value.toInt();
  else if (type == "verticalPaddingWidget")
    this->dataPtr->verticalPadding = _value.toInt();
}


/////////////////////////////////////////////////
void RealtimeFactorDisplay::UpdateTextPose()
{
  if (!this->dataPtr->cameraAttachedTo || !this->dataPtr->realtimeFactorText)
  {
    return;
  }

  std::cout << this->dataPtr->cameraAttachedTo->Parent()->WorldPosition() << std::endl;
  double imgWidth = (double)this->dataPtr->cameraAttachedTo->ImageWidth();
  double imgHeight = (double)this->dataPtr->cameraAttachedTo->ImageHeight();

  // Keep the same text height with wider images (image height doesn't affect).
  double charHeight = 200.0 / imgWidth;
  this->dataPtr->realtimeFactorText->SetCharHeight(charHeight);
  this->dataPtr->realtimeFactorText->SetSpaceWidth(0.7 * charHeight);

  // Re-position the text so it's in the bottom left.
  auto projMx = this->dataPtr->cameraAttachedTo->ProjectionMatrix();
  double scale = 5.0 * projMx(0, 0);  // Distance to display from camera.
  // (x, y) are in film coordinates: origin at center of image, +x left, +y up.
  double x = 1 - this->dataPtr->horizontalPadding/imgWidth;
  double y = 1 - this->dataPtr->horizontalPadding/imgHeight;
  // Convert to camera coordinates.
  // Coordinate axes of the camera are: positive X is into the scene, positive
  // Y is to the left, and positive Z is up.
  double leftOfImage = (scale * x - projMx(0, 2) - projMx(0, 3)) / projMx(0, 0);
  double topOfImage = (scale * y - projMx(1, 2) - projMx(1, 3)) / projMx(1, 1);
  // Lift proportinal to text world height.
  double almostBottomOfImage = -topOfImage + 0.8 * charHeight;
  this->Visual()->SetLocalPosition(scale, leftOfImage, almostBottomOfImage);
}

/////////////////////////////////////////////////
void RealtimeFactorDisplay::ProcessMsg()
{
  if (nullptr == this->dataPtr->realtimeFactorText)
  {
    return;
  }
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  if (this->dataPtr->msg.has_real_time_factor())
  {
    // RTF as a percentage.
    double rtf = this->dataPtr->msg.real_time_factor() * 100;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Real time factor: " << rtf << "%";
    this->dataPtr->realtimeFactorText->SetTextString(ss.str());
  }
  // TODO(dhood): trigger this from window resize.
  this->UpdateTextPose();
}

/////////////////////////////////////////////////
void RealtimeFactorDisplay::OnWorldStatsMsg(
    const ignition::msgs::WorldStatistics &_msg)
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  this->dataPtr->msg.CopyFrom(_msg);
  QMetaObject::invokeMethod(this, "ProcessMsg");
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(
  ignition::gui::display_plugins::RealtimeFactorDisplay,
  ignition::gui::DisplayPlugin)
