# Note on deprecations
A tick-tock release cycle allows easy migration to new software versions.
Obsolete code is marked as deprecated for one major release.
Deprecated code produces compile-time warnings. These warning serve as
notification to users that their code should be upgraded. The next major
release will remove the deprecated code.

## Ignition GUI 1.x to 2.x

* Plugin headers are no longer installed.

## Ignition GUI 0.1.0 to 1.0.0

* Ignition GUI v1 uses QtQuick and is is not compatible with v0, which uses QWidgets.

