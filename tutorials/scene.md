\page scene 3D Scene

## Overview

Applications built on top of Ignition GUI can leverage Ignition Rendering
to display interactive 3D scenes. This tutorial will help application developers
leverage Ignition GUI's plugins to quickly develop plugins that interact with
the 3D scene.

> This tutorial applies for Ignition GUI version 6 (Fortress) and higher.

## Minimal scene

Ignition GUI ships with the `MinimalScene` plugin, which instantiates a 3D
scene and provides orbit controls, but doesn't do much else. Actions such as
adding, modifying and removing rendering elements from the scene must be
performed by other plugins that work alongside the minimal scene.

Each application will have a different way of updating the 3D scene. For example,
[Ignition Gazebo](https://ignitionrobotics.org/libs/gazebo) updates the scene
based on its entities and components, and
[Ignition RViz](https://github.com/ignitionrobotics/ign-rviz/)
updates the scene based on ROS 2 messages. Each of these applications provides
custom plugins that update the 3D scene through events and the render thread.

Ignition GUI ships with a plugin that updates the scene based on Ignition
Transport messages, the `ignition::gui::plugins::TransportSceneManager`.
Applications can use that directly, or use it as inspiration for developing
their own scene managers.

## Transport scene manager example

Let's start by loading `MinimalScene` together with `TransportSceneManager`.
We'll use a simple program to send messages to `TransportSceneManager`, which
will process those and update the 3D scene, and then the `MinimalScene` will
paint the scene to the window.

Follow the instructions in the
[scene_provider example](https://github.com/ignitionrobotics/ign-gui/tree/main/examples/standalone/scene_provider)
and see visuals being added and moved on the 3D scene.

### Getting the scene

After the `MinimalScene` instantiates an `ignition::rendering::Scene` pointer,
any plugin can get it using the rendering engine's singleton. The
`ignition::rendering::sceneFromFirstRenderEngine()`
function is a convenient way of getting the scene for applications that
have a single scene loaded.

The `TransportSceneManager` plugin, for example, attempts to get the scene at
every iteration until it succeeds. This way, it can find a 3D scene even if
`MinimalScene` is loaded afterwards.

### Render thread

Rendering operations aren't thread-safe. To make sure there are no race
conditions, all rendering operations should happen in the same thread, the
"render thread". In order to access that thread from a custom plugin, it's
necessary to listen to the `ignition::gui::events::Render` event, which is
emitted by the `MinimalScene`.

See how the `TransportSceneManager` installs an event filter with
`installEventFilter`, registers an `eventFilter` callback and performs all
rendering operations within the `OnRender` function.

