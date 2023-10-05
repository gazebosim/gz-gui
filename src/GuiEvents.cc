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

#include "gz/gui/GuiEvents.hh"

namespace gz::gui::events
{

class events::SnapIntervals::Implementation
{
  /// \brief XYZ snapping values in meters, these values must be positive.
  public: math::Vector3d xyz;

  /// \brief RPY snapping values in degrees, these values must be
  /// positive.
  public: math::Vector3d rpy;

  /// \brief Scale snapping values - a multiplier of the current size,
  /// these values must be positive.
  public: math::Vector3d scale;
};

class SpawnFromDescription::Implementation
{
  /// \brief The string of the resource to be spawned.
  public: std::string description;
};

class SpawnFromPath::Implementation
{
  /// \brief The path of file to be previewed.
  public: std::string filePath;
};

class HoverToScene::Implementation
{
  /// \brief The 3D point over which the user is hovering.
  public: math::Vector3d point;
};

class HoverOnScene::Implementation
{
  /// \brief The 2D point over which the user is hovering.
  public: common::MouseEvent mouse;
};

class LeftClickToScene::Implementation
{
  /// \brief The 3D point that the user clicked within the scene.
  public: math::Vector3d point;
};

class RightClickToScene::Implementation
{
  /// \brief The 3D point that the user clicked within the scene.
  public: math::Vector3d point;
};

class DropdownMenuEnabled::Implementation
{
  /// \brief The boolean indicating whether the menu is disabled or not
  /// for this event.
  public: bool menuEnabled;
};

class LeftClickOnScene::Implementation
{
  /// \brief Mouse event
  public: common::MouseEvent mouse;
};

class RightClickOnScene::Implementation
{
  /// \brief Mouse event
  public: common::MouseEvent mouse;
};

class BlockOrbit::Implementation
{
  public: bool block;
};

class KeyReleaseOnScene::Implementation
{
  /// \brief Key event
  public: common::KeyEvent key;
};

class KeyPressOnScene::Implementation
{
  /// \brief Key event
  public: common::KeyEvent key;
};

class SpawnCloneFromName::Implementation
{
  /// \brief The name of the resource to be cloned
  public: std::string name;
};

class DropOnScene::Implementation
{
  /// \brief The name of the dropped thing
  public: std::string dropText;

  /// \brief X and Y position of the mouse
  public: gz::math::Vector2i mouse;
};

class ScrollOnScene::Implementation
{
  /// \brief Mouse event with scroll information.
  public: common::MouseEvent mouse;
};

class DragOnScene::Implementation
{
  /// \brief Mouse event with drag information.
  public: common::MouseEvent mouse;
};

class MousePressOnScene::Implementation
{
  /// \brief Mouse event with press information.
  public: common::MouseEvent mouse;
};

class WorldControl::Implementation
{
  /// \brief WorldControl information.
  public: msgs::WorldControl worldControl;
};

class PreRender::Implementation
{
};

/////////////////////////////////////////////////
SnapIntervals::SnapIntervals(
            const math::Vector3d &_xyz,
            const math::Vector3d &_rpy,
            const math::Vector3d &_scale)
  : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->xyz = _xyz;
  this->dataPtr->rpy = _rpy;
  this->dataPtr->scale = _scale;
}

/////////////////////////////////////////////////
math::Vector3d SnapIntervals::Position() const
{
  return this->dataPtr->xyz;
}

/////////////////////////////////////////////////
math::Vector3d SnapIntervals::Rotation() const
{
  return this->dataPtr->rpy;
}

/////////////////////////////////////////////////
math::Vector3d SnapIntervals::Scale() const
{
  return this->dataPtr->scale;
}

/////////////////////////////////////////////////
SpawnFromDescription::SpawnFromDescription(const std::string &_description)
  : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->description = _description;
}

/////////////////////////////////////////////////
const std::string &SpawnFromDescription::Description() const
{
  return this->dataPtr->description;
}

/////////////////////////////////////////////////
SpawnFromPath::SpawnFromPath(const std::string &_filePath)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->filePath = _filePath;
}

/////////////////////////////////////////////////
const std::string &SpawnFromPath::FilePath() const
{
  return this->dataPtr->filePath;
}

/////////////////////////////////////////////////
HoverToScene::HoverToScene(const math::Vector3d &_point)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->point = _point;
}

/////////////////////////////////////////////////
math::Vector3d HoverToScene::Point() const
{
  return this->dataPtr->point;
}

/////////////////////////////////////////////////
HoverOnScene::HoverOnScene(const common::MouseEvent &_mouse)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->mouse = _mouse;
}

/////////////////////////////////////////////////
common::MouseEvent HoverOnScene::Mouse() const
{
  return this->dataPtr->mouse;
}

/////////////////////////////////////////////////
LeftClickToScene::LeftClickToScene(const math::Vector3d &_point)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->point = _point;
}

/////////////////////////////////////////////////
math::Vector3d LeftClickToScene::Point() const
{
  return this->dataPtr->point;
}

/////////////////////////////////////////////////
RightClickToScene::RightClickToScene(const math::Vector3d &_point)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->point = _point;
}

/////////////////////////////////////////////////
math::Vector3d RightClickToScene::Point() const
{
  return this->dataPtr->point;
}

/////////////////////////////////////////////////
DropdownMenuEnabled::DropdownMenuEnabled(bool _menuEnabled)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->menuEnabled = _menuEnabled;
}

/////////////////////////////////////////////////
bool DropdownMenuEnabled::MenuEnabled() const
{
  return this->dataPtr->menuEnabled;
}

/////////////////////////////////////////////////
RightClickOnScene::RightClickOnScene(const common::MouseEvent &_mouse)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->mouse = _mouse;
}

/////////////////////////////////////////////////
const common::MouseEvent &RightClickOnScene::Mouse() const
{
  return this->dataPtr->mouse;
}

/////////////////////////////////////////////////
LeftClickOnScene::LeftClickOnScene(const common::MouseEvent &_mouse)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->mouse = _mouse;
}

/////////////////////////////////////////////////
const common::MouseEvent &LeftClickOnScene::Mouse() const
{
  return this->dataPtr->mouse;
}

/////////////////////////////////////////////////
BlockOrbit::BlockOrbit(const bool &_block)
  : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->block = _block;
}

/////////////////////////////////////////////////
bool BlockOrbit::Block() const
{
  return this->dataPtr->block;
}

/////////////////////////////////////////////////
KeyReleaseOnScene::KeyReleaseOnScene(
  const common::KeyEvent &_key)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->key = _key;
}

/////////////////////////////////////////////////
common::KeyEvent KeyReleaseOnScene::Key() const
{
  return this->dataPtr->key;
}

/////////////////////////////////////////////////
KeyPressOnScene::KeyPressOnScene(
  const common::KeyEvent &_key)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->key = _key;
}

/////////////////////////////////////////////////
common::KeyEvent KeyPressOnScene::Key() const
{
  return this->dataPtr->key;
}

/////////////////////////////////////////////////
SpawnCloneFromName::SpawnCloneFromName(
  const std::string &_name)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->name = _name;
}

/////////////////////////////////////////////////
const std::string &SpawnCloneFromName::Name() const
{
  return this->dataPtr->name;
}

/////////////////////////////////////////////////
DropOnScene::DropOnScene(
  const std::string &_dropText, const gz::math::Vector2i &_dropMouse)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->dropText = _dropText;
  this->dataPtr->mouse = _dropMouse;
}

/////////////////////////////////////////////////
const std::string &DropOnScene::DropText() const
{
  return this->dataPtr->dropText;
}

/////////////////////////////////////////////////
const gz::math::Vector2i &DropOnScene::Mouse() const
{
  return this->dataPtr->mouse;
}

/////////////////////////////////////////////////
ScrollOnScene::ScrollOnScene(const common::MouseEvent &_mouse)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->mouse = _mouse;
}

/////////////////////////////////////////////////
const common::MouseEvent &ScrollOnScene::Mouse() const
{
  return this->dataPtr->mouse;
}

/////////////////////////////////////////////////
DragOnScene::DragOnScene(const common::MouseEvent &_mouse)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->mouse = _mouse;
}

/////////////////////////////////////////////////
common::MouseEvent DragOnScene::Mouse() const
{
  return this->dataPtr->mouse;
}

/////////////////////////////////////////////////
MousePressOnScene::MousePressOnScene(const common::MouseEvent &_mouse)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->mouse = _mouse;
}

/////////////////////////////////////////////////
const common::MouseEvent &MousePressOnScene::Mouse() const
{
  return this->dataPtr->mouse;
}

/////////////////////////////////////////////////
WorldControl::WorldControl(const msgs::WorldControl &_worldControl)
  : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->worldControl = _worldControl;
}

/////////////////////////////////////////////////
const msgs::WorldControl &WorldControl::WorldControlInfo() const
{
  return this->dataPtr->worldControl;
}

/////////////////////////////////////////////////
PreRender::PreRender()
  : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
}
}  // namespace gz::gui::events
