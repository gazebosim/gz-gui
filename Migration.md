# Note on deprecations
A tick-tock release cycle allows easy migration to new software versions.
Obsolete code is marked as deprecated for one major release.
Deprecated code produces compile-time warnings. These warning serve as
notification to users that their code should be upgraded. The next major
release will remove the deprecated code.

## Ignition GUI 3.x to 4.x

* Use rendering4, transport9 and msgs6.
* Deprecated `ignition::gui::convert` to `ignition::common::Time`.
  Use `ignition::msgs::Convert` to `std::chrono::steady_clock::time_point`
  instead.

## Ignition GUI 2.x to 3.x

* Use rendering3, transport8 and msgs5.

## Ignition GUI 1.x to 2.x

* Plugin headers are no longer installed.

* Use ignition msgs4, rendering2, and transport7.

## Ignition GUI 0.1.0 to 1.0.0

* Ignition GUI v1 uses QtQuick and it is not compatible with v0, which uses QWidgets.

