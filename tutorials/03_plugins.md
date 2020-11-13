\page plugins Plugins

Next Tutorial: \ref config
Previous Tutorial: \ref commandline

## Writing plugins

An Ignition GUI plugin is a shared library that defines a widget.
The plugin contains [QML](https://doc.qt.io/qt-5/qtqml-index.html)
code that specifies what the widget looks like, as well as C++ code
that defines the plugin's behaviour and ties it to other libraries.

See [HelloPlugin](https://github.com/ignitionrobotics/ign-gui/blob/ign-gui3/examples/plugin/hello_plugin/)
for an example.

## Finding plugins

Ignition GUI will look for plugins on the following paths, in this order:

1. All paths set on the `IGN_GUI_PLUGIN_PATH` environment variable
2. All paths added by calling `ignition::gui::addPluginPath`
3. `~/.ignition/gui/plugins`
4. [Plugins which are installed with Ignition GUI](https://ignitionrobotics.org/api/gui/3.3/namespaceignition_1_1gui_1_1plugins.html)

## Configuring plugins

Ignition GUI supports loading XML configuration files, which may contain
parameters to be passed directly to the plugin. See the \subpage config
tutorial for more information.

## Built-in plugins

### Image display

Display images from an Ignition Transport topic.

    ign gui -c examples/config/image.config

or

    ign gui -s ImageDisplay

### Publisher

Publish messages on an Ignition Transport topic.

1. Listen to string messages on the `/echo` topic:

        ign topic -e -t /echo

1. On a new terminal, open the publisher GUI:

        ign gui -s Publisher

1. The GUI is pre-filled to publish "Hello" messages on the `/echo` topic.
   Click "Publish" to publish a message and see it on the terminal.

### World stats and control

* Stats: Display sim / real time published on a topic
* Control: Buttons to make play / pause / step service requests

        ign gui -c examples/config/time.config

### Topic echo

Echo messages from an Ignition Transport topic.

    ign gui -c examples/config/pubsub.config

### Key publisher

Publish key presses through Ignition Transport.

    ign gui -c examples/config/keypublisher.config

Change the topic to echo to `/keyboard/keypress`, echo, and start pressing keys.

### Topic viewer

**Not ported to versions 1 or higher yet**

Discover all Ignition Transport topics and introspect its fields.

    ign gui -c examples/config/topicViewer.config

### Topics stats

**Not ported to versions 1 or higher yet**

Show basic stats of all Ignition Transport topics at the same time.

    ign gui -c examples/config/topicsStats.config

### Requester and Responder

**Not ported to versions 1 or higher yet**

Make custom service requests and serve custom services with these two
plugins.

    ign gui -c examples/config/reqres.config

### Displays {#display-plugins}

**Not ported to versions 1 or higher yet**

This is a container for various Display Plugins that each paint on a Scene.
The following example includes the display plugins that are installed with
Ignition GUI by default.

    ign gui -c examples/config/display_plugins.config

#### Finding display plugins

**Not ported to versions 1 or higher yet**

Ignition GUI will look for display plugins on the following paths, in this
order:

1. All paths set on the `IGN_GUI_DISPLAY_PLUGIN_PATH` environment variable
1. `~/.ignition/gui/display_plugins`
1. [Display plugins that are installed with Ignition GUI](https://ignitionrobotics.org/api/gui/0.1/namespaceignition_1_1gui_1_1display_plugins.html)
