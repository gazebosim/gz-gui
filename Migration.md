# Note on deprecations
A tick-tock release cycle allows easy migration to new software versions.
Obsolete code is marked as deprecated for one major release.
Deprecated code produces compile-time warnings. These warning serve as
notification to users that their code should be upgraded. The next major
release will remove the deprecated code.

## Gazebo GUI 3.10 to 3.11

* `Dialog::ReadConfigAttribute` doesn't create a missing file anymore.

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

