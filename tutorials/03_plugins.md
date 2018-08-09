Plugins {#plugins}
=======

## Finding plugins

Ignition GUI will look for plugins on the following paths, in this order:

1. All paths set on the `IGN_GUI_PLUGIN_PATH` environment variable
2. All paths added by calling `ignition::gui::addPluginPath`
3. `~/.ignition/gui/plugins`
4. [Plugins which are installed with Ignition GUI](https://ignitionrobotics.org/api/gui/0.1/namespaceignition_1_1gui_1_1plugins.html)

## Plugin configuration

Ignition GUI supports loading configuration files (XML) which are passed to
plugins and can be parsed using TinyXml2 (For now, see issue #7).

For example (`examples/config/image.config`):

    <plugin filename="ImageDisplay">
      <title>Without picker</title>
      <topic_picker>false</topic_picker>
      <topic>/rendering/image</topic>
    </plugin>

* Developers can read custom plugin configurations overriding the
  `Plugin::LoadConfig` function.

* Ignition GUI processes a few default parameters before passing the config to
  implemented plugins. These are the parameters handled:

    * `<title>`: The title to be displayed on the plugin's title bar

    * `<has_titlebar>`: Set this to false to remove the title bar

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

### Time panel

* Display sim / real time published on a topic
* Buttons to make play / pause service requests

        ign gui -c examples/config/time.config

### Topic echo

Echo messages from an Ignition Transport topic.

    ign gui -c examples/config/pubsub.config

### Topic viewer

Discover all Ignition Transport topics and introspect its fields.

    ign gui -c examples/config/topicViewer.config

### Topics stats

Show basic stats of all Ignition Transport topics at the same time.

    ign gui -c examples/config/topicsStats.config

### Requester and Responder

Make custom service requests and serve custom services with these two
plugins.

    ign gui -c examples/config/reqres.config

### Displays {#display-plugins}

This is a container for various Display Plugins that each paint on a Scene.
The following example includes the display plugins that are installed with
Ignition GUI by default.

    ign gui -c examples/config/display_plugins.config

#### Finding display plugins

Ignition GUI will look for display plugins on the following paths, in this
order:

1. All paths set on the `IGN_GUI_DISPLAY_PLUGIN_PATH` environment variable
1. `~/.ignition/gui/display_plugins`
1. [Display plugins that are installed with Ignition GUI](https://ignitionrobotics.org/api/gui/0.1/namespaceignition_1_1gui_1_1display_plugins.html)
