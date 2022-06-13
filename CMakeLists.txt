cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(ignition-gui7 VERSION 7.0.0)

#============================================================================
# Find ignition-cmake
#============================================================================
find_package(ignition-cmake3 REQUIRED)

#============================================================================
# Configure the project
#============================================================================
gz_configure_project(
  REPLACE_IGNITION_INCLUDE_PATH gz/gui
  VERSION_SUFFIX pre1)

#============================================================================
# Set project-specific options
#============================================================================


#============================================================================
# Search for project-specific dependencies
#============================================================================
message(STATUS "\n\n-- ====== Finding Dependencies ======")

#--------------------------------------
# Find Protobuf
set(REQ_PROTOBUF_VER 3)
gz_find_package(IgnProtobuf
                 VERSION ${REQ_PROTOBUF_VER}
                 REQUIRED
                 PRETTY Protobuf)

#--------------------------------------
# Find Tinyxml2
gz_find_package(TINYXML2 REQUIRED PRIVATE PRETTY tinyxml2)

#--------------------------------------
# Find ignition-utils
gz_find_package(ignition-utils2 REQUIRED)
set(IGN_UTILS_VER ${ignition-utils2_VERSION_MAJOR})

#--------------------------------------
# Find ignition-math
gz_find_package(ignition-math7 REQUIRED)
set(IGN_MATH_VER ${ignition-math7_VERSION_MAJOR})

#--------------------------------------
# Find ignition-common
gz_find_package(ignition-common5 REQUIRED COMPONENTS profiler)
set(IGN_COMMON_VER ${ignition-common5_VERSION_MAJOR})

#--------------------------------------
# Find ignition-plugin
gz_find_package(ignition-plugin2 REQUIRED COMPONENTS loader register)
set(IGN_PLUGIN_VER ${ignition-plugin2_VERSION_MAJOR})

#--------------------------------------
# Find ignition-transport
gz_find_package(ignition-transport12 REQUIRED)
set(IGN_TRANSPORT_VER ${ignition-transport12_VERSION_MAJOR})

#--------------------------------------
# Find ignition-rendering
gz_find_package(ignition-rendering7 REQUIRED)
set(IGN_RENDERING_VER ${ignition-rendering7_VERSION_MAJOR})

#--------------------------------------
# Find ignition-msgs
gz_find_package(ignition-msgs9 REQUIRED)
set(IGN_MSGS_VER ${ignition-msgs9_VERSION_MAJOR})

#--------------------------------------
# Find if gz command is available
find_program(HAVE_GZ_TOOLS gz)

#--------------------------------------
# Find QT
gz_find_package (Qt5
  COMPONENTS
    Core
    Quick
    QuickControls2
    Widgets
  REQUIRED
  PKGCONFIG "Qt5Core Qt5Quick Qt5QuickControls2 Qt5Widgets"
)

set(GZ_GUI_PLUGIN_INSTALL_DIR
  ${CMAKE_INSTALL_PREFIX}/${IGN_LIB_INSTALL_DIR}/ign-${IGN_DESIGNATION}-${PROJECT_VERSION_MAJOR}/plugins
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
    "${IGNITION-MATH_DOXYGEN_TAGFILE} = ${IGNITION-MATH_API_URL}"
    "${IGNITION-MSGS_DOXYGEN_TAGFILE} = ${IGNITION-MSGS_API_URL}"
    "${IGNITION-RENDERING_DOXYGEN_TAGFILE} = ${IGNITION-RENDERING_API_URL}"
    "${IGNITION-TRANSPORT_DOXYGEN_TAGFILE} = ${IGNITION-TRANSPORT_API_URL}"
    "${IGNITION-COMMON_DOXYGEN_TAGFILE} = ${IGNITION-COMMON_API_URL}"
)

if(TARGET doc)
  file(COPY ${CMAKE_SOURCE_DIR}/tutorials/images/ DESTINATION ${CMAKE_BINARY_DIR}/doxygen/html/images/)
endif()
