cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(gz-gui-examples)

add_subdirectory(plugin/custom_context_menu)
add_subdirectory(plugin/dialog_from_plugin)
add_subdirectory(plugin/gz_components)
add_subdirectory(plugin/hello_plugin)
add_subdirectory(plugin/multiple_qml)

add_subdirectory(standalone/custom_drawer)
add_subdirectory(standalone/dialogs)
add_subdirectory(standalone/marker)
add_subdirectory(standalone/point_cloud)
add_subdirectory(standalone/scene_provider)
add_subdirectory(standalone/start_dialog)
add_subdirectory(standalone/window)
