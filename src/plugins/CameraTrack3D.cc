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

#include <string>

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/rendering.hh>

#include "ignition/gui/BoolWidget.hh"
#include "ignition/gui/NumberWidget.hh"
#include "ignition/gui/EnumWidget.hh"
#include "ignition/gui/QtMetatypes.hh"
#include "ignition/gui/Vector3dWidget.hh"
#include "ignition/gui/plugins/CameraTrack3D.hh"

// Value for empty selection
static const std::string kEmpty = "None";

namespace ignition
{
namespace gui
{
namespace plugins
{
  class CameraTrack3DPrivate
  {
    /// \brief Pointer to scene
    public: rendering::ScenePtr scene;

    /// \brief Camera enum widget
    public: EnumWidget *cameraEnum;

    /// \brief Track target enum widget
    public: EnumWidget *trackTarget;

    /// \brief Track offset widget
    public: Vector3dWidget *trackOffset;

    /// \brief Track frame widget
    public: BoolWidget *trackFrame;

    /// \brief Track gain widget
    public: NumberWidget *trackGain;

    /// \brief Follow target enum widget
    public: EnumWidget *followTarget;

    /// \brief Follow offset widget
    public: Vector3dWidget *followOffset;

    /// \brief Follow frame widget
    public: BoolWidget *followFrame;

    /// \brief Follow gain widget
    public: NumberWidget *followGain;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
CameraTrack3D::CameraTrack3D()
  : Plugin(), dataPtr(new CameraTrack3DPrivate)
{
}

/////////////////////////////////////////////////
CameraTrack3D::~CameraTrack3D()
{
}

/////////////////////////////////////////////////
void CameraTrack3D::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "3D Camera tracking";

  // Default parameters
  std::string engineName{"ogre"};
  std::string sceneName{"scene"};

  // Custom parameters
  if (_pluginElem)
  {
    if (auto elem = _pluginElem->FirstChildElement("engine"))
      engineName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("scene"))
      sceneName = elem->GetText();
  }

  // Render engine
  auto engine = rendering::engine(engineName);
  if (!engine)
  {
    ignerr << "Engine [" << engineName << "] is not supported" << std::endl;
    return;
  }

  // Scene
  this->dataPtr->scene = engine->SceneByName(sceneName);
  if (!this->dataPtr->scene)
  {
    ignerr << "Scene [" << sceneName << "] not found" << std::endl;
    return;
  }

  // Cameras
  this->dataPtr->cameraEnum = new EnumWidget("Camera");
  this->connect(this->dataPtr->cameraEnum, &PropertyWidget::ValueChanged, this,
      &CameraTrack3D::OnCameraChange);;

  auto cameraListView = this->dataPtr->cameraEnum->findChild<QListView *>();
  cameraListView->setObjectName("cameraListView");
  cameraListView->installEventFilter(this);

  // Track
  auto trackLabel = new QLabel("<b>TRACK</b>");
  auto trackLabelLayout = new QHBoxLayout();
  trackLabelLayout->addWidget(trackLabel);
  trackLabelLayout->setContentsMargins(10, 0, 0, 0);

  // Track target
  this->dataPtr->trackTarget = new EnumWidget("Target");
  this->connect(this->dataPtr->trackTarget, &PropertyWidget::ValueChanged, this,
      &CameraTrack3D::OnTrackChange);;

  auto trackTargetView = this->dataPtr->trackTarget->findChild<QListView *>();
  trackTargetView->setObjectName("trackTargetView");
  trackTargetView->installEventFilter(this);

  // Track offset
  auto trackOffsetLabel = new QLabel("Offset");
  auto trackOffsetLabelLayout = new QHBoxLayout();
  trackOffsetLabelLayout->addWidget(trackOffsetLabel);
  trackOffsetLabelLayout->setContentsMargins(10, 0, 0, 0);

  this->dataPtr->trackOffset = new Vector3dWidget("Offset");
  this->connect(this->dataPtr->trackOffset, &PropertyWidget::ValueChanged, this,
      &CameraTrack3D::OnTrackChange);

  // Track frame
  this->dataPtr->trackFrame = new BoolWidget("World frame");
  this->dataPtr->trackFrame->SetValue(false);
  this->connect(this->dataPtr->trackFrame, &PropertyWidget::ValueChanged, this,
      &CameraTrack3D::OnTrackChange);

  // Track gain
  this->dataPtr->trackGain = new NumberWidget("Proportional gain");
  this->dataPtr->trackGain->SetValue(1.0);
  this->connect(this->dataPtr->trackGain, &PropertyWidget::ValueChanged, this,
      &CameraTrack3D::OnTrackChange);

  // Follow
  auto followLabel = new QLabel("<b>FOLLOW</b>");
  auto followLabelLayout = new QHBoxLayout();
  followLabelLayout->addWidget(followLabel);
  followLabelLayout->setContentsMargins(10, 0, 0, 0);

  // Follow target
  this->dataPtr->followTarget = new EnumWidget("Target");
  this->connect(this->dataPtr->followTarget, &PropertyWidget::ValueChanged,
      this, &CameraTrack3D::OnFollowChange);

  auto followTargetView = this->dataPtr->followTarget->findChild<QListView *>();
  followTargetView->setObjectName("followTargetView");
  followTargetView->installEventFilter(this);

  // Follow offset
  auto followOffsetLabel = new QLabel("Offset");
  auto followOffsetLabelLayout = new QHBoxLayout();
  followOffsetLabelLayout->addWidget(followOffsetLabel);
  followOffsetLabelLayout->setContentsMargins(10, 0, 0, 0);

  this->dataPtr->followOffset = new Vector3dWidget("Offset");
  this->dataPtr->followOffset->SetValue(QVariant::fromValue(
      math::Vector3d(-10, 10, 10)));
  this->connect(this->dataPtr->followOffset, &PropertyWidget::ValueChanged,
      this, &CameraTrack3D::OnFollowChange);;

  // Follow frame
  this->dataPtr->followFrame = new BoolWidget("World frame");
  this->dataPtr->followFrame->SetValue(false);
  this->connect(this->dataPtr->followFrame, &PropertyWidget::ValueChanged,
      this, &CameraTrack3D::OnFollowChange);;

  // Follow gain
  this->dataPtr->followGain = new NumberWidget("Proportional gain");
  this->dataPtr->followGain->SetValue(1.0);
  this->connect(this->dataPtr->followGain, &PropertyWidget::ValueChanged,
      this, &CameraTrack3D::OnFollowChange);;

  // Spacer
  auto spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // Layout
  auto mainLayout = new QGridLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);

  mainLayout->addWidget(this->dataPtr->cameraEnum, 0, 0, 1, 2);

  mainLayout->addLayout(trackLabelLayout, 1, 0);
  mainLayout->addWidget(this->dataPtr->trackTarget, 2, 0, 1, 2);
  mainLayout->addLayout(trackOffsetLabelLayout, 3, 0);
  mainLayout->addWidget(this->dataPtr->trackOffset, 3, 1);
  mainLayout->addWidget(this->dataPtr->trackFrame, 4, 0, 1, 2);
  mainLayout->addWidget(this->dataPtr->trackGain, 5, 0, 1, 2);

  mainLayout->addLayout(followLabelLayout, 6, 0);
  mainLayout->addWidget(this->dataPtr->followTarget, 7, 0, 1, 2);
  mainLayout->addLayout(followOffsetLabelLayout, 8, 0);
  mainLayout->addWidget(this->dataPtr->followOffset, 8, 1);
  mainLayout->addWidget(this->dataPtr->followFrame, 9, 0, 1, 2);
  mainLayout->addWidget(this->dataPtr->followGain, 10, 0, 1, 2);

  mainLayout->addWidget(spacer, 11, 0, 1, 2);

  this->setLayout(mainLayout);

  // Refresh
  this->RefreshCameraList();
  this->RefreshTargetLists();
}

/////////////////////////////////////////////////
void CameraTrack3D::OnCameraChange(const QVariant &_value)
{
  if (!_value.canConvert<std::string>())
  {
    ignerr << "Value is not a string" << std::endl;
    return;
  }

  auto value = _value.value<std::string>();

  // Get new camera
  auto camera = std::dynamic_pointer_cast<rendering::Camera>(
      this->dataPtr->scene->SensorByName(value));

  // Enable widgets
  this->dataPtr->trackTarget->SetReadOnly(camera == nullptr);
  this->dataPtr->trackOffset->SetReadOnly(camera == nullptr);
  this->dataPtr->trackFrame->SetReadOnly(camera == nullptr);
  this->dataPtr->trackGain->SetReadOnly(camera == nullptr);
  this->dataPtr->followTarget->SetReadOnly(camera == nullptr);
  this->dataPtr->followOffset->SetReadOnly(camera == nullptr);
  this->dataPtr->followFrame->SetReadOnly(camera == nullptr);
  this->dataPtr->followGain->SetReadOnly(camera == nullptr);

  if (!camera)
    return;

  // Update widgets
  auto target = camera->TrackTarget();
  if (target)
  {
    this->dataPtr->trackTarget->SetValue(QVariant::fromValue(target->Name()));
    this->dataPtr->trackOffset->SetValue(QVariant::fromValue(
        camera->TrackOffset()));
    this->dataPtr->trackGain->SetValue(QVariant::fromValue(
        camera->TrackPGain()));
    // TODO: world frame
  }
  else
  {
    this->dataPtr->trackTarget->SetValue(QVariant::fromValue(kEmpty));
  }

  target = camera->FollowTarget();
  if (target)
  {
    this->dataPtr->followTarget->SetValue(QVariant::fromValue(target->Name()));
    this->dataPtr->followOffset->SetValue(QVariant::fromValue(
        camera->FollowOffset()));
    this->dataPtr->followGain->SetValue(QVariant::fromValue(
        camera->FollowPGain()));
    // TODO: world frame
  }
  else
  {
    this->dataPtr->followTarget->SetValue(QVariant::fromValue(kEmpty));
  }
}

/////////////////////////////////////////////////
void CameraTrack3D::OnTrackChange(const QVariant &/*_value*/)
{
  // Camera
  auto cameraName = this->dataPtr->cameraEnum->Value().value<std::string>();

  auto camera = std::dynamic_pointer_cast<rendering::Camera>(
      this->dataPtr->scene->SensorByName(cameraName));
  if (!camera)
  {
    ignerr << "Can't find camera [" << cameraName << "]" << std::endl;
    return;
  }

  // Target
  auto targetName = this->dataPtr->trackTarget->Value().value<std::string>();

  auto target = this->dataPtr->scene->NodeByName(targetName);
  if (!target)
  {
    camera->SetTrackTarget(nullptr);
    return;
  }

  // Offset
  auto offset = this->dataPtr->trackOffset->Value()
      .value<math::Vector3d>();

  // Frame
  auto worldFrame = this->dataPtr->trackFrame->Value().toBool();

  // Gain
  auto gain = this->dataPtr->trackGain->Value().toDouble();

  // Set
  camera->SetTrackTarget(target, offset, worldFrame);
  camera->SetTrackPGain(gain);
}

/////////////////////////////////////////////////
void CameraTrack3D::OnFollowChange(const QVariant &/*_value*/)
{
  // Camera
  auto cameraName = this->dataPtr->cameraEnum->Value().value<std::string>();

  auto camera = std::dynamic_pointer_cast<rendering::Camera>(
      this->dataPtr->scene->SensorByName(cameraName));
  if (!camera)
  {
    ignerr << "Can't find camera [" << cameraName << "]" << std::endl;
    return;
  }

  // Target
  auto targetName = this->dataPtr->followTarget->Value().value<std::string>();

  auto target = this->dataPtr->scene->NodeByName(targetName);
  if (!target)
  {
    camera->SetFollowTarget(nullptr);
    return;
  }

  // Offset
  auto offset = this->dataPtr->followOffset->Value()
      .value<math::Vector3d>();

  // Frame
  auto worldFrame = this->dataPtr->followFrame->Value().toBool();

  // Gain
  auto gain = this->dataPtr->followGain->Value().toDouble();

  // Set
  camera->SetFollowTarget(target, offset, worldFrame);
  camera->SetFollowPGain(gain);
}

/////////////////////////////////////////////////
bool CameraTrack3D::eventFilter(QObject *_obj, QEvent *_e)
{
  // Refresh cameras
  if (_e->type() == QEvent::Show)
  {
    if (_obj->objectName() == "cameraListView")
    {
      this->RefreshCameraList();
    }

    if (_obj->objectName() == "trackTargetView" ||
        _obj->objectName() == "followTargetView")
    {
      this->RefreshTargetLists();
    }
  }

  return QObject::eventFilter(_obj, _e);
}

/////////////////////////////////////////////////
void CameraTrack3D::RefreshCameraList()
{
  // Clear
  this->dataPtr->cameraEnum->Clear();

  // Empty option
  this->dataPtr->cameraEnum->AddItem(kEmpty);

  // Cameras
  for (unsigned int i = 0; i < this->dataPtr->scene->SensorCount(); ++i)
  {
    auto sensor = this->dataPtr->scene->SensorByIndex(i);
    auto camera = std::dynamic_pointer_cast<rendering::Camera>(sensor);
    if (!camera)
      continue;

    this->dataPtr->cameraEnum->AddItem(camera->Name());
  }
}

/////////////////////////////////////////////////
void CameraTrack3D::RefreshTargetLists()
{
  // Current selection
  auto currentTrack = this->dataPtr->trackTarget->Value().value<std::string>();
  auto currentFollow = this->dataPtr->followTarget->Value().value<std::string>();

  // Clear
  this->dataPtr->trackTarget->Clear();
  this->dataPtr->followTarget->Clear();

  // Empty option
  this->dataPtr->trackTarget->AddItem(kEmpty);
  this->dataPtr->followTarget->AddItem(kEmpty);

  // Nodes
  for (unsigned int i = 0; i < this->dataPtr->scene->NodeCount(); ++i)
  {
    auto node = this->dataPtr->scene->NodeByIndex(i);
    if (!node)
      continue;

    this->dataPtr->trackTarget->AddItem(node->Name());
    this->dataPtr->followTarget->AddItem(node->Name());
  }

  // Restore selection
  this->dataPtr->trackTarget->SetValue(QVariant::fromValue(currentTrack));
  this->dataPtr->followTarget->SetValue(QVariant::fromValue(currentFollow));
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::CameraTrack3D,
                                  ignition::gui::Plugin)

