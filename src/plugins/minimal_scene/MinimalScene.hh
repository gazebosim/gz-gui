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

#ifndef GZ_GUI_PLUGINS_MINIMALSCENE_HH_
#define GZ_GUI_PLUGINS_MINIMALSCENE_HH_

#include <string>
#include <memory>

#include <gz/common/KeyEvent.hh>
#include <gz/common/MouseEvent.hh>
#include <gz/math/Color.hh>
#include <gz/math/Pose3.hh>
#include <gz/math/Vector2.hh>
#include <gz/utils/ImplPtr.hh>
#include <gz/rendering/GraphicsAPI.hh>

#include "gz/gui/Plugin.hh"

#include "MinimalSceneRhi.hh"

namespace gz::gui::plugins
{
/// \brief Creates a Gazebo rendering scene and user camera.
/// It is possible to orbit the camera around the scene with
/// the mouse. Use other plugins to manage objects in the scene.
///
/// Only one plugin displaying a Gazebo Rendering scene can be used at a
/// time.
///
/// ## Configuration
///
/// * \<engine\> : Optional render engine name, defaults to 'ogre'. If another
///                engine is already loaded, that will be used, because only
///                one engine is supported at a time currently.
/// * \<scene\> : Optional scene name, defaults to 'scene'. The plugin will
///               create a scene with this name if there isn't one yet. If
///               there is already one, a new camera is added to it.
/// * \<ambient_light\> : Optional color for ambient light, defaults to
///                       (0.3, 0.3, 0.3, 1.0)
/// * \<background_color\> : Optional background color, defaults to
///                          (0.3, 0.3, 0.3, 1.0)
/// * \<camera_pose\> : Optional starting pose for the camera, defaults to
///                     (0, 0, 5, 0, 0, 0)
/// * \<camera_clip\> : Optional near/far clipping distance for camera
///     * \<near\> : Camera's near clipping plane distance, defaults to 0.01
///     * \<far\> : Camera's far clipping plane distance, defaults to 1000.0
/// * \<sky\> : If present, sky is enabled.
/// * \<horizontal_fov\> : Horizontal FOV of the user camera in degrees,
///                        defaults to 90
/// * \<graphics_api\> : Optional graphics API name. Valid choices are:
///                      'opengl', 'metal'. Defaults to 'opengl'.
/// * \<view_controller> : Set the view controller (InteractiveViewControl
///                        currently supports types: ortho or orbit).
class MinimalScene : public Plugin
{
  Q_OBJECT

  /// \brief Loading error message
  Q_PROPERTY(
    QString loadingError
    READ LoadingError
    WRITE SetLoadingError
    NOTIFY LoadingErrorChanged
  )

  /// \brief Constructor
  public: MinimalScene();

  /// \brief Callback when the mouse hovers to a new position.
  /// \param[in] _mouseX x coordinate of the hovered mouse position.
  /// \param[in] _mouseY y coordinate of the hovered mouse position.
  public slots: void OnHovered(int _mouseX, int _mouseY);

  /// \brief Callback when the mouse enters the render window to
  /// focus the window for mouse/key events
  public slots: void OnFocusWindow();

  /// \brief Callback when receives a drop event.
  /// \param[in] _drop Dropped string.
  /// \param[in] _mouseX x coordinate of mouse position.
  /// \param[in] _mouseY y coordinate of mouse position.
  public slots: void OnDropped(const QString &_drop,
      int _mouseX, int _mouseY);

  // Documentation inherited
  public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
      override;

  /// \brief Get the loading error string.
  /// \return String explaining the loading error. If empty, there's no error.
  public: Q_INVOKABLE QString LoadingError() const;

  /// \brief Set the loading error message.
  /// \param[in] _loadingError Error message.
  public: Q_INVOKABLE void SetLoadingError(const QString &_loadingError);

  /// \brief Notify that loading error has changed
  signals: void LoadingErrorChanged();

  /// \brief Loading error message
  public: QString loadingError;

  /// \internal
  /// \brief Pointer to private data.
  GZ_UTILS_UNIQUE_IMPL_PTR(dataPtr)
};

class RenderSync;

/// \brief gz-rendering renderer.
/// All gz-rendering calls should be performed inside this class as it makes
/// sure that opengl calls in the underlying render engine do not interfere
/// with QtQuick's opengl render operations. The main Render function will
/// render to an offscreen texture and notify via signal and slots when it's
/// ready to be displayed.
class GzRenderer
{
  ///  \brief Constructor
  public: GzRenderer();

  /// \param[in] _renderSync RenderSync to safely
  /// synchronize Qt and worker thread (this)
  /// \param[in] _renderThreadRhi Our caller
  public: void Render(RenderSync *_renderSync,
                      RenderThreadRhi &_renderThreadRhi);

  /// \brief Initialize the render engine and scene.
  /// On macOS this must be called on the main thread.
  /// \param[in] _rhi our caller
  /// \return Error message if initialization failed. If empty, no errors
  /// occurred.
  public: std::string Initialize(RenderThreadRhi &_rhi);

  /// \brief Set the graphics API
  /// \param[in] _graphicsAPI The type of graphics API
  public: void SetGraphicsAPI(const rendering::GraphicsAPI &_graphicsAPI);

  /// \brief Destroy camera associated with this renderer
  public: void Destroy();

  /// \brief New mouse event triggered
  /// \param[in] _e New mouse event
  public: void NewMouseEvent(const common::MouseEvent &_e);

  /// \brief New hover event triggered.
  /// \param[in] _hoverPos Mouse hover screen position
  public: void NewHoverEvent(const math::Vector2i &_hoverPos);

  /// \brief New hover event triggered.
  /// \param[in] _dropText Text dropped on the scene
  /// \param[in] _dropPos Mouse drop screen position
  public: void NewDropEvent(const std::string &_dropText,
    const math::Vector2i &_dropPos);

  /// \brief Handle key press event for snapping
  /// \param[in] _e The key event to process.
  public: void HandleKeyPress(const common::KeyEvent &_e);

  /// \brief Handle key release event for snapping
  /// \param[in] _e The key event to process.
  public: void HandleKeyRelease(const common::KeyEvent &_e);

  /// \brief Handle mouse event for view control
  private: void HandleMouseEvent();

  /// \brief Broadcasts the currently hovered 3d scene location.
  private: void BroadcastHoverPos();

  /// \brief Broadcasts drag events.
  private: void BroadcastDrag();

  /// \brief Broadcasts a left click (release) within the scene
  private: void BroadcastLeftClick();

  /// \brief Broadcasts a right click (release) within the scene
  private: void BroadcastRightClick();

  /// \brief Broadcasts a mouse press within the scene
  private: void BroadcastMousePress();

  /// \brief Broadcasts a scroll event within the scene.
  private: void BroadcastScroll();

  /// \brief Broadcasts a key release event within the scene
  private: void BroadcastKeyRelease();

  /// \brief Broadcasts a drop event within the scene
  private: void BroadcastDrop();

  /// \brief Broadcasts a key press event within the scene
  private: void BroadcastKeyPress();

  /// \brief Render engine to use
  public: std::string engineName = "ogre";

  /// \brief Unique scene name
  public: std::string sceneName = "scene";

  /// \brief Initial Camera pose
  public: math::Pose3d cameraPose = math::Pose3d(0, 0, 2, 0, 0.4, 0);

  /// \brief Default camera near clipping plane distance
  public: double cameraNearClip = 0.01;

  /// \brief Default camera far clipping plane distance
  public: double cameraFarClip = 1000.0;

  /// \brief Scene background color
  public: math::Color backgroundColor = math::Color::Black;

  /// \brief Ambient color
  public: math::Color ambientLight = math::Color(0.3f, 0.3f, 0.3f, 1.0f);

  /// \brief True if engine has been initialized;
  public: bool initialized = false;

  /// \brief Render texture size
  public: QSize textureSize = QSize(1024, 1024);

  /// \brief Flag to indicate texture size has changed.
  public: bool textureDirty = true;

  /// \brief True if sky is enabled;
  public: bool skyEnable = false;

  /// \brief Horizontal FOV of the camera;
  public: math::Angle cameraHFOV = math::Angle(M_PI * 0.5);

  /// \brief View controller type
  public: std::string cameraViewController{""};

  /// \brief Retrieves the internal camera.
  /// TODO(darksylinc): Remove this hack.
  public: rendering::CameraPtr Camera();

  /// \internal
  /// \brief Pointer to private data.
  GZ_UTILS_UNIQUE_IMPL_PTR(dataPtr)
};

/// \brief Rendering thread
class RenderThread : public QThread
{
  Q_OBJECT

  /// \brief Constructor
  public: RenderThread();

  /// \brief Render when safe
  /// \param[in] _renderSync RenderSync to safely
  /// synchronize Qt and worker thread (this)
  public slots: void RenderNext(RenderSync *_renderSync);

  /// \brief Shutdown the thread and the render engine
  public slots: void ShutDown();

  /// \brief Slot called to update render texture size
  public slots: void SizeChanged();

  /// \brief Signal to indicate that a frame has been rendered and ready
  /// to be displayed
  /// \param[in] _texturePtr Pointer to a texture Id
  /// \param[in] _size Size of the texture
  signals: void TextureReady(void* _texturePtr, const QSize &_size);

  /// \brief Set a callback to be called in case there are errors.
  /// \param[in] _cb Error callback
  public: void SetErrorCb(std::function<void(const QString &)> _cb);

  /// \brief Function to be called if there are errors.
  public: std::function<void(const QString &)> errorCb;

  /// \brief Offscreen surface to render to
  public: QOffscreenSurface *Surface() const;

  /// \brief Set the offscreen surface to render to
  /// \param[in] _surface Off-screen surface format
  public: void SetSurface(QOffscreenSurface *_surface);

  /// \brief OpenGL context to be passed to the render engine
  public: QOpenGLContext *Context() const;

  /// \brief Set the OpenGL context to be passed to the render engine
  /// \param[in] _context OpenGL context
  public: void SetContext(QOpenGLContext *_context);

  /// \brief Set the graphics API
  /// \param[in] _graphicsAPI The type of graphics API
  public: void SetGraphicsAPI(const rendering::GraphicsAPI &_graphicsAPI);

  /// \brief Carry out initialisation.
  /// On macOS this must be run on the main thread
  public: std::string Initialize();

  /// \brief gz-rendering renderer
  public: GzRenderer gzRenderer;

  /// \brief Pointer to render interface to handle OpenGL/Metal compatibility
  private: std::unique_ptr<RenderThreadRhi> rhi;
};

/// \brief A QQUickItem that manages the render window
class RenderWindowItem : public QQuickItem
{
  Q_OBJECT

  /// \brief Constructor
  /// \param[in] _parent Parent item
  public: explicit RenderWindowItem(QQuickItem *_parent = nullptr);

  public: ~RenderWindowItem();

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
  /// \param[in] _pose Initial camera pose
  public: void SetCameraPose(const math::Pose3d &_pose);

  /// \brief Set the render window camera's near clipping plane distance
  /// \param[in] _near Near clipping plane distance
  public: void SetCameraNearClip(double _near);

  /// \brief Set the render window camera's far clipping plane distance
  /// \param[in] _far Far clipping plane distance
  public: void SetCameraFarClip(double _far);

  /// \brief Called when the mouse hovers to a new position.
  /// \param[in] _hoverPos 2D coordinates of the hovered mouse position on
  /// the render window.
  public: void OnHovered(const gz::math::Vector2i &_hoverPos);

  /// \brief Callback when receives a drop event.
  /// \param[in] _drop Dropped string.
  /// \param[in] _dropPos x coordinate of mouse position.
  public: void OnDropped(const QString &_drop,
      const gz::math::Vector2i &_dropPos);

  /// \brief Set if sky is enabled
  /// \param[in] _sky True to enable the sky, false otherwise.
  public: void SetSkyEnabled(const bool &_sky);

  /// \brief Set the Horizontal FOV of the camera
  /// \param[in] _fov FOV of the camera in degree
  public: void SetCameraHFOV(const math::Angle &_fov);

  /// \brief Set the graphics API
  /// \param[in] _graphicsAPI The type of graphics API
  public: void SetGraphicsAPI(const rendering::GraphicsAPI& _graphicsAPI);

  /// \brief Set the camera view controller
  /// \param[in] _view_controller The camera view controller type to set
  public: void SetCameraViewController(const std::string &_view_controller);

  /// \brief Slot called when thread is ready to be started
  public Q_SLOTS: void Ready();

  /// \brief Handle key press event for snapping
  /// \param[in] _e The key event to process.
  public: void HandleKeyPress(const common::KeyEvent &_e);

  /// \brief Handle key release event for snapping
  /// \param[in] _e The key event to process.
  public: void HandleKeyRelease(const common::KeyEvent &_e);

  /// \brief Set a callback to be called in case there are errors.
  /// \param[in] _cb Error callback
  public: void SetErrorCb(std::function<void(const QString &)> _cb);

  /// \brief Stop rendering and shutdown resources.
  public: void StopRendering();

  // Documentation inherited
  protected: virtual void mousePressEvent(QMouseEvent *_e) override;

  // Documentation inherited
  protected: virtual void mouseReleaseEvent(QMouseEvent *_e) override;

  // Documentation inherited
  protected: virtual void mouseMoveEvent(QMouseEvent *_e) override;

  // Documentation inherited
  protected: virtual void keyPressEvent(QKeyEvent *_e) override;

  // Documentation inherited
  protected: virtual void keyReleaseEvent(QKeyEvent *_e) override;

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
  GZ_UTILS_UNIQUE_IMPL_PTR(dataPtr)
};

/// \brief Texture node for displaying the render texture from gz-renderer
class TextureNode : public QObject, public QSGSimpleTextureNode
{
  Q_OBJECT

  /// \brief Constructor
  /// \param[in] _window Window to display the texture
  /// \param[in] _renderSync RenderSync to safely
  /// synchronize Qt (this) and worker thread
  /// \param[in] _graphicsAPI The type of graphics API
  /// \param[in] _camera Camera owning the Texture Handle
  public: explicit TextureNode(QQuickWindow *_window,
                               RenderSync &_renderSync,
                               const rendering::GraphicsAPI &_graphicsAPI,
                               rendering::CameraPtr &_camera);

  /// \brief Destructor
  public: ~TextureNode() override;

  /// \brief This function gets called on the FBO rendering thread and will
  ///  store the texture id and size and schedule an update on the window.
  /// \param[in] _texturePtr Pointer to a texture Id
  /// \param[in] _size Texture size
  // public slots: void NewTexture(uint _id, const QSize &_size);
  public slots: void NewTexture(void* _texturePtr, const QSize &_size);

  /// \brief Before the scene graph starts to render, we update to the
  /// pending texture
  public slots: void PrepareNode();

  /// \param[in] _renderSync RenderSync to send to the worker thread
  signals: void TextureInUse(RenderSync *_renderSync);

  /// \brief Signal emitted when a new texture is ready to trigger window
  /// update
  signals: void PendingNewTexture();

  /// \brief Texture size
  public: QSize size = QSize(0, 0);

  /// \brief Mutex to protect the texture variables
  public: QMutex mutex;

  /// \brief See RenderSync
  public: RenderSync &renderSync;

  /// \brief Qt quick window
  public: QQuickWindow *window = nullptr;

  /// \brief Pointer to render interface to handle OpenGL/Metal compatibility
  private: std::unique_ptr<TextureNodeRhi> rhi;
};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_MINIMALSCENE_HH_
