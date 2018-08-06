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

#include "ignition/gui/CollapsibleWidget.hh"
#include "ignition/gui/DisplayPlugin.hh"
#include "ignition/gui/QtMetatypes.hh"

class ignition::gui::DisplayPluginPrivate
{
  /// \brief We keep a pointer to the engine and rely on it not being
  /// destroyed, since it is a singleton.
  public: rendering::RenderEngine *engine = nullptr;

  /// \brief Rendering scene to use.
  public: std::weak_ptr<rendering::Scene> weakScenePtr;

  /// \brief The visual to which subclasses can attach their display(s).
  public: rendering::VisualPtr visual = nullptr;
};

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
DisplayPlugin::DisplayPlugin()
  : dataPtr(new DisplayPluginPrivate)
{
  this->title = "Unnamed display";
}

/////////////////////////////////////////////////
DisplayPlugin::~DisplayPlugin()
{
}

/////////////////////////////////////////////////
QWidget *DisplayPlugin::CreateCustomProperties() const
{
  return nullptr;
}

/////////////////////////////////////////////////
void DisplayPlugin::Load(const tinyxml2::XMLElement *_pluginElem)
{
  // TODO(dhood): support config parsing of displays.
  // Despite what it looks like in the following code, the config is not
  // actually being passed in currently.

  // Configuration
  std::string engineName{"ogre"};
  std::string sceneName{"scene"};
  if (_pluginElem)
  {
    if (auto elem = _pluginElem->FirstChildElement("engine"))
      engineName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("scene"))
      sceneName = elem->GetText();
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
    scene = this->dataPtr->engine->SceneByName(sceneName);
    this->dataPtr->weakScenePtr = scene;
    if (!scene)
    {
      error = "Scene \"" + sceneName
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

/////////////////////////////////////////////////
ignition::rendering::VisualPtr DisplayPlugin::Visual() const
{
  return this->dataPtr->visual;
}

/////////////////////////////////////////////////
std::weak_ptr<ignition::rendering::Scene> DisplayPlugin::Scene() const
{
  return this->dataPtr->weakScenePtr;
}

/////////////////////////////////////////////////
QWidget *DisplayPlugin::CreateProperties() const
{
  auto propertiesLayout = new QHBoxLayout();
  propertiesLayout->setContentsMargins(0, 0, 0, 0);
  propertiesLayout->setSpacing(0);

  // Create generic configuration options for all display plugins.
  auto visibleCheck = new QCheckBox(QString::fromStdString(this->title));
  visibleCheck->setToolTip("Toggle visibility");
  visibleCheck->setChecked(this->visible);
  this->connect(visibleCheck,
    SIGNAL(toggled(bool)), this, SLOT(OnVisibilityChange(bool)));

  auto checkLayout = new QVBoxLayout();
  checkLayout->addWidget(visibleCheck);
  checkLayout->setContentsMargins(5, 10, 5, 0);

  propertiesLayout->addLayout(checkLayout);
  propertiesLayout->setAlignment(checkLayout, Qt::AlignTop);

  // Create the custom configuration options for this specific display plugin.
  auto customProperties = this->CreateCustomProperties();

  // Integrate the custom configutaion options, if appropriate.
  if (nullptr != customProperties)
  {
    // Remove the title from the checkbox and put it in a collapsible button.
    visibleCheck->setText("");
    auto collapsible = new CollapsibleWidget(this->title);
    collapsible->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    collapsible->AppendContent(customProperties);
    collapsible->setObjectName("displayPluginCollapsible");
    // Make the button flat so it does not look different to displays without
    // collapsible custom configuration.
    // Note(dhood): this is done programmatically rather than via QSS so that
    // it does not need to be specified in custom styles.
    // There is a note in style.qss that references this, for users wondering
    // why styles aren't taking effect.
    auto collapsible_button = collapsible->Button();
    if (!collapsible_button)
    {
      ignerr << "Error creating collapsible widget for display plugin [ " <<
        this->title << "]" << std::endl;
    }
    else
    {
      collapsible_button->setFlat(true);
    }
    propertiesLayout->addWidget(collapsible);
  }
  else
  {
    visibleCheck->setStyleSheet("spacing: 15px;");
  }

  auto propertiesWidget = new QWidget();
  propertiesWidget->setLayout(propertiesLayout);

  return propertiesWidget;
}

/////////////////////////////////////////////////
void DisplayPlugin::Initialize(const tinyxml2::XMLElement */*_pluginElem*/)
{
}

/////////////////////////////////////////////////
void DisplayPlugin::OnVisibilityChange(const bool _value)
{
  if (nullptr == this->dataPtr->visual)
  {
    return;
  }
  this->visible = _value;
  this->dataPtr->visual->SetVisible(_value);
}

/////////////////////////////////////////////////
tinyxml2::XMLElement * DisplayPlugin::Config(tinyxml2::XMLDocument *_doc) const
{
  // Display
  auto displayElem = _doc->NewElement("display");
  displayElem->SetAttribute("type", this->Type().c_str());
  _doc->InsertEndChild(displayElem);

  // Visible
  auto visibleElem = _doc->NewElement("visible");
  visibleElem->SetText(std::to_string(this->visible).c_str());
  displayElem->InsertEndChild(visibleElem);

  return displayElem;
}

/////////////////////////////////////////////////
std::string DisplayPlugin::ConfigStr() const
{
  tinyxml2::XMLDocument doc;
  this->Config(&doc);

  tinyxml2::XMLPrinter printer;
  doc.Print(&printer);
  return printer.CStr();
}

