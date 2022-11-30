## Gazebo GUI 3

### Gazebo GUI 3.11.2 (2022-08-17)

1. Fix mistaken dialog error message
    * [Pull request #472](https://github.com/gazebosim/gz-gui/pull/472)

### Gazebo GUI 3.11.1 (2022-08-15)

1. Replace pose in Grid3d with GzPose
    * [Pull request #460](https://github.com/gazebosim/gz-gui/pull/461)

1. Remove redundant namespace references
    * [Pull request #460](https://github.com/gazebosim/gz-gui/pull/460)

1. Update codeowners
    * [Pull request #465](https://github.com/gazebosim/gz-gui/pull/465)

1. `GzColor` `ColorDialogue` fix
    * [Pull request #459](https://github.com/gazebosim/gz-gui/pull/459)

1. Fix attribute update when `<dialog>` isn't present
    * [Pull request #455](https://github.com/gazebosim/gz-gui/pull/455)

1. Add test for `Plugin::ConfigStr()`
    * [Pull request #447](https://github.com/gazebosim/gz-gui/pull/447)

### Gazebo GUI 3.11.0 (2022-08-02)

1. Dialog read attribute fixes
    * [Pull request #450](https://github.com/gazebosim/gz-gui/pull/450)
    * [Pull request #442](https://github.com/gazebosim/gz-gui/pull/442)

1. Fixed topic echo test
    * [Pull request #448](https://github.com/gazebosim/gz-gui/pull/448)

1. Teleop: Refactor and support vertical
    * [Pull request #440](https://github.com/gazebosim/gz-gui/pull/440)

1. Change `IGN_DESIGNATION` to `GZ_DESIGNATION`
    * [Pull request #437](https://github.com/gazebosim/gz-gui/pull/437)

1. Ignition -> Gazebo
    * [Pull request #435](https://github.com/gazebosim/gz-gui/pull/435)

### Gazebo Gui 3.10.0 (2022-07-13)

1. Add common widget for vector3
    * [Pull request #427](https://github.com/gazebosim/gz-gui/pull/427)

1. Allow Dialogs to have a MainWindow independent config
    * [Pull request #418](https://github.com/gazebosim/gz-gui/pull/418)

1. Add common widget for pose
    * [Pull request #424](https://github.com/gazebosim/gz-gui/pull/424)
    * [Pull request #431](https://github.com/gazebosim/gz-gui/pull/431)

1. Example running a dialog before the main window
    * [Pull request #407](https://github.com/gazebosim/gz-gui/pull/407)

1. Common widget GzColor
    * [Pull request #410](https://github.com/gazebosim/gz-gui/pull/410)

1. Fix gz_TEST
    * [Pull request #420](https://github.com/gazebosim/gz-gui/pull/420)

1. Make display tests more robust
    * [Pull request #419](https://github.com/gazebosim/gz-gui/pull/419)

1. Bash completion for flags
    * [Pull request #392](https://github.com/gazebosim/gz-gui/pull/392)

1. Disable failing test on Citadel
    * [Pull request #416](https://github.com/gazebosim/gz-gui/pull/416)

1. Search menu keyboard control
    * [Pull request #403](https://github.com/gazebosim/gz-gui/pull/403)
    * [Pull request #405](https://github.com/gazebosim/gz-gui/pull/405)

1. Add config relative path environment variable
    * [Pull request #386](https://github.com/gazebosim/gz-gui/pull/386)

1. Sort plugin list in alphabetical order (including when filtering)
    * [Pull request #387](https://github.com/gazebosim/gz-gui/pull/387)

1. Added array to snackbar qml
    * [Pull request #370](https://github.com/gazebosim/gz-gui/pull/370)

1. Fix some Qt warnings
    * [Pull request #376](https://github.com/gazebosim/gz-gui/pull/376)

1. Added Snackbar qtquick object
    * [Pull request #369](https://github.com/gazebosim/gz-gui/pull/369)

1. Fix menu scrolling when a new plugin is added
    * [Pull request #368](https://github.com/gazebosim/gz-gui/pull/368)

1. Improve KeyPublisher's usability
    * [Pull request #362](https://github.com/gazebosim/gz-gui/pull/362)

1. Backport GridConfig improvements to Citadel's Grid3D
    * [Pull request #363](https://github.com/gazebosim/gz-gui/pull/363)

### Gazebo Gui 3.9.0 (2022-01-14)

1. Added a button that allows shutting down both the client and server.
    * [Pull request #335](https://github.com/gazebosim/gz-gui/pull/335)

1. Prevent Scene3D 💥 if another scene is already loaded
    * [Pull request #347](https://github.com/gazebosim/gz-gui/pull/347)

1. Add project() to examples and remove hard-dependency on Ogre1
    * [Pull request #345](https://github.com/gazebosim/gz-gui/pull/345)

1. Fix codecheck
    * [Pull request #329](https://github.com/gazebosim/gz-gui/pull/329)

1. Use `qmldir` to define QML module with `IgnSpinBox`
    * [Pull request #319](https://github.com/gazebosim/gz-gui/pull/319)

1. Fix `TopicEcho` plugin message display
    * [Pull request #322](https://github.com/gazebosim/gz-gui/pull/322)

1. Don't crash if a plugin has invalid QML
    * [Pull request #315](https://github.com/gazebosim/gz-gui/pull/315)

1. Added log storing for `ign gui` CLI
    * [Pull request #272](https://github.com/gazebosim/gz-gui/pull/272)

### Gazebo Gui 3.8.0 (2021-10-12)

1. Improved doxygen
    * [Pull request #275](https://github.com/gazebosim/gz-gui/pull/275)

1. Add a filter to the plugin menu
    * [Pull request #277](https://github.com/gazebosim/gz-gui/pull/277)

1. 👩‍🌾 Remove bitbucket-pipelines.yml
    * [Pull request #274](https://github.com/gazebosim/gz-gui/pull/274)

1. Require ign-rendering 3.5
    * [Pull request #264](https://github.com/gazebosim/gz-gui/pull/264)

1. New teleop plugin implementation.
    * [Pull request #245](https://github.com/gazebosim/gz-gui/pull/245)

### Gazebo Gui 3.7.0 (2021-07-14)

1. Fix codeowners
    * [Pull request #251](https://github.com/gazebosim/gz-gui/pull/251)

1. Fix plugin added signal, add PluginByName
    * [Pull request #249](https://github.com/gazebosim/gz-gui/pull/249)

1. Fixed tests by passing valid argv
    * [Pull request #244](https://github.com/gazebosim/gz-gui/pull/244)

1. Screenshot plugin fixed dbg message
    * [Pull request #246](https://github.com/gazebosim/gz-gui/pull/246)

1. Detect ign instead of using cmake module to check for ignition-tools
    * [Pull request #240](https://github.com/gazebosim/gz-gui/pull/240)

### Gazebo Gui 3.6.0 (2021-06-17)

1. Update codeowners
    * [Pull request #232](https://github.com/gazebosim/gz-gui/pull/232)

1. Confirmation dialog when closing main window
    * [Pull request #225](https://github.com/gazebosim/gz-gui/pull/225)

1. Avoid grid3D crash
    * [Pull request #227](https://github.com/gazebosim/gz-gui/pull/227)

1. Emit more events from Scene3D
    * [Pull request #213](https://github.com/gazebosim/gz-gui/pull/213)

1. Removed duplicated code with rendering::sceneFromFirstRenderEngine
    * [Pull request #223](https://github.com/gazebosim/gz-gui/pull/223)

1. Remove `tools/code_check` and update codecov
    * [Pull request #222](https://github.com/gazebosim/gz-gui/pull/222)

1. Fixed material specular in `Scene3D`
    * [Pull request #218](https://github.com/gazebosim/gz-gui/pull/218)

1. `check_test_ran.py`: remove grep/xsltproc
    * [Pull request #203](https://github.com/gazebosim/gz-gui/pull/203)

### Gazebo Gui 3.5.1 (2021-03-18)

1. Scene3D: port mesh material fixes from ign-gazebo
    * [Pull request #191](https://github.com/gazebosim/gz-gui/pull/191)

1. Improve the height of plugins in the right split
    * [Pull request #194](https://github.com/gazebosim/gz-gui/pull/194)

### Gazebo Gui 3.5.0 (2021-03-10)

1. Screenshot plugin
    * [Pull request #170](https://github.com/gazebosim/gz-gui/pull/170)

1. Master branch updates
    * [Pull request #187](https://github.com/gazebosim/gz-gui/pull/187)

1. Backport Publisher test from v4
    * [Pull request #173](https://github.com/gazebosim/gz-gui/pull/173)

1. Better error messages when component can't be loaded
    * [Pull request #175](https://github.com/gazebosim/gz-gui/pull/175)

1. Make Grid3D plugin more flexible
    * [Pull request #172](https://github.com/gazebosim/gz-gui/pull/172)

1. Add Windows Installation
    * [Pull request #168](https://github.com/gazebosim/gz-gui/pull/168)

1. Fix codecheck
    * [Pull request #157](https://github.com/gazebosim/gz-gui/pull/157)

### Gazebo Gui 3.4.0 (2020-12-10)

1. Publish plugin API docs
    * [Pull request 128](https://github.com/gazebosim/gz-gui/pull/128)

1. Tutorial tweaks
    * [Pull request 132](https://github.com/gazebosim/gz-gui/pull/132)

1. Floating and standalone plugins respect minimum dimensions
    * [Pull request 135](https://github.com/gazebosim/gz-gui/pull/135)

1. Add scrollable indicator for plugin menu
    * [Pull request 134](https://github.com/gazebosim/gz-gui/pull/134)

1. Re-enable image.config test
    * [Pull request 148](https://github.com/gazebosim/gz-gui/pull/148)

1. Improve fork experience
    * [Pull request 139](https://github.com/gazebosim/gz-gui/pull/139)

1. Resolve updated codecheck issues
    * [Pull request 144](https://github.com/gazebosim/gz-gui/pull/144)

1. Port Gazebo GUI events to Ignition GUI
    * [Pull request 148](https://github.com/gazebosim/gz-gui/pull/148)

1. Change deprecated Qt::MidButton
    * [Pull request 153](https://github.com/gazebosim/gz-gui/pull/153)

1. Add right mouse events and tests
    * [Pull request 154](https://github.com/gazebosim/gz-gui/pull/154)

### Gazebo Gui 3.3.0 (2020-08-31)

1. rename key publisher plugin
    * [Pull request 93](https://github.com/gazebosim/gz-gui/pull/93)

1. Add shift + drag to rotate camera
    * [Pull request 96](https://github.com/gazebosim/gz-gui/pull/96)

1. Scroll bar adjustment
    * [Pull request 97](https://github.com/gazebosim/gz-gui/pull/97)

1. Minimize GUI plugin
    * [Pull request 84](https://github.com/gazebosim/gz-gui/pull/84)
    * [Pull request 99](https://github.com/gazebosim/gz-gui/pull/99)
    * [Pull request 102](https://github.com/gazebosim/gz-gui/pull/102)

1. Fix tests on actions
    * [Pull request 98](https://github.com/gazebosim/gz-gui/pull/98)

1. Disable failing tests
    * [Pull request 76](https://github.com/gazebosim/gz-gui/pull/76)

1. Use world name in default topics
    * [Pull request 104](https://github.com/gazebosim/gz-gui/pull/104)

1. Fix running plugins with bad world name
    * [Pull request 107](https://github.com/gazebosim/gz-gui/pull/107)
    * [Pull request 110](https://github.com/gazebosim/gz-gui/pull/110)
    * [Pull request 111](https://github.com/gazebosim/gz-gui/pull/111)

1. Disable more tests known to fail on macOS and Windows
    * [Pull request 112](https://github.com/gazebosim/gz-gui/pull/112)

1. Add render event
    * [Pull request 70](https://github.com/gazebosim/gz-gui/pull/70)

1. Fixed left menu events
    * [Pull request 85](https://github.com/gazebosim/gz-gui/pull/85)

1. Added Topic Viewer plugin, list all transport topics
    * [Pull request 69](https://github.com/gazebosim/gz-gui/pull/69)

1. Fix ImageDisplay in Ubuntu 20.04
    * [Pull request 96](https://github.com/gazebosim/gz-gui/pull/96)

### Gazebo Gui 3.2.0 (2020-06-22)

1. Sort GUI plugins alphabetically
    * [Pull request 71](https://github.com/gazebosim/gz-gui/pull/71)

1. Add color palette to plugin settings to change background color
    * [Pull request 62](https://github.com/gazebosim/gz-gui/pull/62)

1. Close plugin menu after a plugin has been added
    * [Pull request 60](https://github.com/gazebosim/gz-gui/pull/60)

1. Resize undocked plugins by dragging corners
    * [Pull request 78](https://github.com/gazebosim/gz-gui/pull/78)

1. Move each plugin to its own directory
    * [Pull request 64](https://github.com/gazebosim/gz-gui/pull/64)

1. Add Key Publisher plugin
    * [Pull request 81](https://github.com/gazebosim/gz-gui/pull/81)

1. GitHub migration
    * [Pull request 46](https://github.com/gazebosim/gz-gui/pull/46)
    * [Pull request 47](https://github.com/gazebosim/gz-gui/pull/47)
    * [Pull request 56](https://github.com/gazebosim/gz-gui/pull/56)
    * [Pull request 57](https://github.com/gazebosim/gz-gui/pull/57)

### Gazebo Gui 3.1.0 (2020-03-27)

1. Remove old ign-gui0 stuff
    * [BitBucket pull request 248](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/248)

1. Fix homebrew warnings
    * [BitBucket pull request 249](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/249)

1. Add space for pausing sim
    * [BitBucket pull request 245](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/245)

1. Prettify plugin menu
    * [BitBucket pull request 231](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/231)

1. Expose QML context to plugins
    * [BitBucket pull request 244](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/244)

1. Fix changing topics in ImageDisplay
    * [BitBucket pull request 224](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/224)

1. Made image display responsive.
    * [BitBucket pull request 223](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/223)

1. Downstream applications can override icon using MainWindow::setIcon once they get the window.
    * [BitBucket pull request 222](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/222)

1. Add support for visualizing int 16 bit images in ImageDisplay
    * [BitBucket pull request 241](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/241)

1. Example opening a dialog and accessing main window from plugin
    * [BitBucket pull request 242](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/242)

1. Example composing one plugin with multiple QML files
    * [BitBucket pull request 235](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/235)

1. Protect against null tinyxml GetText
    * [BitBucket pull request 236](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/236)

1. Example using Ign\*.qml components
    * [BitBucket pull request 240](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/240)

1. Config tutorial
    * [BitBucket pull request 233](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/233)

1. Add copyright to all QML files
    * [BitBucket pull request 232](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/232)

1. Documentation updates
    * [BitBucket pull request 229](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/229)
    * [BitBucket pull request 228](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/228)
    * [BitBucket pull request 230](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/230)

### Gazebo Gui 3.0.0 (2019-12-10)

1. Depend on ign-transport8 and ign-msgs5
    * [BitBucket pull request 225](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/225)

1. Depend on ign-rendering3
    * [BitBucket pull request 221](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/221)

## Gazebo Gui 2

### Gazebo Gui 2.3.3 (2020-08-31)

1. Fix running plugins with bad world names.
    * [Pull request 111](https://github.com/gazebosim/gz-gui/pull/111)

1. Disable more tests known to fail on macOS and Windows.
    * [Pull request 112](https://github.com/gazebosim/gz-gui/pull/112)

### Gazebo Gui 2.3.2 (2020-08-19)

1. Revert pull request #107
    * [Pull request 110](https://github.com/gazebosim/gz-gui/pull/110)

### Gazebo Gui 2.3.1 (2020-08-17)

1. Fix running plugins with bad world name
    * [Pull request 107](https://github.com/gazebosim/gz-gui/pull/107)

### Gazebo Gui 2.3.0 (2020-08-11)

1. Add shift + drag to rotate camera
    * [Pull request 96](https://github.com/gazebosim/gz-gui/pull/96)

1. Scroll bar adjustment
    * [Pull request 97](https://github.com/gazebosim/gz-gui/pull/97)

1. Minimize GUI plugin
    * [Pull request 84](https://github.com/gazebosim/gz-gui/pull/84)
    * [Pull request 99](https://github.com/gazebosim/gz-gui/pull/99)

1. Fix tests on actions
    * [Pull request 98](https://github.com/gazebosim/gz-gui/pull/98)

1. Disable failing tests
    * [Pull request 76](https://github.com/gazebosim/gz-gui/pull/76)

1. Use world name in default topics
    * [Pull request 104](https://github.com/gazebosim/gz-gui/pull/104)

### Gazebo Gui 2.2.0 (2020-06-17)

1. Sort GUI plugins alphabetically
    * [Pull request 71](https://github.com/gazebosim/gz-gui/pull/71)

1. Add color palette to plugin settings to change background color
    * [Pull request 62](https://github.com/gazebosim/gz-gui/pull/62)

1. Close plugin menu after a plugin has been added
    * [Pull request 60](https://github.com/gazebosim/gz-gui/pull/60)

1. Resize undocked plugins by dragging corners
    * [Pull request 78](https://github.com/gazebosim/gz-gui/pull/78)

1. Move each plugin to its own directory
    * [Pull request 64](https://github.com/gazebosim/gz-gui/pull/64)

1. GitHub migration
    * [Pull request 46](https://github.com/gazebosim/gz-gui/pull/46)
    * [Pull request 56](https://github.com/gazebosim/gz-gui/pull/56)

1. Fix homebrew warnings
    * [BitBucket pull request 249](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/249)


### Gazebo Gui 2.1.0 (2020-03-04)

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

### Gazebo Gui 2.0.1

1. Made image display responsive.
    * [BitBucket pull request 223](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/223)

1. Downstream applications can override this using MainWindow::setIcon once they get the window.
    * [BitBucket pull request 222](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/222)

### Gazebo Gui 2.0.0

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

## Gazebo Gui 1

### Gazebo Gui 1.X.X

1. Move files not ported to v1 to a separate dir for clarity.
    * [BitBucket pull request 196](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/196)

1. Get ign-gui plugin interface in case there are others.
    * [BitBucket pull request 186](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/186)

### Gazebo Gui 1.0.0
