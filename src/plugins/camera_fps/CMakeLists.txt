qt6_add_qml_module(CameraFps
  URI camera_fps
  VERSION 1.0
  SOURCES CameraFps.cc
  QML_FILES CameraFps.qml
)

target_link_libraries(CameraFps PRIVATE
   gz-rendering${GZ_RENDERING_VER}::gz-rendering${GZ_RENDERING_VER}
   gz-transport${GZ_TRANSPORT_VER}::gz-transport${GZ_TRANSPORT_VER}
   gz-plugin${GZ_PLUGIN_VER}::register
   Qt6::Gui
   Qt6::Widgets
   Qt6::Quick
   Qt6::QuickControls2
)

install (TARGETS CameraFps DESTINATION ${GZ_GUI_PLUGIN_INSTALL_DIR})
