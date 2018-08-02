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
    /// \brief Keep track of the loaded display plugins.
    public: std::vector<std::shared_ptr<DisplayPlugin> > displayPlugins;
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

  // Create layout
  auto displaysLayout = new QVBoxLayout();
  displaysLayout->setContentsMargins(0, 0, 0, 0);
  displaysLayout->setSpacing(0);

  // Displays
  if (auto displaysElem = _pluginElem->FirstChildElement("displays"))
  {
    for (auto displayElem = displaysElem->FirstChildElement("display");
        displayElem != nullptr;
        displayElem = displayElem->NextSiblingElement("display"))
    {
      auto pluginToLoad = displayElem->Attribute("type");
      if (!pluginToLoad)
      {
        ignerr << "Display plugin type not specified." << std::endl;
        continue;
      }
      std::shared_ptr<DisplayPlugin> displayPlugin =
        loadDisplayPlugin(pluginToLoad, displayElem);
      if (displayPlugin == nullptr)
      {
        ignerr << "Couldn't load plugin [" << pluginToLoad << "]" << std::endl;
        continue;
      }

      this->dataPtr->displayPlugins.push_back(displayPlugin);

      // Create the configuration options for the display plugin.
      auto pluginProperties = displayPlugin->CreateProperties();
      displaysLayout->addWidget(pluginProperties);
    }
  }

  // Make the displays stack compactly vertically.
  displaysLayout->addStretch(1);

  auto widget = new QWidget();
  widget->setLayout(displaysLayout);

  // Place the list of displays inside a scroll area.
  // This prevents the expansion of displays' configurations from pushing the
  // widget off the bottom of the screen.
  auto scrollArea = new QScrollArea();
  scrollArea->setWidget(widget);
  scrollArea->setWidgetResizable(true);
  scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  auto layout = new QVBoxLayout();
  layout->setSpacing(0);
  layout->addWidget(scrollArea);

  this->setLayout(layout);
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Displays,
                                  ignition::gui::Plugin)

