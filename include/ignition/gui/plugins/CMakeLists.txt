set (qt_headers_local
  Grid3D
#  ImageDisplay
  Publisher
#  Requester
#  Responder
  Scene3D
  TopicEcho
#  TopicInterface
#  TopicsStats
#  TopicViewer
  WorldControl
  WorldStats
)

set(IGNITION_GUI_PLUGIN_INSTALL_DIR
  ${CMAKE_INSTALL_PREFIX}/${IGN_LIB_INSTALL_DIR}/ign-${IGN_DESIGNATION}-${PROJECT_VERSION_MAJOR}/plugins
)

# Plugin shared libraries
foreach (src ${qt_headers_local})
  QT5_WRAP_CPP(${src}_MOC ${src}.hh)
  QT5_ADD_RESOURCES(${src}_RCC ${src}.qrc)
  add_library(${src} SHARED
    ${PROJECT_SOURCE_DIR}/src/plugins/${src}.cc
    ${${src}_MOC}
    ${${src}_RCC}
  )
  target_link_libraries(${src}
    PUBLIC
      ${PROJECT_LIBRARY_TARGET_NAME}
      ${IGNITION-RENDERING_LIBRARIES}
    PRIVATE
      ignition-plugin${IGN_PLUGIN_VER}::register
  )
  install (TARGETS ${src} DESTINATION ${IGNITION_GUI_PLUGIN_INSTALL_DIR})
endforeach (src ${qt_headers_local})

#add_subdirectory(plot)
