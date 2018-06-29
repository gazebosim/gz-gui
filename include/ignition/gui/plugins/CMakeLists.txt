set (qt_headers_local
  Displays
  Grid3D
  ImageDisplay
  Publisher
  Requester
  Responder
  Scene3D
  TimePanel
  TopicEcho
  TopicInterface
  TopicsStats
  TopicViewer
)

set(IGNITION_GUI_PLUGIN_INSTALL_DIR
  ${CMAKE_INSTALL_PREFIX}/${IGN_LIB_INSTALL_DIR}/ign-${IGN_DESIGNATION}-${PROJECT_VERSION_MAJOR}/plugins
)

# Plugin shared libraries
foreach (src ${qt_headers_local})
  QT5_WRAP_CPP(${src}_MOC ${src}.hh)
  add_library(${src} SHARED ${PROJECT_SOURCE_DIR}/src/plugins/${src}.cc ${${src}_MOC})
  target_link_libraries(${src}
    ${IGNITION-COMMON_LIBRARIES}
    ${IGNITION-GUI_LIBRARIES}
    ${IGNITION-MSGS_LIBRARIES}
    ${IGNITION-RENDERING_LIBRARIES}
    ${IGNITION-TRANSPORT_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES})
endforeach (src ${qt_headers_local})

foreach (src ${qt_headers_local})
  install (TARGETS ${src} DESTINATION ${IGNITION_GUI_PLUGIN_INSTALL_DIR})
endforeach (src ${qt_headers_local})

add_subdirectory(displays)
add_subdirectory(plot)
