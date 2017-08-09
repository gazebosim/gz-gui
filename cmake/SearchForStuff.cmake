include (${project_cmake_dir}/Utils.cmake)
include (CheckCXXSourceCompiles)

include (${project_cmake_dir}/FindOS.cmake)
include (FindPkgConfig)

########################################
# Include man pages stuff
include (${project_cmake_dir}/Ronn2Man.cmake)
add_manpage_target()

#################################################
# Ignition tools
find_package(ignition-tools QUIET)
if (IGNITION-TOOLS_BINARY_DIRS)
  message (STATUS "Found Ignition Tools")
  set (HAVE_IGN_TOOLS TRUE)
else()
  BUILD_WARNING ("ignition-tools not found, for command line utilities, please install ignition-tools.")
endif()

################################################################################
# Ignition common
find_package(ignition-common0 QUIET)
if (NOT ignition-common0_FOUND)
  BUILD_ERROR ("Missing: Ignition Common (libignition-common0-dev)")
else()
  message (STATUS "Found Ignition Common")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-COMMON_CXX_FLAGS}")
  include_directories(${IGNITION-COMMON_INCLUDE_DIRS})
  link_directories(${IGNITION-COMMON_LIBRARY_DIRS})
endif()

########################################
# Ignition transport
find_package(ignition-transport3 QUIET)
if (NOT ignition-transport3_FOUND)
  BUILD_ERROR ("Missing: Ignition Transport libignition-transport3-dev.")
else()
  message (STATUS "Found Ignition Transport")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-TRANSPORT_CXX_FLAGS}")
  include_directories(${IGNITION-TRANSPORT_INCLUDE_DIRS})
  link_directories(${IGNITION-TRANSPORT_LIBRARY_DIRS})
  set (HAVE_IGN_TRANSPORT TRUE)
endif()

########################################
# Find QT
find_package (Qt5Widgets)
if (NOT Qt5Widgets_FOUND)
  BUILD_ERROR("Missing: Qt5Widgets")
endif()

find_package (Qt5Core)
if (NOT Qt5Core_FOUND)
  BUILD_ERROR("Missing: Qt5Core")
endif()

find_package (Qt5OpenGL)
if (NOT Qt5OpenGL_FOUND)
  BUILD_ERROR("Missing: Qt5OpenGL")
endif()

find_package (Qt5Test)
if (NOT Qt5Test_FOUND)
  BUILD_ERROR("Missing: Qt5Test")
endif()

########################################
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

# version
set ( _VERSION_FILE ${QWT_INCLUDE_DIR}/qwt_global.h )
file ( STRINGS ${_VERSION_FILE} _VERSION_LINE REGEX "define[ ]+QWT_VERSION_STR" )
if ( _VERSION_LINE )
  string ( REGEX REPLACE ".*define[ ]+QWT_VERSION_STR[ ]+\"(.*)\".*" "\\1"
      QWT_VERSION_STRING "${_VERSION_LINE}" )
  string ( REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+).*" "\\1"
      QWT_MAJOR_VERSION "${QWT_VERSION_STRING}" )
  string ( REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+).*" "\\2"
      QWT_MINOR_VERSION "${QWT_VERSION_STRING}" )
  string ( REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+).*" "\\3"
      QWT_PATCH_VERSION "${QWT_VERSION_STRING}" )
  set(QWT_VERSION
    ${QWT_MAJOR_VERSION}.${QWT_MINOR_VERSION}.${QWT_PATCH_VERSION})
endif ()

# in Windows, the path need to point to the parent to get correct qwt/foo headers
if (WIN32)
  SET(QWT_INCLUDE_DIR "${QWT_INCLUDE_DIR}\\..")
endif()

if (QWT_INCLUDE_DIR AND QWT_LIBRARY AND (NOT ${QWT_VERSION} VERSION_LESS 6.1.0))
  message (STATUS "Looking for qwt - found: version ${QWT_VERSION}")
else()
  message (STATUS "Looking for qwt >= 6.1.0 - not found")
  BUILD_ERROR ("Missing: libqwt-dev. Required for plotting.")
endif ()

#################################################
# Find tinyxml2. Only debian distributions package tinyxml with a pkg-config
# Use pkg_check_modules and fallback to manual detection
# (needed, at least, for MacOS)

# Use system installation on UNIX and Apple, and internal copy on Windows
if (UNIX OR APPLE)
  message (STATUS "Using system tinyxml2.")
  set (USE_EXTERNAL_TINYXML2 True)
elseif(WIN32)
  message (STATUS "Using internal tinyxml2.")
  set (USE_EXTERNAL_TINYXML2 False)
else()
  message (STATUS "Unknown platform, unable to configure tinyxml2.")
  BUILD_ERROR("Unknown platform")
endif()

if (USE_EXTERNAL_TINYXML2)
  pkg_check_modules(tinyxml2 tinyxml2)
  if (NOT tinyxml2_FOUND)
      find_path (tinyxml2_INCLUDE_DIRS tinyxml2.h ${tinyxml2_INCLUDE_DIRS} ENV CPATH)
      find_library(tinyxml2_LIBRARIES NAMES tinyxml2)
      set (tinyxml2_FAIL False)
      if (NOT tinyxml2_INCLUDE_DIRS)
        message (STATUS "Looking for tinyxml2 headers - not found")
        set (tinyxml2_FAIL True)
      endif()
      if (NOT tinyxml2_LIBRARIES)
        message (STATUS "Looking for tinyxml2 library - not found")
        set (tinyxml2_FAIL True)
      endif()
      if (NOT tinyxml2_LIBRARY_DIRS)
        message (STATUS "Looking for tinyxml2 library dirs - not found")
        set (tinyxml2_FAIL True)
      endif()
  endif()

  if (tinyxml2_FAIL)
    message (STATUS "Looking for tinyxml2.h - not found")
    BUILD_ERROR("Missing: tinyxml2")
  else()
    include_directories(${tinyxml2_INCLUDE_DIRS})
    link_directories(${tinyxml2_LIBRARY_DIRS})
  endif()
else()
  # Needed in WIN32 since in UNIX the flag is added in the code installed
  message (STATUS "Skipping search for tinyxml2")
  set (tinyxml2_INCLUDE_DIRS "")
  set (tinyxml2_LIBRARIES "")
  set (tinyxml2_LIBRARY_DIRS "")
endif()
