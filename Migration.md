# Note on deprecations
A tick-tock release cycle allows easy migration to new software versions.
Obsolete code is marked as deprecated for one major release.
Deprecated code produces compile-time warnings. These warning serve as
notification to users that their code should be upgraded. The next major
release will remove the deprecated code.

## Gazebo GUI 6.X to 7.X

* The environment variable `IGN_GUI_PLUGIN_PATH` is deprecated. Use `GZ_GUI_PLUGIN_PATH` instead.
* The `ignition` namespace is deprecated and will be removed in future versions.  Use `gz` instead.

* Header files under `ignition/...` are deprecated and will be removed in future versions.
  Please use `gz/...` instead.

* The `gz-gui` element is deprecated and will be removed.
  Please use `gz-gui` instead.

* `ignitionVersion()` is deprecated and will be removed in future versions.
  Please use `gzVersion()` instead.

* The project name has been changed to use the `gz-` prefix, you **must** use the `gz` prefix!
  * This also means that any generated code that use the project name (e.g. CMake variables, in-source macros) would have to be migrated.
  * Some non-exhaustive examples of this include:
    * `GZ_<PROJECT>_<VISIBLE/HIDDEN>`
    * CMake `-config` files
    * Paths that depend on the project name

* QML `import ignition.gui 1.0` is deprecated, use `import gz.gui 1.0` instead.

## Gazebo GUI 6.2 to 6.3

* New QML dependencies, only needed for the NavSatMap plugin: `qml-module-qtlocation`, `qml-module-qtpositioning`

## Gazebo GUI 6.1 to 6.2

* All features from `Grid3D` have been incorporated into `GridConfig`. The code
  for the original `Grid3D` has been removed and now the installed library is
  just a copy of `GridConfig`. Existing `Grid3D` users shouldn't be affected and
  may continue to use `Grid3D` as before.

## Gazebo GUI 5.x to 6.x

* The `Scene3D` plugin is deprecated, use `MinimalScene` with
  `TransportSceneManager` instead.

## Gazebo GUI 5.1 to 5.2

* The `Application::PluginAdded` signal used to send empty strings. Now it
  sends the plugin's unique name.

## Gazebo GUI 4.4 to 4.5

* The `Application::PluginAdded` signal used to send empty strings. Now it
  sends the plugin's unique name.

## Gazebo GUI 3.x to 4.x

* Use rendering4, transport9 and msgs6.
* Deprecated `gz::gui::convert` to `gz::common::Time`.
  Use `gz::msgs::Convert` to `std::chrono::steady_clock::time_point`
  instead.

## Gazebo GUI 3.6 to 3.7

* The `Application::PluginAdded` signal used to send empty strings. Now it
  sends the plugin's unique name.

## Gazebo GUI 2.x to 3.x

* Use rendering3, transport8 and msgs5.

## Gazebo GUI 1.x to 2.x

* Plugin headers are no longer installed.

* Use ignition msgs4, rendering2, and transport7.

## Gazebo GUI 0.1.0 to 1.0.0

* Gazebo GUI v1 uses QtQuick and it is not compatible with v0, which uses QWidgets.
