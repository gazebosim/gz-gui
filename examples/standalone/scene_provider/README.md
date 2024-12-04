## Scene provider

This example demonstrates how to populate and update a 3D scene using
Gazebo Transport.

This example is meant to be used with `examples/config/scened.config`, which
loads the `MinimalScene` plugin to create the scene, and the `TransportSceneManager`
plugin to update the scene using Gazebo Transport.

## Build

```bash
cd examples/standalone/scene_provider
mkdir build
cd build
cmake ..
make
```

## Run

In one terminal, start the scene provider:

```bash
cd examples/standalone/scene_provider/build
./scene_provider
```

On another terminal, start the example config:

```bash
gz gui -c examples/config/scene3d.config
```

You should see a black box moving around the scene.

## Testing other plugins

### Camera tracking

Some commands to test camera tracking with this demo:

Move to box:

```bash
gz service -s /gui/move_to --reqtype gz.msgs.StringMsg --reptype gz.msgs.Boolean --timeout 2000 --req 'data: "box_model"'
```

Echo camera pose:

```bash
gz topic -e -t /gui/camera/pose
```

Echo camera tracking information:

```bash
gz topic -e -t /gui/currently_tracked
```

Follow box from track topic:

```bash
gz topic -t /gui/track -m gz.msgs.CameraTrack -p 'track_mode: 2, follow_target: { name: "box_model"}'
```

Follow box from track topic:

```bash
gz topic -t /gui/track -m gz.msgs.CameraTrack -p 'track_mode: 2, follow_target: "box_model", follow_offset: {x: -1, y: 0, z: 1}'
```

Update follow offset from track topic:

```bash
gz topic -t /gui/track -m gz.msgs.CameraTrack -p 'track_mode: 2, follow_target: {name: "box_model"}, follow_offset: {x: -1, y: 0, z: 1}'
```
