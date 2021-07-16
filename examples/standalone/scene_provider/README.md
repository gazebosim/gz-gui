## Scene provider

This example demonstrates how to populate and update a 3D scene using
Ignition Transport.

This example is meant to be used with `examples/config/scened.config`, which
loads the `Scene3D` plugin to create the scene, and the `TransportSceneManager`
plugin to update the scene using Ignition Transport.

## Build

```
cd examples/standalone/scene_provider
mkdir build
cd build
cmake ..
make
```

## Run

In one terminal, start the scene provider:

```
cd examples/standalone/scene_provider/build
./scene_provider
```

On another terminal, start the example config:

```
ign gui -c examples/config/scene3d.config
```

You should see a black box moving around the scene.

## Testing other plugins

### Camera tracking

Some commands to test camera tracking with this demo:

Move to box:

```
ign service -s /gui/move_to --reqtype ignition.msgs.StringMsg --reptype ignition.msgs.Boolean --timeout 2000 --req 'data: "box_model"'
```

Echo camera pose:

```
ign topic -e -t /gui/camera/pose
```

Follow box:

```
ign service -s /gui/follow --reqtype ignition.msgs.StringMsg --reptype ignition.msgs.Boolean --timeout 2000 --req 'data: "box_model"'
```

Update follow offset:

```
ign service -s /gui/follow/offset --reqtype ignition.msgs.Vector3d --reptype ignition.msgs.Boolean --timeout 2000 --req 'x: 5, y: 5, z: 5'
```
