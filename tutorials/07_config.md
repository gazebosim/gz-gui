\page config Config

Next Tutorial: \ref layout

## Configuration files

Gazebo GUI supports loading XML configuration files that define what
the window should look like and which plugins should be loaded.

### Loading

By default, Gazebo GUI will load the config file at
`$HOME/.gz/gui/default.config`, if it exists.

Configuration files can also be loaded from the command line or through the
C++ API.

From the command line, use the `--config` / `-c` option.
For example, you can specify an absolute path:

`gz gui -c /absolute/path/to/example.config`

Or a path relative to the current working directory:

`gz gui -c relative/path/to/example.config`

Or a path relative to a custom directory, which you can specify by setting the
environment variable `GZ_GUI_RESOURCE_PATH`, like so:

`GZ_GUI_RESOURCE_PATH=/absolute/path/to/ gz gui --config example.config`

From the C++ API, pass the file path to
[Application::LoadConfig](https://gazebosim.org/api/gui/6.0/classignition_1_1gui_1_1Application.html#a03c4c3a1b1e58cc4bff05658f21fff17).

### File structure

Gazebo GUI accepts the following top-level elements on a config file:

* `<window>`: Options related to the entire window's layout.
  See \subpage layout for more details.
* `<plugin>`: Zero or more plugins to be loaded at startup.
    * `filename`: This attribute specifies the plugin library to be loaded.
    * `<gz-gui>`: Gazebo GUI processes this block before passing the
      config to the plugin. See
      [plugin_params.config](https://github.com/gazebosim/gz-gui/blob/main/examples/config/plugin_params.config)
      for an example.
    * custom elements: Developers can read custom plugin configurations overriding the
      [Plugin::LoadConfig](https://gazebosim.org/api/gui/6.0/classignition_1_1gui_1_1Plugin.html#a720646.0af4cd247b994b905559fd4ee)
      function, see the
      [HelloPlugin](https://github.com/gazebosim/gz-gui/blob/main/examples/plugin/hello_plugin/HelloPlugin.cc)
      example.

See the example plugin block below:

    <plugin filename="ImageDisplay">
      <gz-gui>
        <property type="int" key="height">120</property>
      </gz-gui>
      <topic>/camera</topic>
    </plugin>

This will load the `libImageDisplay.so` plugin, Gazebo GUI will set its
`height` to `120` pixels, and the plugin-specific `<topic>` parameter will be
handled within `ImageDisplay::LoadConfig`.
