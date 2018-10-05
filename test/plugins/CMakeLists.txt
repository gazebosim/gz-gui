if (WIN32)
  add_definitions(-DBUILDING_DLL)
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}
)

link_directories(
)

set (plugins
  TestBadInheritancePlugin
  TestNotRegisteredPlugin
  TestPlugin
)

# Plugin shared libraries
foreach (src ${plugins})
  QT5_WRAP_CPP(${src}_MOC ${src}.hh)
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}.qrc")
    QT5_ADD_RESOURCES(${src}_RCC ${src}.qrc)
  endif()
  add_library(${src} SHARED
    ${src}.cc
    ${${src}_MOC}
    ${${src}_RCC}
  )
  target_link_libraries(${src}
    PUBLIC
    ${IGNITION-COMMON_LIBRARIES}
    ${IGNITION-GUI_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Qml_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
    ${Qt5QuickControls2_LIBRARIES}
    PRIVATE
    ignition-plugin${IGN_PLUGIN_VER}::register
  )
endforeach (src ${qt_headers_local})

