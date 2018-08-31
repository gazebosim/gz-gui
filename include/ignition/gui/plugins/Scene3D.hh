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

#include <string>
#include <memory>

#include <ignition/math/Color.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/math/Vector2.hh>
#include <ignition/math/Vector3.hh>

#include <ignition/rendering/Camera.hh>

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
  class RenderWindowNodePrivate;

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
    // piublic: math::Vector3d ScreenToScene(const math::Vector2i &_screenPos)
    //    const;

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
  };

  /// \brief A QQUickItem that manages the render window
  class RenderWindowItem: public QQuickFramebufferObject
//  class RenderWindowItem : public QQuickItem
  {
    Q_OBJECT

/*    public: class Renderer
    {
      public: Renderer(){}
      public: virtual ~Renderer(){}
      public: virtual void synchronize(RenderWindowItem *_item);
      public: virtual void Initialize();
//      public: virtual void update();
      public: virtual void render();
      public: virtual QOpenGLFramebufferObject *createFramebufferObject(const QSize &_isze);

      private: QQuickWindow *window = nullptr;
      public: RenderWindowItem *item = nullptr;
      private: std::string sceneName{"scene"};
      private: std::string engineName{"ogre2"};
      private: rendering::CameraPtr camera;
      private: math::Pose3d cameraPose = math::Pose3d(0, 0, 5, 0, 0, 0);
      private: bool initialized = false;
      public: GLuint textureId = 0u;
      public: QSize textureSize = QSize(800, 600);
      private: QOffscreenSurface *surface = nullptr;
      private: QOpenGLFramebufferObject *fbo = nullptr;
      private: rendering::ImagePtr img;
      private: QOpenGLContext *ctx = nullptr;
    };
*/

    /// \brief Constructor
    /// \param[in] _parent Parent item
    public: explicit RenderWindowItem(QQuickItem *_parent = nullptr);

    /// \brief Destructor
    public: virtual ~RenderWindowItem();

//    public: RenderWindowItem::Renderer *createRenderer() const;
    public: QQuickFramebufferObject::Renderer *createRenderer() const;

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

    /// \brief Set the initial pose the render window camera
    /// \param[in] _pose Initical camera pose
    public: void SetCameraPose(const math::Pose3d &_pose);

    /// \brief Initialize the render engine
    private: void InitializeEngine();

    /// \brief Activate the render window OpenGL context
    // public: void ActivateRenderWindowContext();

    /// \brief Deactivate the render window OpenGL context
    // public: void DoneRenderWindowContext();

    /// \brief Get the render window context
    public: QOpenGLContext *RenderWindowContext() const;

    /// \brief Get the qt context
    public: QOpenGLContext *QtContext() const;

    public: void SetImage(QSize _size, unsigned char *_data);


    /// \brief Overrides the paint event to render the render engine
    /// camera view
    /// \param[in] _oldNode The node passed in previous updatePaintNode
    /// function. It represents the visual representation of the item.
    /// \param[in] _data The node transformation data.
    private: QSGNode *updatePaintNode(QSGNode *_node,
        QQuickItem::UpdatePaintNodeData *_data);
/*
    /// \brief Timer callabck. This queues a call to update the item.
    /// \param[in] _event A Qt timer event.
    private: void timerEvent(QTimerEvent *_event);
*/

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<RenderWindowItemPrivate> dataPtr;
  };
/*

  /// \brief A QQUickItem that manages the render window
  class RenderWindowNode: public QSGGeometryNode
  {
    public: RenderWindowNode();
    public: ~RenderWindowNode();
    public: void SetSize(QSize _size);
    public: void SetRenderWindowItem(RenderWindowItem *_item);
    public: void SetCamera(rendering::CameraPtr _camera);

    public: void ActivateContext();
    public: void DoneContext();

    public: void preprocess();

    /// \brief Update the GL render texture
    private: void UpdateFBO();

    public: void update();

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<RenderWindowNodePrivate> dataPtr;

  };
*/


/*
class FboNode : public QSGTextureProvider, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    FboNode();
    ~FboNode();
    void scheduleRender();
    QSGTexture *texture() const override;

public Q_SLOTS:
    void render();
    void handleScreenChange()
    {
        if (window->effectiveDevicePixelRatio() != devicePixelRatio) {
            renderer->invalidateFramebufferObject();
            quickFbo->update();
        }
    }

public:
    QQuickWindow *window;
    QOpenGLFramebufferObject *fbo;
//    QQuickFramebufferObject::Renderer *renderer;
    RenderWindowItem::Renderer *renderer;
//    QQuickFramebufferObject *quickFbo;

    bool renderPending;
    bool invalidatePending;

    qreal devicePixelRatio;
};
*/

}
}
}

#endif
