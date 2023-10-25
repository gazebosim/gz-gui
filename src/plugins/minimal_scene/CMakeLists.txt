set(SOURCES
  MinimalScene.cc
  MinimalScene.hh
  MinimalSceneRhi.cc
  MinimalSceneRhi.hh
  MinimalSceneRhiOpenGL.cc
  MinimalSceneRhiOpenGL.hh
  MinimalSceneRhiVulkan.cc
  MinimalSceneRhiVulkan.hh
  EngineToQtInterface.cc
  EngineToQtInterface.hh
)

qt6_add_qml_module(MinimalScene
  URI MinimalScene
  VERSION 1.0
  SOURCES ${SOURCES}
  QML_FILES MinimalScene.qml
)

target_link_libraries(MinimalScene PRIVATE
   gz-rendering${GZ_RENDERING_VER}::gz-rendering${GZ_RENDERING_VER}
   gz-transport${GZ_TRANSPORT_VER}::gz-transport${GZ_TRANSPORT_VER}
   gz-plugin${GZ_PLUGIN_VER}::register
   Qt6::Gui
   Qt6::Widgets
   Qt6::Quick
   Qt6::QuickControls2
)

install (TARGETS MinimalScene DESTINATION ${GZ_GUI_PLUGIN_INSTALL_DIR})
