cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(ignition-gui0 VERSION 0.1.0)

#============================================================================
# Find ignition-cmake
#============================================================================
find_package(ignition-cmake1 REQUIRED)

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
ign_find_package(ignition-math5 REQUIRED)
set(IGN_MATH_VER ${ignition-math5_VERSION_MAJOR})

#--------------------------------------
# Find ignition-common
ign_find_package(ignition-common2 REQUIRED)
set(IGN_COMMON_VER ${ignition-common2_VERSION_MAJOR})

#--------------------------------------
# Find ignition-transport
ign_find_package(ignition-transport5 REQUIRED)
set(IGN_TRANSPORT_VER ${ignition-transport5_VERSION_MAJOR})

#--------------------------------------
# Find ignition-rendering
ign_find_package(ignition-rendering0 REQUIRED)
set(IGN_RENDERING_VER ${ignition-rendering0_VERSION_MAJOR})

#--------------------------------------
# Find ignition-msgs
ign_find_package(ignition-msgs2 REQUIRED)
set(IGN_MSGS_VER ${ignition-msgs2_VERSION_MAJOR})

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

