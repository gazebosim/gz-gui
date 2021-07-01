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

