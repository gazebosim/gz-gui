set (qt_headers
  TestHelper.hh
)

qt_wrap_cpp(headers_MOC ${qt_headers})

add_library(${PROJECT_NAME}_test_helpers SHARED
  TestHelper.cc
  ${headers_MOC}
)
target_compile_definitions(${PROJECT_NAME}_test_helpers PRIVATE TestHelper_EXPORTS)
target_link_libraries(${PROJECT_NAME}_test_helpers
  PUBLIC
    ${PROJECT_LIBRARY_TARGET_NAME}
)
