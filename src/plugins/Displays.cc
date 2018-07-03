/*
 * Copyright (C) 2018 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/rendering.hh>

#include "ignition/gui/Iface.hh"
#include "ignition/gui/QtMetatypes.hh"
#include "ignition/gui/DisplayPlugin.hh"
#include "ignition/gui/plugins/Displays.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class DisplaysPrivate
  {
    /// \brief We keep a pointer to the engine and rely on it not being
    /// destroyed, since it is a singleton.
    public: rendering::RenderEngine *engine;

    /// \brief We keep the scene name rather than a shared pointer because we
    /// don't want to share ownership.
    public: std::string sceneName{"scene"};

    /// \brief Keep track of the loaded plugins.
    public: std::vector<std::shared_ptr<Plugin> > displayPlugins;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
Displays::Displays()
  : Plugin(), dataPtr(new DisplaysPrivate)
{
}

/////////////////////////////////////////////////
Displays::~Displays()
{
}

/////////////////////////////////////////////////
void Displays::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Displays";

  // Configuration
  std::string engineName{"ogre"};
  if (_pluginElem)
  {
    if (auto elem = _pluginElem->FirstChildElement("engine"))
      engineName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("scene"))
      this->dataPtr->sceneName = elem->GetText();
  }

  std::string error{""};
  rendering::ScenePtr scene;

  // Render engine
  this->dataPtr->engine = rendering::engine(engineName);
  if (!this->dataPtr->engine)
  {
    error = "Engine \"" + engineName
           + "\" not supported, Displays plugin won't work.";
    ignwarn << error << std::endl;
  }
  else
  {
    // Scene
    scene = this->dataPtr->engine->SceneByName(this->dataPtr->sceneName);
    if (!scene)
    {
      error = "Scene \"" + this->dataPtr->sceneName
             + "\" not found, Displays plugin won't work.";
      ignwarn << error << std::endl;
    }
  }

  // Don't waste time loading widgets if this will be deleted anyway
  if (this->DeleteLaterRequested())
    return;

  if (!error.empty())
  {
    // Add message
    auto msg = new QLabel(QString::fromStdString(error));

    auto mainLayout = new QVBoxLayout();
    mainLayout->addWidget(msg);
    mainLayout->setAlignment(msg, Qt::AlignCenter);
    this->setLayout(mainLayout);

    return;
  }
  // Create layout
  auto mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  this->setLayout(mainLayout);

  // TODO(dhood): Get list of initial display plugins from config file.
  std::list<std::string> pluginsToLoad {"GridDisplay", "RealtimeFactorDisplay"};
  for (auto pluginToLoad : pluginsToLoad) {
    std::shared_ptr<Plugin> plugin =
      loadPluginWithoutAdding(pluginToLoad, nullptr);
    if (plugin == nullptr)
    {
      ignerr << "Couldn't load plugin [" << pluginToLoad << "]" << std::endl;
      return;
    }

    std::shared_ptr<ignition::gui::DisplayPlugin> displayPlugin =
      std::dynamic_pointer_cast<ignition::gui::DisplayPlugin> (plugin);
    if (displayPlugin == nullptr)
    {
      ignerr << "Couldn't cast plugin [" << pluginToLoad
        << "] to DisplayPlugin" << std::endl;
      return;
    }
    this->dataPtr->displayPlugins.push_back(plugin);

    // Create the standard options for the display plugin.
    auto standardProperties = displayPlugin->CreateStandardProperties();
    mainLayout->addWidget(standardProperties);

  }
  auto spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mainLayout->addWidget(spacer);
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Displays,
                                  ignition::gui::Plugin)

