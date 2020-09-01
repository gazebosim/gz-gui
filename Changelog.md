## Ignition Gui 2

### Ignition Gui 2.3.3 (2020-08-31)

1. Fix running plugins with bad world names.
    * [Pull request 111](https://github.com/ignitionrobotics/ign-gui/pull/111)

1. Disable more tests known to fail on macOS and Windows.
    * [Pull request 112](https://github.com/ignitionrobotics/ign-gui/pull/112)

### Ignition Gui 2.3.2 (2020-08-19)

1. Revert pull request #107
    * [Pull request 110](https://github.com/ignitionrobotics/ign-gui/pull/110)

### Ignition Gui 2.3.1 (2020-08-17)

1. Fix running plugins with bad world name
    * [Pull request 107](https://github.com/ignitionrobotics/ign-gui/pull/107)

### Ignition Gui 2.3.0 (2020-08-11)

1. Add shift + drag to rotate camera
    * [Pull request 96](https://github.com/ignitionrobotics/ign-gui/pull/96)

1. Scroll bar adjustment
    * [Pull request 97](https://github.com/ignitionrobotics/ign-gui/pull/97)

1. Minimize GUI plugin
    * [Pull request 84](https://github.com/ignitionrobotics/ign-gui/pull/84)
    * [Pull request 99](https://github.com/ignitionrobotics/ign-gui/pull/99)

1. Fix tests on actions
    * [Pull request 98](https://github.com/ignitionrobotics/ign-gui/pull/98)

1. Disable failing tests
    * [Pull request 76](https://github.com/ignitionrobotics/ign-gui/pull/76)

1. Use world name in default topics
    * [Pull request 104](https://github.com/ignitionrobotics/ign-gui/pull/104)

### Ignition Gui 2.2.0 (2020-06-17)

1. Sort GUI plugins alphabetically
    * [Pull request 71](https://github.com/ignitionrobotics/ign-gui/pull/71)

1. Add color palette to plugin settings to change background color
    * [Pull request 62](https://github.com/ignitionrobotics/ign-gui/pull/62)

1. Close plugin menu after a plugin has been added
    * [Pull request 60](https://github.com/ignitionrobotics/ign-gui/pull/60)

1. Resize undocked plugins by dragging corners
    * [Pull request 78](https://github.com/ignitionrobotics/ign-gui/pull/78)

1. Move each plugin to its own directory
    * [Pull request 64](https://github.com/ignitionrobotics/ign-gui/pull/64)

1. GitHub migration
    * [Pull request 46](https://github.com/ignitionrobotics/ign-gui/pull/46)
    * [Pull request 56](https://github.com/ignitionrobotics/ign-gui/pull/56)

1. Fix homebrew warnings
    * [BitBucket pull request 249](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/249)


### Ignition Gui 2.1.0 (2020-03-04)

1. Add space for pausing sim
    * [BitBucket pull request 245](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/245)

1. Expose QML context to plugins
    * [BitBucket pull request 244](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/244)

1. Prettify plugin menu
    * [BitBucket pull request 231](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/231)

1. Example opening a dialog and accessing main window from plugin
    * [BitBucket pull request 242](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/242)

1. Example using Ign\*.qml components
    * [BitBucket pull request 240](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/240)

1. Protect against null tinyxml GetText
    * [BitBucket pull request 236](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/236)

1. Config tutorial
    * [BitBucket pull request 233](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/233)

1. Add copyright to all QML files
    * [BitBucket pull request 232](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/232)

1. Fix changing topics in ImageDisplay
    * [BitBucket pull request 224](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/224)

### Ignition Gui 2.0.1

1. Made image display responsive.
    * [BitBucket pull request 223](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/223)

1. Downstream applications can override this using MainWindow::setIcon once they get the window.
    * [BitBucket pull request 222](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/222)

### Ignition Gui 2.0.0

1. Delete QmlApplicationEngine later.
    * [BitBucket pull request 209](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/209)

1. Decouple toolbar colors from material theme colors.
    * [BitBucket pull request 208](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/208)

1. Stop mouse wheel propagation at the card level.
    * [BitBucket pull request 207](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/207)

1. Fix theme support.
    * [BitBucket pull request 206](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/206)

1. Don't install plugin headers, move images to unported.
    * [BitBucket pull request 205](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/205)

1. Application PluginAdded signal.
    * [BitBucket pull request 204](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/204)

1. Use the new `msgs::Image::pixel_format_type` field, and treat `msgs::Image::pixel_format` as a backup.
    * [BitBucket pull request 203](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/203)

1. Upgrade to ignition-msgs4 and ignition-transport7.
    * [BitBucket pull request 202](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/202)

1. Fix topic echo test and make msgs visible.
    * [BitBucket pull request 201](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/201)

## Ignition Gui 1

### Ignition Gui 1.X.X

1. Move files not ported to v1 to a separate dir for clarity.
    * [BitBucket pull request 196](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/196)

1. Get ign-gui plugin interface in case there are others.
    * [BitBucket pull request 186](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/186)

### Ignition Gui 1.0.0
