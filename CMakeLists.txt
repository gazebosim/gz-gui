cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(ignition-gui1 VERSION 1.0.0)

#============================================================================
# Find ignition-cmake
#============================================================================
find_package(ignition-cmake2 REQUIRED)

#============================================================================
# Configure the project
#============================================================================
ign_configure_project(VERSION_SUFFIX pre2)

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
ign_find_package(IgnProtobuf
                 VERSION ${REQ_PROTOBUF_VER}
                 REQUIRED
                 PRETTY Protobuf)

#--------------------------------------
# Find Tinyxml2
ign_find_package(TINYXML2 REQUIRED PRIVATE PRETTY tinyxml2)

#--------------------------------------
# Find ignition-math
ign_find_package(ignition-math6 REQUIRED)
set(IGN_MATH_VER ${ignition-math6_VERSION_MAJOR})

#--------------------------------------
# Find ignition-common
ign_find_package(ignition-common3 REQUIRED)
set(IGN_COMMON_VER ${ignition-common3_VERSION_MAJOR})

#--------------------------------------
# Find ignition-transport
ign_find_package(ignition-transport6 REQUIRED)
set(IGN_TRANSPORT_VER ${ignition-transport6_VERSION_MAJOR})

#--------------------------------------
# Find ignition-rendering
ign_find_package(ignition-rendering1 REQUIRED)
set(IGN_RENDERING_VER ${ignition-rendering1_VERSION_MAJOR})

#--------------------------------------
# Find ignition-msgs
ign_find_package(ignition-msgs3 REQUIRED)
set(IGN_MSGS_VER ${ignition-msgs3_VERSION_MAJOR})

#--------------------------------------
# Find ignition-tools
ign_find_package(ignition-tools QUIET)
if (ignition-tools_FOUND)
  set (HAVE_IGN_TOOLS TRUE)
endif()

#--------------------------------------
# Find QT
find_package (Qt5
  COMPONENTS
    Core
    Quick
    QuickControls2
  REQUIRED
)

#--------------------------------------
# Find QWT (QT graphing library)
#find_path(QWT_INCLUDE_DIR NAMES qwt.h PATHS
#  /usr/include
#  /usr/local/include
#  /usr/local/lib/qwt.framework/Headers
#  ${QWT_WIN_INCLUDE_DIR}
#
#  PATH_SUFFIXES qwt qwt5
#)
#if (NOT QWT_INCLUDE_DIR)
#  ign_build_error("qwt5 header file was not found")
#endif()
#
#if(APPLE)
#  find_library(QWT_LIBRARY NAMES qwt PATHS
#    /usr/lib
#    /usr/local/lib
#    /usr/local/lib/qwt.framework
#    ${QWT_WIN_LIBRARY_DIR}
#  )
#else()
#  find_library(QWT_LIBRARY NAMES qwt-qt5 PATHS
#    /usr/lib
#    /usr/local/lib
#    /usr/local/lib/qwt.framework
#    ${QWT_WIN_LIBRARY_DIR}
#  )
#endif()
#if (NOT QWT_LIBRARY)
#  ign_build_error("qwt5 library was not found")
#endif()

#============================================================================
# Configure the build
#============================================================================
ign_configure_build(QUIT_IF_BUILD_ERRORS)

#============================================================================
# ign command line support
#============================================================================
add_subdirectory(conf)

#============================================================================
# Create package information
#============================================================================
ign_create_packages()

configure_file(${CMAKE_SOURCE_DIR}/api.md.in ${CMAKE_BINARY_DIR}/api.md)
configure_file(${CMAKE_SOURCE_DIR}/tutorials.md.in ${CMAKE_BINARY_DIR}/tutorials.md)

ign_create_docs(
  API_MAINPAGE_MD "${CMAKE_BINARY_DIR}/api.md"
  TUTORIALS_MAINPAGE_MD "${CMAKE_BINARY_DIR}/tutorials.md"
  TAGFILES
  "\"${CMAKE_SOURCE_DIR}/doc/qt.tag.xml=http://doc.qt.io/qt-5/\" \
   \"${IGNITION-MATH_DOXYGEN_TAGFILE} = ${IGNITION-MATH_API_URL}\" \
   \"${IGNITION-MSGS_DOXYGEN_TAGFILE} = ${IGNITION-MSGS_API_URL}\" \
   \"${IGNITION-TRANSPORT_DOXYGEN_TAGFILE} = ${IGNITION-TRANSPORT_API_URL}\" \
   \"${IGNITION-COMMON_DOXYGEN_TAGFILE} = ${IGNITION-COMMON_API_URL}\""
  )
