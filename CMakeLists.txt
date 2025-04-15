cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(gz-gui VERSION 10.0.0)

#============================================================================
# Find gz-cmake
#============================================================================
find_package(gz-cmake4 REQUIRED)

#============================================================================
# Configure the project
#============================================================================
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
gz_configure_project(VERSION_SUFFIX pre1)

#============================================================================
# Set project-specific options
#============================================================================


#============================================================================
# Search for project-specific dependencies
#============================================================================
message(STATUS "\n\n-- ====== Finding Dependencies ======")

#--------------------------------------
# Find Protobuf
gz_find_package(GzProtobuf
                REQUIRED
                PRETTY Protobuf)

#--------------------------------------
# Find Tinyxml2
gz_find_package(TINYXML2 REQUIRED PRIVATE PRETTY tinyxml2)

#--------------------------------------
# Find gz-utils
gz_find_package(gz-utils3 REQUIRED)
set(GZ_UTILS_VER ${gz-utils3_VERSION_MAJOR})

#--------------------------------------
# Find gz-math
gz_find_package(gz-math8 REQUIRED)
set(GZ_MATH_VER ${gz-math8_VERSION_MAJOR})

#--------------------------------------
# Find gz-common
gz_find_package(gz-common6 REQUIRED COMPONENTS profiler)
set(GZ_COMMON_VER ${gz-common6_VERSION_MAJOR})

#--------------------------------------
# Find gz-plugin
gz_find_package(gz-plugin3 REQUIRED COMPONENTS loader register)
set(GZ_PLUGIN_VER ${gz-plugin3_VERSION_MAJOR})

#--------------------------------------
# Find gz-transport
gz_find_package(gz-transport REQUIRED)

#--------------------------------------
# Find gz-rendering
gz_find_package(gz-rendering9 REQUIRED)
set(GZ_RENDERING_VER ${gz-rendering9_VERSION_MAJOR})

#--------------------------------------
# Find gz-msgs
gz_find_package(gz-msgs11 REQUIRED)
set(GZ_MSGS_VER ${gz-msgs11_VERSION_MAJOR})

# Find if command is available. This is used to enable tests.
# Note that CLI files are installed regardless of whether the dependency is
# available during build time
find_program(HAVE_GZ_TOOLS gz)
set(GZ_TOOLS_VER 2)

#--------------------------------------
# Find QT
set(QT_MAJOR_VERSION 5)
set(QT_MINOR_VERSION 15)
gz_find_package (Qt${QT_MAJOR_VERSION}
  VERSION ${QT_MAJOR_VERSION}.${QT_MINOR_VERSION}
  COMPONENTS
    Core
    Quick
    QuickControls2
    Widgets
    Test
  REQUIRED
  PKGCONFIG_VER_COMPARISON >=
  PKGCONFIG "Qt${QT_MAJOR_VERSION}Core Qt${QT_MAJOR_VERSION}Quick Qt${QT_MAJOR_VERSION}QuickControls2 Qt${QT_MAJOR_VERSION}Widgets")
add_compile_definitions(QT_DISABLE_DEPRECATED_UP_TO=0x050F00)

set(GZ_GUI_PLUGIN_RELATIVE_INSTALL_DIR
  ${GZ_LIB_INSTALL_DIR}/gz-${GZ_DESIGNATION}-${PROJECT_VERSION_MAJOR}/plugins
)

#============================================================================
# Configure the build
#============================================================================
gz_configure_build(QUIT_IF_BUILD_ERRORS)

#============================================================================
# gz command line support
#============================================================================
add_subdirectory(conf)

#============================================================================
# Create package information
#============================================================================
gz_create_packages()

configure_file(${CMAKE_SOURCE_DIR}/api.md.in ${CMAKE_BINARY_DIR}/api.md)
configure_file(${CMAKE_SOURCE_DIR}/tutorials.md.in ${CMAKE_BINARY_DIR}/tutorials.md)

gz_create_docs(
  API_MAINPAGE_MD "${CMAKE_BINARY_DIR}/api.md"
  TUTORIALS_MAINPAGE_MD "${CMAKE_BINARY_DIR}/tutorials.md"
  ADDITIONAL_INPUT_DIRS "${CMAKE_SOURCE_DIR}/src/plugins"
  IMAGE_PATH_DIRS "${CMAKE_SOURCE_DIR}/tutorials/images"
  TAGFILES
    "${CMAKE_SOURCE_DIR}/doc/qt.tag.xml=http://doc.qt.io/qt-5/"
    "${GZ-MATH_DOXYGEN_TAGFILE} = ${GZ-MATH_API_URL}"
    "${GZ-MSGS_DOXYGEN_TAGFILE} = ${GZ-MSGS_API_URL}"
    "${GZ-RENDERING_DOXYGEN_TAGFILE} = ${GZ-RENDERING_API_URL}"
    "${GZ-TRANSPORT_DOXYGEN_TAGFILE} = ${GZ-TRANSPORT_API_URL}"
    "${GZ-COMMON_DOXYGEN_TAGFILE} = ${GZ-COMMON_API_URL}"
)

if(TARGET doc)
  file(COPY ${CMAKE_SOURCE_DIR}/tutorials/images/ DESTINATION ${CMAKE_BINARY_DIR}/doxygen/html/images/)
endif()
