qt6_add_qml_module(CameraTracking
  URI camera_tracking
  VERSION 1.0
  SOURCES CameraTracking.cc
  QML_FILES CameraTracking.qml
)

target_link_libraries(CameraTracking PRIVATE
   gz-rendering${GZ_RENDERING_VER}::gz-rendering${GZ_RENDERING_VER}
   gz-transport${GZ_TRANSPORT_VER}::gz-transport${GZ_TRANSPORT_VER}
   gz-plugin${GZ_PLUGIN_VER}::register
   Qt6::Gui
   Qt6::Widgets
   Qt6::Quick
   Qt6::QuickControls2
)

install(TARGETS CameraTracking DESTINATION ${GZ_GUI_PLUGIN_INSTALL_DIR})
