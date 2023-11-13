if (WIN32)
  add_definitions(-DBUILDING_DLL)
endif()

set (plugins
  TestBadInheritancePlugin
  TestInvalidQmlPlugin
  TestNotRegisteredPlugin
  TestPlugin
)

# Plugin shared libraries
foreach (src ${plugins})
  qt_wrap_cpp(${src}_MOC ${src}.hh)
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}.qrc")
    qt_add_resources(${src}_RCC ${src}.qrc)
  endif()
  add_library(${src} SHARED
    ${src}.cc
    ${${src}_MOC}
    ${${src}_RCC}
  )
  target_link_libraries(${src}
    PUBLIC
    ${PROJECT_LIBRARY_TARGET_NAME}
    PRIVATE
    gz-plugin${GZ_PLUGIN_VER}::register
  )
endforeach (src ${qt_headers_local})
