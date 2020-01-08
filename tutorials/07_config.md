\page config Config

Next Tutorial: \ref layout

## Configuration files

Ignition GUI supports loading XML configuration files that define what
the window should look like and which plugins should be loaded.

### Loading

By default, Ignition GUI will load the config file at
`$HOME/.ignition/gui/default.config` if it exists.

Configuration files can also be loaded from the command line or through the
C++ API.

From the command line, use the `--config` / `-c` option, for example:

`ign gui -c path/to/example.config`

From the C++ API, pass the file path to
[Application::LoadConfig](https://ignitionrobotics.org/api/gui/3.0/classignition_1_1gui_1_1Application.html#a03c4c3a1b1e58cc4bff05658f21fff17).

### File structure

Ignition GUI accepts the following top-level elements on a config file:

* `<window>`: Options related to the entire window's layout.
  See \subpage layout for more details.
* `<plugin>`: Zero or more plugins to be loaded at startup.
    * `filename`: This argument specifies the plugin library to be loaded.
    * `<ignition-gui>`: Ignition GUI processes this block before passing the
      config to the plugin. See
      [plugin_params.config](https://bitbucket.org/ignitionrobotics/ign-gui/src/ign-gui2/examples/config/plugin_params.config)
      for an example.
    * custom elements: Developers can read custom plugin configurations overriding the
      [Plugin::LoadConfig](https://ignitionrobotics.org/api/gui/3.0/classignition_1_1gui_1_1Plugin.html#a72064530af4cd247b994b905559fd4ee)
      function, see the
      [HelloPlugin](https://bitbucket.org/ignitionrobotics/ign-gui/src/ign-gui2/examples/plugin/hello_plugin/HelloPlugin.cc)
      example.

See the example plugin block below:

    <plugin filename="ImageDisplay">
      <ignition-gui>
        <property type="int" key="height">120</property>
      </ignition-gui>
      <topic>/camera</topic>
    </plugin>

This will load the `libImageDisplay.so` plugin, Ignition GUI will set its
`height` to `120`, and the plugin-specific `<topic>` parameter will be
handled within `ImageDisplay::LoadConfig`.

