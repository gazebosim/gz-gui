# Note on deprecations
A tick-tock release cycle allows easy migration to new software versions.
Obsolete code is marked as deprecated for one major release.
Deprecated code produces compile-time warnings. These warning serve as
notification to users that their code should be upgraded. The next major
release will remove the deprecated code.

## Ignition GUI 6.1 to 6.2

* All features from `Grid3D` have been incorportated into `GridConfig`. The code
  for the original `Grid3D` has been removed and now the installed library is
  just a copy of `GridConfig`. Existing `Grid3D` users shouldn't be affected and
  may continue to use `Grid3D` as before.

## Ignition GUI 5.x to 6.x

* The `Scene3D` plugin is deprecated, use `MinimalScene` with
  `TransportSceneManager` instead.

## Ignition GUI 5.1 to 5.2

* The `Application::PluginAdded` signal used to send empty strings. Now it
  sends the plugin's unique name.

## Ignition GUI 4.4 to 4.5

* The `Application::PluginAdded` signal used to send empty strings. Now it
  sends the plugin's unique name.

## Ignition GUI 3.x to 4.x

* Use rendering4, transport9 and msgs6.
* Deprecated `ignition::gui::convert` to `ignition::common::Time`.
  Use `ignition::msgs::Convert` to `std::chrono::steady_clock::time_point`
  instead.

## Ignition GUI 3.6 to 3.7

* The `Application::PluginAdded` signal used to send empty strings. Now it
  sends the plugin's unique name.

## Ignition GUI 2.x to 3.x

* Use rendering3, transport8 and msgs5.

## Ignition GUI 1.x to 2.x

* Plugin headers are no longer installed.

* Use ignition msgs4, rendering2, and transport7.

## Ignition GUI 0.1.0 to 1.0.0

* Ignition GUI v1 uses QtQuick and it is not compatible with v0, which uses QWidgets.

