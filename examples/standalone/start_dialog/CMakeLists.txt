cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

project(gz-gui-start-dialog)

if(POLICY CMP0100)
  cmake_policy(SET CMP0100 NEW)
endif()

set(CMAKE_AUTOMOC ON)

find_package(gz-gui8 REQUIRED)
set(GZ_GUI_VER ${gz-gui8_VERSION_MAJOR})

set(EXEC_NAME "start_dialog")

QT5_ADD_RESOURCES(resources_RCC ${EXEC_NAME}.qrc)

add_executable(${EXEC_NAME}
  ${EXEC_NAME}.cc
  ${resources_RCC}
)
target_link_libraries(${EXEC_NAME}
  gz-gui${GZ_GUI_VER}::gz-gui${GZ_GUI_VER}
)

