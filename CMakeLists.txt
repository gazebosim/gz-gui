cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(ignition-gui0 VERSION 0.1.0)

#============================================================================
# Find ignition-cmake
#============================================================================
find_package(ignition-cmake0 REQUIRED)

#============================================================================
# Configure the project
#============================================================================
ign_configure_project()

#============================================================================
# Set project-specific options
#============================================================================


#============================================================================
# Search for project-specific dependencies
#============================================================================
message(STATUS "\n\n-- ====== Finding Dependencies ======")

#--------------------------------------
# Find Protobuf
if(MSVC)
  # On Windows we require version 3+ in order to have proper support for
  # shared libraries.
  set(REQ_PROTOBUF_VER 3)
  set(PROTOBUF_PURPOSE "Windows requires version 3+ to ensure proper support for DLLs")
else()
  set(REQ_PROTOBUF_VER 2.3.0)
  set(PROTOBUF_PURPOSE)
endif()
ign_find_package(IgnProtobuf
                 VERSION ${REQ_PROTOBUF_VER}
                 REQUIRED
                 PRETTY Protobuf
                 PURPOSE ${PROTOBUF_PURPOSE})

#--------------------------------------
# Find Tinyxml2
ign_find_package(TINYXML2 REQUIRED PRIVATE PRETTY tinyxml2)

#--------------------------------------
# Find ignition-math
set(IGN_MATH_VER 4)
ign_find_package(ignition-math${IGN_MATH_VER} REQUIRED)

#--------------------------------------
# Find ignition-common
set(IGN_COMMON_VER 0)
ign_find_package(ignition-common${IGN_COMMON_VER} REQUIRED)

#--------------------------------------
# Find ignition-msgs
set(IGN_MSGS_VER 1)
ign_find_package(ignition-msgs${IGN_MSGS_VER} REQUIRED)

#--------------------------------------
# Find ignition-transport
find_package(ignition-transport3 QUIET)
if (NOT ignition-transport3_FOUND)
  ign_build_error("Missing: Ignition Transport libignition-transport3-dev.")
else()
  message (STATUS "Found Ignition Transport 3")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-TRANSPORT_CXX_FLAGS}")
  include_directories(${IGNITION-TRANSPORT_INCLUDE_DIRS})
  link_directories(${IGNITION-TRANSPORT_LIBRARY_DIRS})
endif()

#--------------------------------------
# Find ignition-tools
ign_find_package(ignition-tools QUIET)
if (ignition-tools_FOUND)
  set (HAVE_IGN_TOOLS TRUE)
endif()

#--------------------------------------
# Find QT
find_package (Qt5Widgets)
if (NOT Qt5Widgets_FOUND)
  ign_build_error("Missing: Qt5Widgets")
else()
  message (STATUS "Found Qt5Widgets")
endif()

find_package (Qt5Core)
if (NOT Qt5Core_FOUND)
  ign_build_error("Missing: Qt5Core")
else()
  message (STATUS "Found Qt5Core")
endif()

#--------------------------------------
# Find QWT (QT graphing library)
find_path(QWT_INCLUDE_DIR NAMES qwt.h PATHS
  /usr/include
  /usr/local/include
  /usr/local/lib/qwt.framework/Headers
  ${QWT_WIN_INCLUDE_DIR}

  PATH_SUFFIXES qwt qwt5
)

find_library(QWT_LIBRARY NAMES qwt-qt5 qwt PATHS
  /usr/lib
  /usr/local/lib
  /usr/local/lib/qwt.framework
  ${QWT_WIN_LIBRARY_DIR}
)

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

