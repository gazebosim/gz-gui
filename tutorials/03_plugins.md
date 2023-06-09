\page plugins Plugins

Next Tutorial: \ref config
Previous Tutorial: \ref commandline

## Writing plugins

A Gazebo GUI plugin is a shared library that defines a widget.
The plugin contains [QML](https://doc.qt.io/qt-5/qtqml-index.html)
code that specifies what the widget looks like, as well as C++ code
that defines the plugin's behavior and ties it to other libraries.

See [HelloPlugin](https://github.com/gazebosim/gz-gui/blob/main/examples/plugin/hello_plugin/)
for an example.

## Finding plugins

Gazebo GUI will look for plugins on the following paths, in this order:

1. All paths set on the `GZ_GUI_PLUGIN_PATH` environment variable
2. All paths added by calling `gz::gui::addPluginPath`
3. `~/.gz/gui/plugins`
4. [Plugins which are installed with Gazebo GUI](https://gazebosim.org/api/gui/6.0/namespace gz_1_1gui_1_1plugins.html)

## Configuring plugins

Gazebo GUI supports loading XML configuration files, which may contain
parameters to be passed directly to the plugin. See the \subpage config
tutorial for more information.

## Built-in plugins

### Image display

Display images from a Gazebo Transport topic.

    gz gui -c examples/config/image.config

or

    gz gui -s ImageDisplay

### Publisher

Publish messages on a Gazebo Transport topic.

1. Listen to string messages on the `/echo` topic:

        gz topic -e -t /echo

2. On a new terminal, open the publisher GUI:

        gz gui -s Publisher

3. The GUI is pre-filled to publish "Hello" messages on the `/echo` topic.
   Click "Publish" to publish a message and see it on the terminal.

### World stats and control

* Stats: Display sim / real time published on a topic
* Control: Buttons to make play / pause / step service requests

        gz gui -c examples/config/time.config

### Topic echo

Echo messages from a Gazebo Transport topic.

    gz gui -c examples/config/pubsub.config

### Key publisher

Publish key presses through Gazebo Transport.

    gz gui -c examples/config/keypublisher.config

Change the topic to echo to `/keyboard/keypress`, echo, and start pressing keys.

### Topic viewer

**Not ported to versions 1 or higher yet**

Discover all Gazebo Transport topics and introspect its fields.

    gz gui -c examples/config/topicViewer.config

### Topics stats

**Not ported to versions 1 or higher yet**

Show basic stats of all Gazebo Transport topics at the same time.

    gz gui -c examples/config/topicsStats.config

### Requester and Responder

**Not ported to versions 1 or higher yet**

Make custom service requests and serve custom services with these two
plugins.

    gz gui -c examples/config/reqres.config

### Displays

**Not ported to versions 1 or higher yet**

This is a container for various Display Plugins that each paint on a Scene.
The following example includes the display plugins that are installed with
Gazebo GUI by default.

    gz gui -c examples/config/display_plugins.config

#### Finding display plugins

**Not ported to versions 1 or higher yet**

Gazebo GUI will look for display plugins on the following paths, in this
order:

1. All paths set on the `GZ_GUI_DISPLAY_PLUGIN_PATH` environment variable
2. `~/.gz/gui/display_plugins`
3. [Display plugins that are installed with Gazebo GUI](https://gazebosim.org/api/gui/0.1/namespace gz_1_1gui_1_1display_plugins.html)
