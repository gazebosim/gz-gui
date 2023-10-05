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
#ifndef GZ_GUI_GUIEVENTS_HH_
#define GZ_GUI_GUIEVENTS_HH_

#include <QEvent>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <gz/common/KeyEvent.hh>
#include <gz/common/MouseEvent.hh>
#include <gz/math/Vector2.hh>
#include <gz/math/Vector3.hh>
#include <gz/msgs/world_control.pb.h>
#include <gz/utils/ImplPtr.hh>

#include "gz/gui/Export.hh"

namespace gz::gui::events
{

/// User defined events should start from QEvent::MaxUser and
/// count down to avoid collision with gz-sim events

/// \brief Event called in the render thread of a 3D scene after the user
/// camera has rendered.
/// It's safe to make rendering calls in this event's callback.
class Render : public QEvent
{
  public: Render()
      : QEvent(kType)
  {
  }
  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser);
};

/// \brief The class for sending and receiving custom snap value events.
/// This event is used in the Transform Control plugin tool when the
/// user manually alters their snapping values.
class GZ_GUI_VISIBLE SnapIntervals : public QEvent
{
  /// \brief Constructor
  /// \param[in] _xyz XYZ snapping values.
  /// \param[in] _rpy RPY snapping values.
  /// \param[in] _scale Scale snapping values.
  public: SnapIntervals(
              const math::Vector3d &_xyz,
              const math::Vector3d &_rpy,
              const math::Vector3d &_scale);

  /// \brief Get the XYZ snapping values.
  /// \return The XYZ snapping values.
  public: math::Vector3d Position() const;

  /// \brief Get the RPY snapping values.
  /// \return The RPY snapping values.
  public: math::Vector3d Rotation() const;

  /// \brief Get the scale snapping values.
  /// \return The scale snapping values.
  public: math::Vector3d Scale() const;

  /// \brief The QEvent representing a snap event occurrence.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 1);

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event called to spawn a resource, given its description as a
/// string.
class GZ_GUI_VISIBLE SpawnFromDescription : public QEvent
{
  /// \brief Constructor
  /// \param[in] _description The resource's description as a string, such
  /// as an SDF file.
  public: explicit SpawnFromDescription(const std::string &_description);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 2);

  /// \brief Get the string description of the resource.
  /// \return The resource string
  public: const std::string &Description() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event called to spawn a resource, which takes the path
/// to its file.
class GZ_GUI_VISIBLE SpawnFromPath : public QEvent
{
  /// \brief Constructor
  /// \param[in] _filePath The path to a file.
  public: explicit SpawnFromPath(const std::string &_filePath);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 3);

  /// \brief Get the path of the file.
  /// \return The file path.
  public: const std::string &FilePath() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event which is called to broadcast the 3D coordinates of a
/// user's mouse hover within the scene.
class GZ_GUI_VISIBLE HoverToScene : public QEvent
{
  /// \brief Constructor
  /// \param[in] _point The point at which the mouse is hovering within
  /// the scene
  public: explicit HoverToScene(const math::Vector3d &_point);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 4);

  /// \brief Get the point within the scene over which the user is
  /// hovering.
  /// \return The 3D point
  public: math::Vector3d Point() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event which is called to broadcast the 3D coordinates of a
/// user's releasing the left button within the scene.
/// \sa LeftClickOnScene
class GZ_GUI_VISIBLE LeftClickToScene : public QEvent
{
  /// \brief Constructor
  /// \param[in] _point The point which the user has left clicked within
  /// the scene
  public: explicit LeftClickToScene(const math::Vector3d &_point);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 5);

  /// \brief Get the point within the scene that the user clicked.
  /// \return The 3D point.
  public: math::Vector3d Point() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event which is called to broadcast the 3D coordinates of a
/// user's releasing the right button within the scene.
/// \sa RightClickOnScene
class GZ_GUI_VISIBLE RightClickToScene : public QEvent
{
  /// \brief Constructor
  /// \param[in] _point The point which the user has right clicked
  /// within the scene
  public: explicit RightClickToScene(const math::Vector3d &_point);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 6);

  /// \brief Get the point within the scene that the user clicked.
  /// \return The 3D point.
  public: math::Vector3d Point() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event which is called to enable or disable the dropdown menu.
/// This is primarily used by plugins which also use the right click
/// mouse event to cancel any actions currently in progress.
class GZ_GUI_VISIBLE DropdownMenuEnabled : public QEvent
{
  /// \brief Constructor
  /// \param[in] _menuEnabled The boolean indicating whether the dropdown
  /// menu should be enabled or disabled.
  public: explicit DropdownMenuEnabled(bool _menuEnabled);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 7);

  /// \brief Gets whether the menu is enabled or not for this event.
  /// \return True if enabling the menu, false if disabling the menu
  public: bool MenuEnabled() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event which is called to broadcast the key release within
/// the scene.
class GZ_GUI_VISIBLE KeyReleaseOnScene : public QEvent
{
  /// \brief Constructor
  /// \param[in] _key The key released event within the scene
  public: explicit KeyReleaseOnScene(const common::KeyEvent &_key);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 8);

  /// \brief Get the released key within the scene that the user released.
  /// \return The key code.
  public: common::KeyEvent Key() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event which is called to broadcast the key press within
/// the scene.
class GZ_GUI_VISIBLE KeyPressOnScene : public QEvent
{
  /// \brief Constructor
  /// \param[in] _key The pressed key within the scene
  public: explicit KeyPressOnScene(const common::KeyEvent &_key);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 9);

  /// \brief Get the key within the scene that the user pressed
  /// \return The key code.
  public: common::KeyEvent Key() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event which is called to broadcast information about left
/// mouse releases on the scene.
/// For the 3D coordinates of that point on the scene, see
/// `LeftClickToScene`.
/// \sa LeftClickToScene
class GZ_GUI_VISIBLE LeftClickOnScene : public QEvent
{
  /// \brief Constructor
  /// \param[in] _mouse The left mouse event on the scene
  public: explicit LeftClickOnScene(
    const common::MouseEvent &_mouse);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 10);

  /// \brief Return the left mouse event
  public: const common::MouseEvent &Mouse() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event which is called to broadcast information about right
/// mouse releases on the scene.
/// For the 3D coordinates of that point on the scene, see
/// `RightClickToScene`.
/// \sa RightClickToScene
class GZ_GUI_VISIBLE RightClickOnScene : public QEvent
{
  /// \brief Constructor
  /// \param[in] _mouse The right mouse event on the scene
  public: RightClickOnScene(
    const common::MouseEvent &_mouse);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 11);

  /// \brief Return the right mouse event
  public: const common::MouseEvent &Mouse() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event that block the Interactive View control when some of the
/// other plugins require it. For example: When the transform control is
/// active we should block the movements of the camera.
class GZ_GUI_VISIBLE BlockOrbit : public QEvent
{
  /// \brief Constructor
  /// \param[in] _block True to block otherwise False
  public: explicit BlockOrbit(const bool &_block);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 12);

  /// \brief Get the if the event should block the Interactive view
  /// controller
  /// \return True to block otherwise False.
  public: bool Block() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event which is called to broadcast the 2D coordinates of a
/// user's mouse hover within the scene.
class GZ_GUI_VISIBLE HoverOnScene : public QEvent
{
  /// \brief Constructor
  /// \param[in] _mouse The hover mouse event on the scene
  public: explicit HoverOnScene(const common::MouseEvent &_mouse);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 13);

  /// \brief Get the point within the scene over which the user is
  /// hovering.
  /// \return The 2D point
  public: common::MouseEvent Mouse() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event called to clone a resource, given its name as a string.
class GZ_GUI_VISIBLE SpawnCloneFromName : public QEvent
{
  /// \brief Constructor
  /// \param[in] _name The name of the resource to clone
  public: explicit SpawnCloneFromName(const std::string &_name);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 14);

  /// \brief Get the name of the resource to be cloned
  /// \return The name of the resource to be cloned
  public: const std::string &Name() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event called to clone a resource, given its name as a string.
class GZ_GUI_VISIBLE DropOnScene : public QEvent
{
  /// \brief Constructor
  /// \param[in] _dropText Dropped string.
  /// \param[in] _dropMouse x and y  coordinate of mouse position.
  public: explicit DropOnScene(
    const std::string &_dropText,
    const math::Vector2i &_dropMouse);

  /// \brief Get the text of the dropped thing on the scene
  /// \return The name of the dropped thing on the scene
  public: const std::string &DropText() const;

  /// \brief Get X and Y position
  /// \return Get X and Y position
  public: const math::Vector2i &Mouse() const;

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 15);

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event which is called to broadcast information about mouse
/// scrolls on the scene.
class GZ_GUI_VISIBLE ScrollOnScene : public QEvent
{
  /// \brief Constructor
  /// \param[in] _mouse The scroll mouse event on the scene
  public: explicit ScrollOnScene(const common::MouseEvent &_mouse);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 16);

  /// \brief Return the scroll mouse event
  public: const common::MouseEvent &Mouse() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event which is called to broadcast information about mouse
/// drags on the scene.
class GZ_GUI_VISIBLE DragOnScene : public QEvent
{
  /// \brief Constructor
  /// \param[in] _mouse The drag mouse event on the scene
  public: explicit DragOnScene(const common::MouseEvent &_mouse);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 17);

  /// \brief Get the point within the scene over which the user is
  /// dragging.
  /// \return The 2D point
  public: common::MouseEvent Mouse() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event which is called to broadcast information about mouse
/// presses on the scene, with right, left or middle buttons.
class GZ_GUI_VISIBLE MousePressOnScene : public QEvent
{
  /// \brief Constructor
  /// \param[in] _mouse The mouse event on the scene
  public: MousePressOnScene(
    const common::MouseEvent &_mouse);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 18);

  /// \brief Return the button press mouse event
  public: const common::MouseEvent &Mouse() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event which is called to share WorldControl information.
class GZ_GUI_VISIBLE WorldControl : public QEvent
{
  /// \brief Constructor
  /// \param[in] _worldControl The WorldControl information
  public: explicit WorldControl(const msgs::WorldControl &_worldControl);

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 19);

  /// \brief Get the WorldControl information
  /// \return The WorldControl information
  public: const msgs::WorldControl &WorldControlInfo() const;

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};

/// \brief Event called in the render thread of a 3D scene, before the
/// user camera is rendered.
/// It's safe to make rendering calls in this event's callback.
class GZ_GUI_VISIBLE PreRender : public QEvent
{
  /// \brief Constructor
  public: PreRender();

  /// \brief Unique type for this event.
  static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 20);

  /// \internal
  /// \brief Private data pointer
  GZ_UTILS_IMPL_PTR(dataPtr)
};
}  // namespace gz::gui::events
#endif  // GZ_GUI_GUIEVENTS_HH_
