## Scene provider

This example demonstrates how to populate and update a 3D scene using
Gazebo Transport.

This example is meant to be used with `examples/config/scened.config`, which
loads the `MinimalScene` plugin to create the scene, and the `TransportSceneManager`
plugin to update the scene using Gazebo Transport.

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
gz gui -c examples/config/scene3d.config
```

You should see a black box moving around the scene.

## Testing other plugins

### Camera tracking

Some commands to test camera tracking with this demo:

Move to box:

```
gz service -s /gui/move_to --reqtype gz.msgs.StringMsg --reptype gz.msgs.Boolean --timeout 2000 --req 'data: "box_model"'
```

Echo camera pose:

```
gz topic -e -t /gui/camera/pose
```

Follow box:

```
gz service -s /gui/follow --reqtype gz.msgs.StringMsg --reptype gz.msgs.Boolean --timeout 2000 --req 'data: "box_model"'
```

Update follow offset:

```
gz service -s /gui/follow/offset --reqtype gz.msgs.Vector3d --reptype gz.msgs.Boolean --timeout 2000 --req 'x: 5, y: 5, z: 5'
```

Set next follow pgain:

```
gz service -s /gui/follow/pgain --reqtype gz.msgs.Double --reptype gz.msgs.Boolean --timeout 2000 --req 'data: 1.0'
```

Update follow offset with new pgain:

```
gz service -s /gui/follow/offset --reqtype gz.msgs.Vector3d --reptype gz.msgs.Boolean --timeout 2000 --req 'x: 1, y: 1, z: 5'
```