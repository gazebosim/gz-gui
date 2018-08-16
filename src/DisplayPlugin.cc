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
  /// \brief Type of display plugin.
  public: std::string type = "DisplayPlugin";

  /// \brief If the display should be rendered.
  public: bool visible = true;

  /// \brief Name of rendering scene requested.
  public: std::string sceneName = "scene";

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
  if (_pluginElem && _pluginElem->Attribute("type"))
  {
    this->dataPtr->type = _pluginElem->Attribute("type");
  }
  else
  {
    ignerr << "Type not known for display plugin." << std::endl;
  }

  // Configuration
  std::string engineName{"ogre"};
  if (_pluginElem)
  {
    // We don't support other engines currently because only Ogre supports the
    // functionality required for Display Plugins (e.g. SetVisible()).
    /*
    if (auto elem = _pluginElem->FirstChildElement("engine"))
      engineName = elem->GetText();
    */

    // Update the requested scene name even it fails to load, so that it is the
    // name that will be saved in the config.
    if (auto elem = _pluginElem->FirstChildElement("scene"))
      this->dataPtr->sceneName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("visible"))
      elem->QueryBoolText(&this->dataPtr->visible);

    if (auto elem = _pluginElem->FirstChildElement("title"))
      this->title = elem->GetText();
  }

  std::string error{""};
  rendering::ScenePtr scene;

  // Render engine
  auto engine = rendering::engine(engineName);
  if (!engine)
  {
    error = "Engine \"" + engineName
      + "\" not supported, display plugin [" + this->dataPtr->type
      + "] won't work.";
    ignwarn << error << std::endl;
  }
  else
  {
    // Scene
    scene = engine->SceneByName(this->dataPtr->sceneName);
    this->dataPtr->weakScenePtr = scene;
    if (!scene)
    {
      error = "Scene \"" + this->dataPtr->sceneName
        + "\" not found, display plugin [" + this->dataPtr->type
        + "] won't work.";
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
  this->OnVisibilityChange(this->dataPtr->visible);
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
  visibleCheck->setObjectName("displayPluginVisibleCheck");
  visibleCheck->setChecked(this->dataPtr->visible);
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
    auto collapsibleButton = collapsible->Button();
    if (!collapsibleButton)
    {
      ignerr << "Error creating collapsible widget for display plugin [ " <<
        this->title << "]" << std::endl;
    }
    else
    {
      collapsibleButton->setFlat(true);
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
  this->dataPtr->visible = _value;
  this->dataPtr->visual->SetVisible(_value);
}

/////////////////////////////////////////////////
tinyxml2::XMLElement *DisplayPlugin::Config(tinyxml2::XMLDocument *_doc) const
{
  // Display
  auto displayElem = _doc->NewElement("display");
  displayElem->SetAttribute("type", this->dataPtr->type.c_str());
  _doc->InsertEndChild(displayElem);

  // Title
  auto titleElem = _doc->NewElement("title");
  titleElem->SetText(this->title.c_str());
  displayElem->InsertEndChild(titleElem);

  // Scene
  auto sceneElem = _doc->NewElement("scene");
  // Save the requested scene even if it couldn't be loaded/is not longer valid.
  sceneElem->SetText(this->dataPtr->sceneName.c_str());
  displayElem->InsertEndChild(sceneElem);

  // Visible
  auto visibleElem = _doc->NewElement("visible");
  visibleElem->SetText(this->dataPtr->visible ? "true" : "false");
  displayElem->InsertEndChild(visibleElem);

  // Custom config for subclasses
  // Pass in this document so it can be used to create any new element(s).
  // TinyXML2 doesn't support inserting elements created from different docs.
  if (auto customConfigElems = this->CustomConfig(_doc))
  {
    // The custom config contains a list of sibling elements of the properties.
    // Attach each of them to the display's element.
    // Note that inserting the custom config element elsewhere in the document
    // removes it from customConfigElems (which is why the first child is
    // repeatedly checked).
    while (auto customConfigElem = customConfigElems->FirstChildElement())
    {
      displayElem->InsertEndChild(customConfigElem);
    }
  }

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

