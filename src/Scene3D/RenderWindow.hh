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
#ifndef IGNITION_GUI_RENDERWINDOW_HH_
#define IGNITION_GUI_RENDERWINDOW_HH_

#include <QThread>

#include <ignition/common/MouseEvent.hh>

#include <ignition/math/Color.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/math/Vector2.hh>
#include <ignition/math/Vector3.hh>

#include <memory>
#include <string>

#include <QQuickItem>
#include "ignition/gui/qt.h"
#include "IgnRenderer.hh"

namespace ignition
{
namespace gui
{
  class RenderWindowItemPrivate;

  class RenderThread : public QThread
  {
    Q_OBJECT

    /// \brief Constructor
    public: RenderThread();

    /// \brief Render the next frame
    public slots: void RenderNext();

    /// \brief Shutdown the thread and the render engine
    public slots: void ShutDown();

    /// \brief Slot called to update render texture size
    public slots: void SizeChanged();

    /// \brief Signal to indicate that a frame has been rendered and ready
    /// to be displayed
    /// \param[in] _id GLuid of the opengl texture
    /// \param[in] _size Size of the texture
    signals: void TextureReady(int _id, const QSize &_size);

    /// \brief Offscreen surface to render to
    public: QOffscreenSurface *surface = nullptr;

    /// \brief OpenGL context to be passed to the render engine
    public: QOpenGLContext *context = nullptr;

    /// \brief Ign-rendering renderer
    public: IgnRenderer ignRenderer;
  };

  /// \brief A QQUickItem that manages the render window
  class RenderWindowItem : public QQuickItem
  {
    Q_OBJECT

    /// \brief Constructor
    /// \param[in] _parent Parent item
    public: explicit RenderWindowItem(QQuickItem *_parent = nullptr);

    /// \brief Destructor
    public: virtual ~RenderWindowItem();

    /// \brief Set background color of render window
    /// \param[in] _color Color of render window background
    public: void SetBackgroundColor(const math::Color &_color);

    /// \brief Set ambient light of render window
    /// \param[in] _ambient Color of ambient light
    public: void SetAmbientLight(const math::Color &_ambient);

    /// \brief Set engine name used to create the render window
    /// \param[in] _name Name of render engine
    public: void SetEngineName(const std::string &_name);

    /// \brief Set name of scene created inside the render window
    /// \param[in] _name Name of scene
    public: void SetSceneName(const std::string &_name);

    /// \brief Set if sky is enabled
    /// \param[in] _sky True if enable, false otherwise
    public: void SetSkyEnabled(const bool &_sky);

    /// \brief
    /// \param[in] _gain
    public: void SetFollowPGain(const double &_gain);

    void SetFollowTarget(const std::string &_target,
        bool _waitForTarget);

    /// \brief True to set the camera to follow the target in world frame,
    /// false to follow in target's local frame
    /// \param[in] _gain Camera follow p gain.
    public: void SetFollowWorldFrame(bool _worldFrame);

    /// \brief Set the camera follow offset position
    /// \param[in] _offset Camera follow offset position.
    public: void SetFollowOffset(const math::Vector3d &_offset);

    /// \brief Move the user camera to move to the specified target
    /// \param[in] _target Target to move the camera to
    public: void SetMoveTo(const std::string &_target);

    /// \brief Set the user camera visibility mask
    /// \param[in] _mask Visibility mask to set to
    public: void SetVisibilityMask(uint32_t _mask);

    /// \brief Set the initial pose the render window camera
    /// \param[in] _pose Initical camera pose
    public: void SetCameraPose(const math::Pose3d &_pose);

    /// \brief Set scene service to use in this render window
    /// A service call will be made using ign-transport to get scene
    /// data using this service
    /// \param[in] _service Scene service name
    public: void SetSceneService(const std::string &_service);

    /// \brief Set pose topic to use for updating objects in the scene
    /// The renderer will subscribe to this topic to get pose messages of
    /// visuals in the scene
    /// \param[in] _topic Pose topic
    public: void SetPoseTopic(const std::string &_topic);

    /// \brief Set deletion topic to use for deleting objects from the scene
    /// The renderer will subscribe to this topic to get notified when entities
    /// in the scene get deleted
    /// \param[in] _topic Deletion topic
    public: void SetDeletionTopic(const std::string &_topic);

    /// \brief Set the scene topic to use for updating objects in the scene
    /// The renderer will subscribe to this topic to get updates scene messages
    /// \param[in] _topic Scene topic
    public: void SetSceneTopic(const std::string &_topic);

    /// \brief Slot called when thread is ready to be started
    public Q_SLOTS: void Ready();

    // Documentation inherited
    protected: virtual void mousePressEvent(QMouseEvent *_e) override;

    // Documentation inherited
    protected: virtual void mouseReleaseEvent(QMouseEvent *_e) override;

    // Documentation inherited
    protected: virtual void mouseMoveEvent(QMouseEvent *_e) override;

    // Documentation inherited
    protected: virtual void wheelEvent(QWheelEvent *_e) override;

    /// \brief Overrides the paint event to render the render engine
    /// camera view
    /// \param[in] _oldNode The node passed in previous updatePaintNode
    /// function. It represents the visual representation of the item.
    /// \param[in] _data The node transformation data.
    /// \return Updated node.
    private: QSGNode *updatePaintNode(QSGNode *_oldNode,
        QQuickItem::UpdatePaintNodeData *_data) override;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<RenderWindowItemPrivate> dataPtr;
  };
}
}

#endif
