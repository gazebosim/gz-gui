gz_get_sources(tests)

gz_build_tests(TYPE REGRESSION 
               SOURCES ${tests}
               ENVIRONMENT GZ_GUI_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})
