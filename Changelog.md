## Gazebo GUI 7

### Gazebo GUI 7.X.X (202X-XX-XX)

### Gazebo GUI 7.1.0 (2023-03-10)


1. Set View Camera controller from plugin configuration
    * [Pull request #506](https://github.com/gazebosim/gz-gui/pull/506)

1. Add service for configuring view control sensitivity
    * [Pull request #504](https://github.com/gazebosim/gz-gui/pull/504)

1. Fix large / unexpected camera movements
    * [Pull request #502](https://github.com/gazebosim/gz-gui/pull/502)

1. Added Reset button to world_control
    * [Pull request #476](https://github.com/gazebosim/gz-gui/pull/476)
    * [Pull request #505](https://github.com/gazebosim/gz-gui/pull/505)

1. Add view control reference visual
    * [Pull request #500](https://github.com/gazebosim/gz-gui/pull/500)

1. Tries to fix a scenario when a user sends a malformed point cloud
    * [Pull request #494](https://github.com/gazebosim/gz-gui/pull/494)

1. Fix mistaken dialog error message
    * [Pull request #472](https://github.com/gazebosim/gz-gui/pull/472)

1. Replace color and pose for GridConfig with GzColor and GzPose
    * [Pull request #463](https://github.com/gazebosim/gz-gui/pull/463)

1. Replace pose in Grid3d with GzPose
    * [Pull request #461](https://github.com/gazebosim/gz-gui/pull/461)

1. Remove redundant namespace references
    * [Pull request #460](https://github.com/gazebosim/gz-gui/pull/460)

1. Update codeowners
    * [Pull request #465](https://github.com/gazebosim/gz-gui/pull/465)

1. Fix warnings on Jammy
    * [Pull request #464](https://github.com/gazebosim/gz-gui/pull/464)

1. GzColor ColorDialogue fix
    * [Pull request #459](https://github.com/gazebosim/gz-gui/pull/459)

1. Fix attribute update when <dialog> isn't present
    * [Pull request #455](https://github.com/gazebosim/gz-gui/pull/455)

1. Add plotting to common widget pose
    * [Pull request #439](https://github.com/gazebosim/gz-gui/pull/439)

1. Add test for Plugin::ConfigStr()
    * [Pull request #447](https://github.com/gazebosim/gz-gui/pull/447)

### Gazebo GUI 7.0.0 (2022-09-XX)

1. Use RTLD_NODELETE=true when loading libraries
    * [Pull request #469](https://github.com/gazebosim/gz-gui/pull/469)

1. Update cmd/CMakeLists to conform with all other gz libraries
    * [Pull request #478](https://github.com/gazebosim/gz-gui/pull/478)

1. Clean up headers
    * [Pull request #490](https://github.com/gazebosim/gz-gui/pull/490)
    * [Pull request #486](https://github.com/gazebosim/gz-gui/pull/486)
    * [Pull request #479](https://github.com/gazebosim/gz-gui/pull/479)

1. Fixed topic echo test
    * [Pull request #448](https://github.com/gazebosim/gz-gui/pull/448)

1. Use stepping field in message
    * [Pull request #441](https://github.com/gazebosim/gz-gui/pull/441)

1. Teleop: Refactor and support vertical
    * [Pull request #440](https://github.com/gazebosim/gz-gui/pull/440)

1. Remove deprecated image.data API
    * [Pull request #430](https://github.com/gazebosim/gz-gui/pull/430)

1. Remove deprecated Scene3d plugin
    * [Pull request #438](https://github.com/gazebosim/gz-gui/pull/438)

1. Add common widget for vector3
    * [Pull request #427](https://github.com/gazebosim/gz-gui/pull/427)

1. Allow Dialogs to have a MainWindow independent config
    * [Pull request #450](https://github.com/gazebosim/gz-gui/pull/450)
    * [Pull request #442](https://github.com/gazebosim/gz-gui/pull/442)
    * [Pull request #418](https://github.com/gazebosim/gz-gui/pull/418)

1. Add common widget for pose
    * [Pull request #431](https://github.com/gazebosim/gz-gui/pull/431)
    * [Pull request #424](https://github.com/gazebosim/gz-gui/pull/424)

1. Example running a dialog before the main window
    * [Pull request #407](https://github.com/gazebosim/gz-gui/pull/407)

1. Add .cc extension to suppress CMake warnings
    * [Pull request #428](https://github.com/gazebosim/gz-gui/pull/428)

1. Update GoogleTest to latest version
    * [Pull request #426](https://github.com/gazebosim/gz-gui/pull/426)

1. Common widget GzColor
    * [Pull request #410](https://github.com/gazebosim/gz-gui/pull/410)

1. Make display tests more robust
    * [Pull request #419](https://github.com/gazebosim/gz-gui/pull/419)

1. Bash completion for flags
    * [Pull request #392](https://github.com/gazebosim/gz-gui/pull/392)

1. Disable failing test on Citadel
    * [Pull request #420](https://github.com/gazebosim/gz-gui/pull/420)
    * [Pull request #416](https://github.com/gazebosim/gz-gui/pull/416)

1. Search menu keyboard control
    * [Pull request #405](https://github.com/gazebosim/gz-gui/pull/405)
    * [Pull request #403](https://github.com/gazebosim/gz-gui/pull/403)

1. User camera FOV control in SDF files
    * [Pull request #400](https://github.com/gazebosim/gz-gui/pull/400)

1. Add config relative path environment variable
    * [Pull request #386](https://github.com/gazebosim/gz-gui/pull/386)

1. Add repo specific issue templates
    * [Pull request #393](https://github.com/gazebosim/gz-gui/pull/393)

1. Sort plugin list in alphabetical order (including when filtering)
    * [Pull request #387](https://github.com/gazebosim/gz-gui/pull/387)

1. Rename Ignition to Gazebo
    * [Pull request #449](https://github.com/gazebosim/gz-gui/pull/449)
    * [Pull request #446](https://github.com/gazebosim/gz-gui/pull/446)
    * [Pull request #437](https://github.com/gazebosim/gz-gui/pull/437)
    * [Pull request #435](https://github.com/gazebosim/gz-gui/pull/435)
    * [Pull request #433](https://github.com/gazebosim/gz-gui/pull/433)
    * [Pull request #425](https://github.com/gazebosim/gz-gui/pull/425)
    * [Pull request #422](https://github.com/gazebosim/gz-gui/pull/422)
    * [Pull request #417](https://github.com/gazebosim/gz-gui/pull/417)
    * [Pull request #414](https://github.com/gazebosim/gz-gui/pull/414)
    * [Pull request #413](https://github.com/gazebosim/gz-gui/pull/413)
    * [Pull request #412](https://github.com/gazebosim/gz-gui/pull/412)
    * [Pull request #401](https://github.com/gazebosim/gz-gui/pull/401)
    * [Pull request #398](https://github.com/gazebosim/gz-gui/pull/398)
    * [Pull request #395](https://github.com/gazebosim/gz-gui/pull/395)
    * [Pull request #384](https://github.com/gazebosim/gz-gui/pull/384)

1. set cameraPassCountPerGpuFlush to 6
    * [Pull request #381](https://github.com/gazebosim/gz-gui/pull/381)

1. Add Ubuntu Jammy CI
    * [Pull request #374](https://github.com/gazebosim/gz-gui/pull/374)

1. Avoid nullptr access in MinimalScene
    * [Pull request #380](https://github.com/gazebosim/gz-gui/pull/380)

1. Added array to snackbar qml
    * [Pull request #370](https://github.com/gazebosim/gz-gui/pull/370)

1. Fix some Qt warnings
    * [Pull request #376](https://github.com/gazebosim/gz-gui/pull/376)

1. Fix nondiscard warnings with lock_guard
    * [Pull request #373](https://github.com/gazebosim/gz-gui/pull/373)

1. Remove Bionic from future releases (Garden+)
    * [Pull request #371](https://github.com/gazebosim/gz-gui/pull/371)

1. Use gz-rendering's screenToScene functions
    * [Pull request #372](https://github.com/gazebosim/gz-gui/pull/372)

1. Added Snackbar qtquick object
    * [Pull request #369](https://github.com/gazebosim/gz-gui/pull/369)

1. Fix menu scrolling when a new plugin is added
    * [Pull request #368](https://github.com/gazebosim/gz-gui/pull/368)

1. Add Metal support to MinimalScene and Qt Application
    * [Pull request #323](https://github.com/gazebosim/gz-gui/pull/323)

1. Cleanup GUI plugin menu
    * [Pull request #366](https://github.com/gazebosim/gz-gui/pull/366)
    * [Pull request #365](https://github.com/gazebosim/gz-gui/pull/365)
    * [Pull request #364](https://github.com/gazebosim/gz-gui/pull/364)
    * [Pull request #363](https://github.com/gazebosim/gz-gui/pull/363)
    * [Pull request #362](https://github.com/gazebosim/gz-gui/pull/362)

1. Attempt at fixing Helpers test on macOS
    * [Pull request #361](https://github.com/gazebosim/gz-gui/pull/361)

1. Removed unused code in MinimalScene plugin
    * [Pull request #359](https://github.com/gazebosim/gz-gui/pull/359)

1. Point cloud visualization plugin
    * [Pull request #346](https://github.com/gazebosim/gz-gui/pull/346)

1. Set per-point color on markers
    * [Pull request #318](https://github.com/gazebosim/gz-gui/pull/318)

1. Bumps in garden
    * [Pull request #388](https://github.com/gazebosim/gz-gui/pull/388)
    * [Pull request #385](https://github.com/gazebosim/gz-gui/pull/385)
    * [Pull request #343](https://github.com/gazebosim/gz-gui/pull/343)
    * [Pull request #327](https://github.com/gazebosim/gz-gui/pull/327)
    * [Pull request #313](https://github.com/gazebosim/gz-gui/pull/313)
    * [Pull request #301](https://github.com/gazebosim/gz-gui/pull/301)    

1. Added macOS source code installation
    * [Pull request #297](https://github.com/gazebosim/gz-gui/pull/297)


## Gazebo GUI 6

### Gazebo GUI 6.7.0 (2022-12-02)

1. Set View Camera controller from plugin configuration
    * [Pull request #506](https://github.com/gazebosim/gz-gui/pull/506)

1. Add service for configuring view control sensitivity
    * [Pull request #504](https://github.com/gazebosim/gz-gui/pull/504)

1. Fix large / unexpected camera movements
    * [Pull request #502](https://github.com/gazebosim/gz-gui/pull/502)

1. Add view control reference visual
    * [Pull request #500](https://github.com/gazebosim/gz-gui/pull/500)

### Gazebo GUI 6.6.1 (2022-08-17)

1. Fix mistaken dialog error message
    * [Pull request #472](https://github.com/gazebosim/gz-gui/pull/472)

1. Replace color and pose for `GridConfig` with `GzColor` and `GzPose`
    * [Pull request #463](https://github.com/gazebosim/gz-gui/pull/463)

1. Replace pose in `Grid3d` with `GzPose`
    * [Pull request #461](https://github.com/gazebosim/gz-gui/pull/461)

1. Remove redundant namespace references
    * [Pull request #460](https://github.com/gazebosim/gz-gui/pull/460)

1. Update codeowners
    * [Pull request #465](https://github.com/gazebosim/gz-gui/pull/465)

1. Fix warnings on Jammy
    * [Pull request #464](https://github.com/gazebosim/gz-gui/pull/464)

### Gazebo GUI 6.6.0 (2022-08-10)

1. GzColor ColorDialogue fix
    * [Pull request #459](https://github.com/gazebosim/gz-gui/pull/459)

1. Fix attribute update when `<dialog>` isn't present
    * [Pull request #455](https://github.com/gazebosim/gz-gui/pull/455)

1. Add plotting to common widget pose
    * [Pull request #439](https://github.com/gazebosim/gz-gui/pull/439)

1. Add test for Plugin::ConfigStr()
    * [Pull request #447](https://github.com/gazebosim/gz-gui/pull/447)

### Gazebo GUI 6.5.0 (2022-08-02)

1. `Teleop`: Refactor and support vertical
    * [Pull request #440](https://github.com/gazebosim/gz-gui/pull/440)

1. Change `IGN_DESIGNATION` to `GZ_DESIGNATION`
    * [Pull request #437](https://github.com/gazebosim/gz-gui/pull/437)

1. Ignition -> Gazebo
    * [Pull request #435](https://github.com/gazebosim/gz-gui/pull/435)

1. Allow Dialogs to have a MainWindow independent config
    * [Pull request #418](https://github.com/gazebosim/gz-gui/pull/418)
    * [Pull request #442](https://github.com/gazebosim/gz-gui/pull/442)

1. Add common widget for Vector3
    * [Pull request #427](https://github.com/gazebosim/gz-gui/pull/427)

1. Add common widget for Pose
    * [Pull request #424](https://github.com/gazebosim/gz-gui/pull/424)
    * [Pull request #431](https://github.com/gazebosim/gz-gui/pull/431)
    * [Pull request #450](https://github.com/gazebosim/gz-gui/pull/450)

1. Add common widget for Color
    * [Pull request #410](https://github.com/gazebosim/gz-gui/pull/410)

1. Example running a dialog before the main window
    * [Pull request #407](https://github.com/gazebosim/gz-gui/pull/407)

1. Test fixes and improvements
    * [Pull request #420](https://github.com/gazebosim/gz-gui/pull/420)
    * [Pull request #419](https://github.com/gazebosim/gz-gui/pull/419)
    * [Pull request #448](https://github.com/gazebosim/gz-gui/pull/448)
    * [Pull request #416](https://github.com/gazebosim/gz-gui/pull/416)
    * [Pull request #374](https://github.com/gazebosim/gz-gui/pull/374)

1. Bash completion for flags
    * [Pull request #392](https://github.com/gazebosim/gz-gui/pull/392)

1. Shortcut to search and auto highlight search bar
    * [Pull request #405](https://github.com/gazebosim/gz-gui/pull/405)

1. Search menu keyboard control
    * [Pull request #403](https://github.com/gazebosim/gz-gui/pull/403)

1. User camera FOV control in SDF files
    * [Pull request #400](https://github.com/gazebosim/gz-gui/pull/400)
    * [Pull request #434](https://github.com/gazebosim/gz-gui/pull/434)

1. Add config relative path environment variable
    * [Pull request #386](https://github.com/gazebosim/gz-gui/pull/386)

1. Add repo specific issue templates
    * [Pull request #393](https://github.com/gazebosim/gz-gui/pull/393)

1. Sort plugin list in alphabetical order (including when filtering)
    * [Pull request #387](https://github.com/gazebosim/gz-gui/pull/387)

1. Set `cameraPassCountPerGpuFlush` to 6
    * [Pull request #381](https://github.com/gazebosim/gz-gui/pull/381)

1. Avoid nullptr access in MinimalScene
    * [Pull request #380](https://github.com/gazebosim/gz-gui/pull/380)

1. Fix some Qt warnings
    * [Pull request #376](https://github.com/gazebosim/gz-gui/pull/376)

1. Use `gz-rendering`'s `screenToScene` functions
    * [Pull request #372](https://github.com/gazebosim/gz-gui/pull/372)

1. Added Snackbar qtquick object
    * [Pull request #369](https://github.com/gazebosim/gz-gui/pull/369)
    * [Pull request #370](https://github.com/gazebosim/gz-gui/pull/370)

1. Fix menu scrolling when a new plugin is added
    * [Pull request #368](https://github.com/gazebosim/gz-gui/pull/368)

1. `TransportSceneManager`: Prevent freeze when inserted from menu
    * [Pull request #365](https://github.com/gazebosim/gz-gui/pull/365)

1. Populate plugins that were empty
    * [Pull request #366](https://github.com/gazebosim/gz-gui/pull/366)

1. Prevent `MinimalScene` 💥 if another scene is already loaded
    * [Pull request #364](https://github.com/gazebosim/gz-gui/pull/364)

1. Improve `KeyPublisher`'s usability
    * [Pull request #362](https://github.com/gazebosim/gz-gui/pull/362)

1. Removed unused code in `MinimalScene` plugin
    * [Pull request #359](https://github.com/gazebosim/gz-gui/pull/359)

### Gazebo GUI 6.4.0 (2022-02-01)

1. Improve inserting WorldControl and WorldStats from menu
    * [Pull request #349](https://github.com/gazebosim/gz-gui/pull/349)

1. Added a button that allows shutting down both the client and server.
    * [Pull request #335](https://github.com/gazebosim/gz-gui/pull/335)

1. Prevent Scene3D 💥 if another scene is already loaded
    * [Pull request #347](https://github.com/gazebosim/gz-gui/pull/347)

1. Add project() to examples and remove hard-dependency on Ogre1
    * [Pull request #345](https://github.com/gazebosim/gz-gui/pull/345)

### Gazebo GUI 6.3.0 (2022-01-06)

1. NavSat (GPS) map plugin
    * [Pull request #342](https://github.com/gazebosim/gz-gui/pull/342)

### Gazebo GUI 6.2.0 (2021-12-08)

1. Grid config: set values from startup and improve layout
    * [Pull request #324](https://github.com/gazebosim/gz-gui/pull/324)

1. Fix codecheck
    * [Pull request #332](https://github.com/gazebosim/gz-gui/pull/332)
    * [Pull request #329](https://github.com/gazebosim/gz-gui/pull/329)

1. Offer a way to disable warnings on marker manager
    * [Pull request #326](https://github.com/gazebosim/gz-gui/pull/326)

1. Add PreRender event to MinimalScene
    * [Pull request #325](https://github.com/gazebosim/gz-gui/pull/325)

1. Use qmldir to define QML module with IgnSpinBox
    * [Pull request #319](https://github.com/gazebosim/gz-gui/pull/319)

1. Fix TopicEcho plugin message display
    * [Pull request #322](https://github.com/gazebosim/gz-gui/pull/322)

1. Don't set visual scale for point markers
    * [Pull request #321](https://github.com/gazebosim/gz-gui/pull/321)

1. Set marker point size from message
    * [Pull request #317](https://github.com/gazebosim/gz-gui/pull/317)

1. Don't crash if a plugin has invalid QML
    * [Pull request #315](https://github.com/gazebosim/gz-gui/pull/315)

1. Added log storing for gz-gui
    * [Pull request #272](https://github.com/gazebosim/gz-gui/pull/272)

### Gazebo GUI 6.1.0 (2021-11-05)

1. Improved doxygen
    * [Pull request #275](https://github.com/gazebosim/gz-gui/pull/275)

1. Fix mimimal scene deadlock on shutdown
    * [Pull request #300](https://github.com/gazebosim/gz-gui/pull/300)

1. Fix memory leak
    * [Pull request #287](https://github.com/gazebosim/gz-gui/pull/287)

1. Set near/far camera clipping distance
    * [Pull request #309](https://github.com/gazebosim/gz-gui/pull/309)

1. Support emitting an event on play/pause/step
    * [Pull request #306](https://github.com/gazebosim/gz-gui/pull/306)

### Gazebo GUI 6.0.0 (2021-09-XX)

1. Add a filter to the plugin menu
    * [Pull request #277](https://github.com/gazebosim/gz-gui/pull/277)

1. New events

    1. Drop
        * [Pull request #282](https://github.com/gazebosim/gz-gui/pull/282)

    1. SpawnCloneFromName
        * [Pull request #278](https://github.com/gazebosim/gz-gui/pull/278)

    1. HoverOnScene
        * [Pull request #273](https://github.com/gazebosim/gz-gui/pull/273)

    1. Mouse Events based on ignition::common
        * [Pull request #228](https://github.com/gazebosim/gz-gui/pull/228)

1. New plugins

    1. Interactive view control
        * [Pull request #231](https://github.com/gazebosim/gz-gui/pull/231)

    1. Marker Manager
        * [Pull request #247](https://github.com/gazebosim/gz-gui/pull/247)

    1. Moved tape measure and grid config from gz-sim to gz-gui
        * [Pull request #236](https://github.com/gazebosim/gz-gui/pull/236)

1. PIMPL GUI events
    * [Pull request #253](https://github.com/gazebosim/gz-gui/pull/253)

1. Added winID to fix second windows in OGRE2.2
    * [Pull request #292](https://github.com/gazebosim/gz-gui/pull/292)

1. Added camera tracking
    * [Pull request #226](https://github.com/gazebosim/gz-gui/pull/226)
    * [Pull request #254](https://github.com/gazebosim/gz-gui/pull/254)

1. Split transport scene manager into a plugin outside Scene3D. Adds MinimalScene
    * [Pull request #221](https://github.com/gazebosim/gz-gui/pull/221)
    * [Pull request #284](https://github.com/gazebosim/gz-gui/pull/284)
    * [Pull request #285](https://github.com/gazebosim/gz-gui/pull/285)
    * [Pull request #268](https://github.com/gazebosim/gz-gui/pull/268)
    * [Pull request #224](https://github.com/gazebosim/gz-gui/pull/224)
    * [Pull request #282](https://github.com/gazebosim/gz-gui/pull/282)
    * [Pull request #273](https://github.com/gazebosim/gz-gui/pull/273)
    * [Pull request #286](https://github.com/gazebosim/gz-gui/pull/286)
    * [Pull request #295](https://github.com/gazebosim/gz-gui/pull/295)

1. Remove deprecations: tock
    * [Pull request #243](https://github.com/gazebosim/gz-gui/pull/243)

1. Depend on gz-msgs8, gz-transport11, gz-rendering6
    * [Pull request #210](https://github.com/gazebosim/gz-gui/pull/210)

1. Infrastructure
    * [Pull request #207](https://github.com/gazebosim/gz-gui/pull/207)
    * [Pull request #219](https://github.com/gazebosim/gz-gui/pull/219)
    * [Pull request #266](https://github.com/gazebosim/gz-gui/pull/266)
    * [Pull request #274](https://github.com/gazebosim/gz-gui/pull/274)

## Gazebo GUI 5

### Gazebo GUI 5.5.0 (2022-03-28)

1. Fix some Qt warnings
    * [Pull request #376](https://github.com/gazebosim/gz-gui/pull/376)

1. Added Snackbar QtQuick object
    * [Pull request #369](https://github.com/gazebosim/gz-gui/pull/369)
    * [Pull request #370](https://github.com/gazebosim/gz-gui/pull/370)

1. Fix menu scrolling when a new plugin is added
    * [Pull request #368](https://github.com/gazebosim/gz-gui/pull/368)

1. Improve KeyPublisher's usability
    * [Pull request #362](https://github.com/gazebosim/gz-gui/pull/362)

1. Backport GridConfig improvements to Grid3D
    * [Pull request #363](https://github.com/gazebosim/gz-gui/pull/363)

### Gazebo GUI 5.4.0 (2022-01-31)

1. Improve inserting WorldControl and WorldStats from menu
    * [Pull request #349](https://github.com/gazebosim/gz-gui/pull/349)

1. Added a button that allows shutting down both the client and server.
    * [Pull request #335](https://github.com/gazebosim/gz-gui/pull/335)

1. Prevent Scene3D 💥 if another scene is already loaded
    * [Pull request #347](https://github.com/gazebosim/gz-gui/pull/347)

1. Add project() to examples and remove hard-dependency on Ogre1
    * [Pull request #345](https://github.com/gazebosim/gz-gui/pull/345)

1. Fix codecheck
    * [Pull request #329](https://github.com/gazebosim/gz-gui/pull/329)

1. Use qmldir to define QML module with IgnSpinBox
    * [Pull request #319](https://github.com/gazebosim/gz-gui/pull/319)

1. Fix TopicEcho plugin message display
    * [Pull request #322](https://github.com/gazebosim/gz-gui/pull/322)

1. Don't crash if a plugin has invalid QML
    * [Pull request #315](https://github.com/gazebosim/gz-gui/pull/315)

1. Added log storing for gz-gui
    * [Pull request #272](https://github.com/gazebosim/gz-gui/pull/272)


### Gazebo GUI 5.3.0 (2021-10-13)

1. Improved doxygen
    * [Pull request #275](https://github.com/gazebosim/gz-gui/pull/275)

1. Fix memory leak
    * [Pull request #287](https://github.com/gazebosim/gz-gui/pull/287)

1. Add a filter to the plugin menu
    * [Pull request #277](https://github.com/gazebosim/gz-gui/pull/277)

1. 👩‍🌾 Remove bitbucket-pipelines.yml
    * [Pull request #274](https://github.com/gazebosim/gz-gui/pull/274)

1. Fix doc build error
    * [Pull request #266](https://github.com/gazebosim/gz-gui/pull/266)

### Gazebo GUI 5.2.0 (2021-07-27)

1. New teleop plugin implementation.
    * [Pull request #245](https://github.com/gazebosim/gz-gui/pull/245)

1. Fix plugin added signal, add PluginByName
    * [Pull request #249](https://github.com/gazebosim/gz-gui/pull/249)

1. Fixed tests by passing valid argv
    * [Pull request #244](https://github.com/gazebosim/gz-gui/pull/244)

1. Screenshot plugin fixed dbg message
    * [Pull request #246](https://github.com/gazebosim/gz-gui/pull/246)

1. Detect gz instead of using cmake module to check for gz-tools
    * [Pull request #240](https://github.com/gazebosim/gz-gui/pull/240)

### Gazebo GUI 5.1.0 (2021-06-23)

1. Depend on common 4.1 and rendering 5.1
    * [Pull request #238](https://github.com/gazebosim/gz-gui/pull/238)

1. Update codeowners
    * [Pull request #232](https://github.com/gazebosim/gz-gui/pull/232)

1. Confirmation dialog when closing main window
    * [Pull request #225](https://github.com/gazebosim/gz-gui/pull/225)

1. Avoid Grid3D crash
    * [Pull request #227](https://github.com/gazebosim/gz-gui/pull/227)

1. Emit more events from Scene3D
    * [Pull request #213](https://github.com/gazebosim/gz-gui/pull/213)

1. Removed duplicated code with `rendering::sceneFromFirstRenderEngine`
    * [Pull request #223](https://github.com/gazebosim/gz-gui/pull/223)

1. Remove `tools/code_check` and update codecov
    * [Pull request #222](https://github.com/gazebosim/gz-gui/pull/222)

1. Refactor Image Display to use ConvertToRGB function
    * [Pull request #212](https://github.com/gazebosim/gz-gui/pull/212)

1. Fixed material specular in Scene3D
    * [Pull request #218](https://github.com/gazebosim/gz-gui/pull/218)

1. Updated scale for capsule in Scene3D
    * [Pull request #215](https://github.com/gazebosim/gz-gui/pull/215)

1. Support Capsule and Ellipsoid in Scene3D
    * [Pull request #214](https://github.com/gazebosim/gz-gui/pull/214)

1. `check_test_ran.py`: remove grep/xsltproc
    * [Pull request #203](https://github.com/gazebosim/gz-gui/pull/203)

### Gazebo GUI 5.0.0 (2021-03-31)

1. Bump in edifice: gz-common4
    * [Pull request #169](https://github.com/gazebosim/gz-gui/pull/169)

1. Bump in edifice: gz-msgs7 and gz-transport10
    * [Pull request #167](https://github.com/gazebosim/gz-gui/pull/167)

1. Bump in edifice: gz-rendering5
    * [Pull request #141](https://github.com/gazebosim/gz-gui/pull/141)

1. Documentation updates
    * [Pull request #202](https://github.com/gazebosim/gz-gui/pull/202)
    * [Pull request #204](https://github.com/gazebosim/gz-gui/pull/204)

## Gazebo GUI 4

### Gazebo GUI 4.X.X

### Gazebo GUI 4.X.X (20XX-XX-XX)


### Gazebo GUI 4.5.0 (2021-07-26)

1. New teleop plugin implementation.
    * [Pull request #245](https://github.com/gazebosim/gz-gui/pull/245)

1. Fix codeowners
    * [Pull request #251](https://github.com/gazebosim/gz-gui/pull/251)

1. Fix plugin added signal, add PluginByName
    * [Pull request #249](https://github.com/gazebosim/gz-gui/pull/249)

1. Fixed tests by passing valid argv
    * [Pull request #244](https://github.com/gazebosim/gz-gui/pull/244)

1. Screenshot plugin fixed dbg message
    * [Pull request #246](https://github.com/gazebosim/gz-gui/pull/246)

1. Detect gz instead of using cmake module to check for gz-tools
    * [Pull request #240](https://github.com/gazebosim/gz-gui/pull/240)

### Gazebo GUI 4.4.0 (2021-06-21)

1. Bump required gz-rendering version to 4.8
    * [Pull request #234](https://github.com/gazebosim/gz-gui/pull/234)

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

1. Refactor Image Display to use ConvertToRGB function
    * [Pull request #212](https://github.com/gazebosim/gz-gui/pull/212)

1. Fixed material specular in scene3D
    * [Pull request #218](https://github.com/gazebosim/gz-gui/pull/218)

1. `check_test_ran.py`: remove grep/xsltproc
    * [Pull request #203](https://github.com/gazebosim/gz-gui/pull/203)

1. Improve the height of plugins in the right split
    * [Pull request #194](https://github.com/gazebosim/gz-gui/pull/194)

1. Scene3D: port mesh material fixes from gz-sim
    * [Pull request #191](https://github.com/gazebosim/gz-gui/pull/191)

### Gazebo GUI 4.3.0 (2021-03-11)

1. Screenshot plugin
    * [Pull request #170](https://github.com/gazebosim/gz-gui/pull/170)

1. Master branch updates
    * [Pull request #187](https://github.com/gazebosim/gz-gui/pull/187)

1. 👩‍🌾 Disable tests that initialize App on macOS: they're all flaky
    * [Pull request #184](https://github.com/gazebosim/gz-gui/pull/184)

1. Remove issue & PR templates
    * [Pull request #181](https://github.com/gazebosim/gz-gui/pull/181)

1. Backport Publisher test from v4
    * [Pull request #173](https://github.com/gazebosim/gz-gui/pull/173)

1. Fix version number
    * [Pull request #179](https://github.com/gazebosim/gz-gui/pull/179)

1. Better error messages when component can't be loaded
    * [Pull request #175](https://github.com/gazebosim/gz-gui/pull/175)

1. Make Grid3D plugin more flexible
    * [Pull request #172](https://github.com/gazebosim/gz-gui/pull/172)

1. Add Windows Installation
    * [Pull request #168](https://github.com/gazebosim/gz-gui/pull/168)

### Gazebo GUI 4.2.0 (2021-02-09)

1. Added issue and PR templates
    * [Pull request 174](https://github.com/gazebosim/gz-gui/pull/174)

1. Visualize single channel 8 bit image data
    * [Pull request 176](https://github.com/gazebosim/gz-gui/pull/176)

### Gazebo GUI 4.1.1 (2021-01-05)

1. Use default value of double ptr argv for CLI
    * [Pull request 165](https://github.com/gazebosim/gz-gui/pull/165)

1. Generate valid topics depending on world name
    * [Pull request 164](https://github.com/gazebosim/gz-gui/pull/164)

### Gazebo GUI 4.1.0 (2020-12-18)

1. Publish plugin API docs
    * [Pull request 128](https://github.com/gazebosim/gz-gui/pull/128)

1. Tutorial tweaks
    * [Pull request 132](https://github.com/gazebosim/gz-gui/pull/132)

1. Floating and standalone plugins respect minimum dimensions
    * [Pull request 135](https://github.com/gazebosim/gz-gui/pull/135)

1. Add scrollable indicator for plugin menu
    * [Pull request 134](https://github.com/gazebosim/gz-gui/pull/134)

1. Add qml-module-qtcharts dependency
    * [Pull request 136](https://github.com/gazebosim/gz-gui/pull/136)

1. Re-enable image.config test
    * [Pull request 140](https://github.com/gazebosim/gz-gui/pull/140)

1. Improve fork experience
    * [Pull request 139](https://github.com/gazebosim/gz-gui/pull/139)

1. Resolve updated codecheck issues
    * [Pull request 144](https://github.com/gazebosim/gz-gui/pull/144)
    * [Pull request 157](https://github.com/gazebosim/gz-gui/pull/157)

1. Port Gazebo GUI events to Gazebo GUI
    * [Pull request 148](https://github.com/gazebosim/gz-gui/pull/148)

1. Change deprecated Qt::MidButton
    * [Pull request 153](https://github.com/gazebosim/gz-gui/pull/153)

1. Add right mouse events and tests
    * [Pull request 154](https://github.com/gazebosim/gz-gui/pull/154)

1. Avoid crash on macOS
    * [Pull request 155](https://github.com/gazebosim/gz-gui/pull/155)

### Gazebo GUI 4.0.0 (2020-09-29)

1. GitHub migration
    * [Pull request 59](https://github.com/gazebosim/gz-gui/pull/59)

1. Fixed ImageDisplay in Ubuntu 20.04
    * [Pull request 88](https://github.com/gazebosim/gz-gui/pull/88)

1. Re-enable publisher test
    * [Pull request 85](https://github.com/gazebosim/gz-gui/pull/85)

1. Disable TopicViewer test on Windows and macOS
    * [Pull request 85](https://github.com/gazebosim/gz-gui/pull/85)

1. Documentation updates
    * [Pull request 106](https://github.com/gazebosim/gz-gui/pull/106)
    * [Pull request 130](https://github.com/gazebosim/gz-gui/pull/130)
    * [Pull request 127](https://github.com/gazebosim/gz-gui/pull/127)

1. Fix qt anchor warnings
    * [Pull request 126](https://github.com/gazebosim/gz-gui/pull/126)

1. Plotting: transport plotting and interface
    * [Pull request 115](https://github.com/gazebosim/gz-gui/pull/115)
    * [Pull request 125](https://github.com/gazebosim/gz-gui/pull/125)
    * [Pull request 124](https://github.com/gazebosim/gz-gui/pull/124)
    * [Pull request 89](https://github.com/gazebosim/gz-gui/pull/89)

1. Use std::chrono instead of common::Time
    * [Pull request 116](https://github.com/gazebosim/gz-gui/pull/116)

1. Depend on QtCharts and change application base class to QApplication
    * [BitBucket pull request 252](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/252)

1. Depend on gz-msgs6 and gz-transport9
    * [Pull request 92](https://github.com/gazebosim/gz-gui/pull/92)

1. Depend on gz-rendering4
    * [BitBucket pull request 243](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/243)

## Gazebo GUI 3

### Gazebo GUI 3.12.0 (2022-11-30)

1. Add degree as an optional unit for rotation in GzPose.
    * [Pull request #475](https://github.com/gazebosim/gz-gui/pull/475)

1. Fix image display test.
    * [Pull request #468](https://github.com/gazebosim/gz-gui/pull/468)

1. Update cmd/CMakeLists to conform with all other gz libraries.
    * [Pull request #478](https://github.com/gazebosim/gz-gui/pull/478)

1. Add key publisher test.
    * [Pull request #477](https://github.com/gazebosim/gz-gui/pull/477)

1. Add pointer check in Application::RemovePlugin.
    * [Pull request #501](https://github.com/gazebosim/gz-gui/pull/501)

1. Ign to gz header migration.
    * [Pull request #466](https://github.com/gazebosim/gz-gui/pull/466)

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

### Gazebo GUI 3.10.0 (2022-07-13)

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

1. Fix ign_TEST
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

### Gazebo GUI 3.9.0 (2022-01-14)

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

1. Added log storing for `gz gui` CLI
    * [Pull request #272](https://github.com/gazebosim/gz-gui/pull/272)

### Gazebo GUI 3.8.0 (2021-10-12)

1. Improved doxygen
    * [Pull request #275](https://github.com/gazebosim/gz-gui/pull/275)

1. Add a filter to the plugin menu
    * [Pull request #277](https://github.com/gazebosim/gz-gui/pull/277)

1. 👩‍🌾 Remove bitbucket-pipelines.yml
    * [Pull request #274](https://github.com/gazebosim/gz-gui/pull/274)

1. Require gz-rendering 3.5
    * [Pull request #264](https://github.com/gazebosim/gz-gui/pull/264)

1. New teleop plugin implementation.
    * [Pull request #245](https://github.com/gazebosim/gz-gui/pull/245)

### Gazebo GUI 3.7.0 (2021-07-14)

1. Fix codeowners
    * [Pull request #251](https://github.com/gazebosim/gz-gui/pull/251)

1. Fix plugin added signal, add PluginByName
    * [Pull request #249](https://github.com/gazebosim/gz-gui/pull/249)

1. Fixed tests by passing valid argv
    * [Pull request #244](https://github.com/gazebosim/gz-gui/pull/244)

1. Screenshot plugin fixed dbg message
    * [Pull request #246](https://github.com/gazebosim/gz-gui/pull/246)

1. Detect gz instead of using cmake module to check for gz-tools
    * [Pull request #240](https://github.com/gazebosim/gz-gui/pull/240)

### Gazebo GUI 3.6.0 (2021-06-17)

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

### Gazebo GUI 3.5.1 (2021-03-18)

1. Scene3D: port mesh material fixes from gz-sim
    * [Pull request #191](https://github.com/gazebosim/gz-gui/pull/191)

1. Improve the height of plugins in the right split
    * [Pull request #194](https://github.com/gazebosim/gz-gui/pull/194)

### Gazebo GUI 3.5.0 (2021-03-10)

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

### Gazebo GUI 3.4.0 (2020-12-10)

1. Publish plugin API docs
    * [Pull request 128](https://github.com/gazebosim/gz-gui/pull/128)

1. Tutorial tweaks
    * [Pull request 132](https://github.com/gazebosim/gz-gui/pull/132)

1. Floating and standalone plugins respect minimum dimensions
    * [Pull request 135](https://github.com/gazebosim/gz-gui/pull/135)

1. Add scrollable indicator for plugin menu
    * [Pull request 134](https://github.com/gazebosim/gz-gui/pull/134)

1. Re-enable image.config test
    * [Pull request 140](https://github.com/gazebosim/gz-gui/pull/140)

1. Improve fork experience
    * [Pull request 139](https://github.com/gazebosim/gz-gui/pull/139)

1. Resolve updated codecheck issues
    * [Pull request 144](https://github.com/gazebosim/gz-gui/pull/144)

1. Port Gazebo GUI events to Gazebo GUI
    * [Pull request 148](https://github.com/gazebosim/gz-gui/pull/148)

1. Change deprecated Qt::MidButton
    * [Pull request 153](https://github.com/gazebosim/gz-gui/pull/153)

1. Add right mouse events and tests
    * [Pull request 154](https://github.com/gazebosim/gz-gui/pull/154)

### Gazebo GUI 3.3.0 (2020-08-31)

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

### Gazebo GUI 3.2.0 (2020-06-22)

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

### Gazebo GUI 3.1.0 (2020-03-27)

1. Remove old gz-gui0 stuff
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

### Gazebo GUI 3.0.0 (2019-12-10)

1. Depend on gz-transport8 and gz-msgs5
    * [BitBucket pull request 225](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/225)

1. Depend on gz-rendering3
    * [BitBucket pull request 221](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/221)

## Gazebo GUI 2

### Gazebo GUI 2.3.3 (2020-08-31)

1. Fix running plugins with bad world names.
    * [Pull request 111](https://github.com/gazebosim/gz-gui/pull/111)

1. Disable more tests known to fail on macOS and Windows.
    * [Pull request 112](https://github.com/gazebosim/gz-gui/pull/112)

### Gazebo GUI 2.3.2 (2020-08-19)

1. Revert pull request #107
    * [Pull request 110](https://github.com/gazebosim/gz-gui/pull/110)

### Gazebo GUI 2.3.1 (2020-08-17)

1. Fix running plugins with bad world name
    * [Pull request 107](https://github.com/gazebosim/gz-gui/pull/107)

### Gazebo GUI 2.3.0 (2020-08-11)

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

### Gazebo GUI 2.2.0 (2020-06-17)

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


### Gazebo GUI 2.1.0 (2020-03-04)

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

### Gazebo GUI 2.0.1

1. Made image display responsive.
    * [BitBucket pull request 223](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/223)

1. Downstream applications can override this using MainWindow::setIcon once they get the window.
    * [BitBucket pull request 222](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/222)

### Gazebo GUI 2.0.0

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

## Gazebo GUI 1

### Gazebo GUI 1.X.X

1. Move files not ported to v1 to a separate dir for clarity.
    * [BitBucket pull request 196](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/196)

1. Get gz-gui plugin interface in case there are others.
    * [BitBucket pull request 186](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-gui/pull-requests/186)

### Gazebo GUI 1.0.0
