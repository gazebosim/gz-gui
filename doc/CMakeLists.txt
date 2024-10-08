find_package(Doxygen)

set(GZ_DOXYGEN_TAGFILES
  "\"${CMAKE_SOURCE_DIR}/doc/qt.tag.xml=http://doc.qt.io/qt-5/\" \
   \"${GZ-MATH_DOXYGEN_TAGFILE} = ${GZ-MATH_API_URL}\" \
   \"${GZ-MSGS_DOXYGEN_TAGFILE} = ${GZ-MSGS_API_URL}\" \
   \"${GZ-TRANSPORT_DOXYGEN_TAGFILE} = ${GZ-TRANSPORT_API_URL}\" \
   \"${GZ-COMMON_DOXYGEN_TAGFILE} = ${GZ-COMMON_API_URL}\""
)

if (DOXYGEN_FOUND)
  configure_file(${GZ_CMAKE_DOXYGEN_DIR}/api.in
                 ${CMAKE_BINARY_DIR}/api.dox @ONLY)

  configure_file(${GZ_CMAKE_DOXYGEN_DIR}/tutorials.in
                 ${CMAKE_BINARY_DIR}/tutorials.dox @ONLY)

  add_custom_target(doc ALL
    # Generate the API documentation
    ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/api.dox
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}

    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/tutorials.dox

    COMMENT "Generating API documentation with Doxygen" VERBATIM)

  install(FILES ${CMAKE_BINARY_DIR}/doc/${PROJECT_NAME_LOWER}.tag.xml 
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/gz/${PROJECT_NAME_LOWER}_${PROJECT_VERSION_MINOR})
endif()
