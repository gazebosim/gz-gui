if (WIN32)
  add_definitions(-DBUILDING_DLL)
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}
)

link_directories(
)

set (plugins
  TestDisplayPlugin
)

# Display plugin shared libraries
foreach (src ${plugins})
  QT5_WRAP_CPP(${src}_MOC ${src}.hh)
  add_library(${src} SHARED ${src}.cc ${${src}_MOC})
  target_link_libraries(${src}
    ${PROJECT_LIBRARY_TARGET_NAME}
    ${IGNITION-RENDERING_LIBRARIES})
endforeach (src ${qt_headers_local})

