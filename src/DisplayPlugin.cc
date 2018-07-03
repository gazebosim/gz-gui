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

#include <ignition/common/Console.hh>
#include <ignition/rendering.hh>

#include "ignition/gui/QtMetatypes.hh"
#include "ignition/gui/DisplayPlugin.hh"
#include "ignition/gui/CollapsibleWidget.hh"

class ignition::gui::DisplayPluginPrivate
{
  /// \brief We keep a pointer to the engine and rely on it not being
  /// destroyed, since it is a singleton.
  public: rendering::RenderEngine *engine;

  /// \brief We keep the scene name rather than a shared pointer because we
  /// don't want to share ownership.
  public: std::string sceneName{"scene"};

  /// \brief The visual to which subclasses can attach their display(s).
  public: rendering::VisualPtr visual;

  /// \brief If the display should be rendered.
  public: bool visible = true;
};

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
DisplayPlugin::DisplayPlugin()
  : Plugin(), dataPtr(new DisplayPluginPrivate)
{
}

/////////////////////////////////////////////////
DisplayPlugin::~DisplayPlugin()
{
}

/////////////////////////////////////////////////
QWidget* DisplayPlugin::CreateCustomProperties()
{
  return nullptr;
}

/////////////////////////////////////////////////
void DisplayPlugin::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  this->title = "Unnamed display";

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
           + "\" not supported, DisplayPlugin plugin won't work.";
    ignwarn << error << std::endl;
  }
  else
  {
    // Scene
    scene = this->dataPtr->engine->SceneByName(this->dataPtr->sceneName);
    if (!scene)
    {
      error = "Scene \"" + this->dataPtr->sceneName
             + "\" not found, DisplayPlugin plugin won't work.";
      ignwarn << error << std::endl;
    }
    else
    {
      auto root = scene->RootVisual();
      this->dataPtr->visual = scene->CreateVisual();
      root->AddChild(this->dataPtr->visual);
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
  this->Initialize(_pluginElem);
}

ignition::rendering::VisualPtr DisplayPlugin::Visual()
{
  return this->dataPtr->visual;
}

ignition::rendering::ScenePtr DisplayPlugin::Scene()
{
  // TODO: improve
  return this->dataPtr->engine->SceneByName(this->dataPtr->sceneName);
}

QWidget* DisplayPlugin::CreateProperties()
{
  auto propertiesLayout = new QHBoxLayout();
  auto propertiesWidget = new QWidget();

  // Create generic configuration options for all display plugins.
  auto visibleCheck = new QCheckBox(QString::fromStdString(this->title));
  visibleCheck->setObjectName("visibleCheck");
  visibleCheck->setToolTip("Toggle visibility");
  visibleCheck->setChecked(this->dataPtr->visible);
  this->connect(visibleCheck,
    SIGNAL(toggled(bool)), this, SLOT(OnVisibilityChange(bool)));
  propertiesLayout->addWidget(visibleCheck);

  // Create the custom configuration options for this specific display plugin.
  auto customProperties = this->CreateCustomProperties();

  // Integrate the custom configutaion options, if appropriate.
  if (nullptr != customProperties)
  {
    // Remove the title from the checkbox and put it in a collapsible button.
    visibleCheck->setText("");
    auto collapsible = new CollapsibleWidget(this->title);
    // TODO(dhood): Make the widget use its uncollapsed size from the start?
    collapsible->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    collapsible->AppendContent(customProperties);
    propertiesLayout->addWidget(collapsible);
  }
  propertiesWidget->setLayout(propertiesLayout);

  return propertiesWidget;
}

/////////////////////////////////////////////////
void DisplayPlugin::Initialize(const tinyxml2::XMLElement *_pluginElem)
{
}

/////////////////////////////////////////////////
void DisplayPlugin::OnVisibilityChange(bool _value)
{
  this->dataPtr->visible = _value;
  // TODO(dhood): call setVisible on the visual
}

