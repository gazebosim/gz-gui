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

#ifndef IGNITION_GUI_PLUGINS_SCENE3D_HH_
#define IGNITION_GUI_PLUGINS_SCENE3D_HH_

#include <memory>
#include <ignition/math/Vector2.hh>
#include <ignition/math/Vector3.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class Scene3DPrivate;
  class RenderWindowItemPrivate;

  /// \brief Creates a new ignition rendering scene or adds a user-camera to an
  /// existing scene. It is possible to orbit the camera around the scene with
  /// the mouse. Use other plugins to manage objects in the scene.
  ///
  /// ## Configuration
  ///
  /// * \<engine\> : Optional render engine name, defaults to 'ogre'.
  /// * \<scene\> : Optional scene name, defaults to 'scene'. The plugin will
  ///               create a scene with this name if there isn't one yet. If
  ///               there is already one, a new camera is added to it.
  /// * \<ambient_light\> : Optional color for ambient light, defaults to
  ///                       (0.3, 0.3, 0.3, 1.0)
  /// * \<background_color\> : Optional background color, defaults to
  ///                          (0.3, 0.3, 0.3, 1.0)
  /// * \<camera_pose\> : Optional starting pose for the camera, defaults to
  ///                     (0, 0, 5, 0, 0, 0)
  class Scene3D : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: Scene3D();

    /// \brief Destructor
    public: virtual ~Scene3D();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
        override;

    /// \brief Retrieve the first point on a surface in the 3D scene hit by a
    /// ray cast from the given 2D screen coordinates.
    /// \param[in] _screenPos 2D coordinates on the screen, in pixels.
    /// \return 3D coordinates of a point in the 3D scene.
    public: math::Vector3d ScreenToScene(const math::Vector2i &_screenPos)
        const;

    /// \brief
    public: QQuickItem *Item() const override;
    private: QQuickItem *item;

    // Documentation inherited
//    protected: virtual QPaintEngine *paintEngine() const override;

    // Documentation inherited
//    protected: virtual void paintEvent(QPaintEvent *_e) override;
//
//    // Documentation inherited
//    protected: virtual void resizeEvent(QResizeEvent *_e) override;
//
//    // Documentation inherited
//    protected: virtual void mousePressEvent(QMouseEvent *_e) override;
//
//    // Documentation inherited
//    protected: virtual void mouseReleaseEvent(QMouseEvent *_e) override;
//
//    // Documentation inherited
//    protected: virtual void mouseMoveEvent(QMouseEvent *_e) override;
//
//    // Documentation inherited
//    protected: virtual void wheelEvent(QWheelEvent *_e) override;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<Scene3DPrivate> dataPtr;


    private: void InitializeEngine();
    private: void ActivateRenderWindowContext();
    private: void DoneRenderWindowContext();
  };

  class RenderWindowItem: public QQuickItem 
  {
    Q_OBJECT

    /// \brief Constructor
    public: explicit RenderWindowItem(QQuickItem *_parent = nullptr);

    public: virtual ~RenderWindowItem();

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<RenderWindowItemPrivate> dataPtr;


    private: void InitializeEngine();
    private: void ActivateRenderWindowContext();
    private: void DoneRenderWindowContext();
    private: void UpdateFBO();
    private: QSGNode *updatePaintNode(QSGNode *_oldNode,
        QQuickItem::UpdatePaintNodeData *);

    private: void timerEvent(QTimerEvent *);
  };

}
}
}

#endif
