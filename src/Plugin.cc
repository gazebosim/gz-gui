/*
 * Copyright (C) 2017 Open Source Robotics Foundation
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

#include <ignition/common/Console.hh>
#include "ignition/gui/Application.hh"
#include "ignition/gui/Plugin.hh"

class ignition::gui::PluginPrivate
{
  /// \brief Set this to true if the plugin should be deleted as soon as it has
  ///  a parent.
  public: bool deleteLater{false};

  /// \brief Holds the value of the `delete_later` attribute on the
  /// configuration. Subclasses can check this value for example to return
  /// before the end of LoadConfig.
  public: bool deleteLaterRequested{false};

  /// \brief Pointer to item generated with plugin's QML
  public: QQuickItem *pluginItem{nullptr};

  /// \brief Pointer to wrapping card item
  public: QQuickItem *cardItem{nullptr};

  /// \brief Card's X position
  public: int x{-1};

  /// \brief Card's Y position
  public: int y{-1};

  /// \brief Card's Z position
  public: int z{-1};

  /// \brief Card's height
  public: int height{-1};

  /// \brief Card's width
  public: int width{-1};

  /// \brief
  public: std::string cardFrame = "visible";
};

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
Plugin::Plugin() : dataPtr(new PluginPrivate)
{
}

/////////////////////////////////////////////////
Plugin::~Plugin()
{
}

/////////////////////////////////////////////////
void Plugin::Load(const tinyxml2::XMLElement *_pluginElem)
{
  if (!_pluginElem)
  {
    ignerr << "Failed to load plugin with a NULL element." << std::endl;
    return;
  }

  // TODO: Too complicated to deep clone elements with tinyxml2, storing
  // string for now and consider moving away from tinyxml
  tinyxml2::XMLPrinter printer;
  if (!_pluginElem->Accept(&printer))
  {
    ignwarn << "There was an error parsing the plugin element for " <<
        "[" << this->title << "]." << std::endl;
  }
  else
  {
    this->configStr = std::string(printer.CStr());
  }

  // Qml file
  std::string filename = _pluginElem->Attribute("filename");

  // This let's <filename>.qml use <pluginclass> functions and properties
  auto context = new QQmlContext(App()->Engine()->rootContext());
  context->setContextProperty(QString::fromStdString(filename), this);

  // Instantiate plugin QML file into a component
  std::string qmlFile(":/" + filename + "/" + filename + ".qml");
  QQmlComponent component(App()->Engine(), QString::fromStdString(qmlFile));

  // Create an item for the plugin
  this->dataPtr->pluginItem =
      qobject_cast<QQuickItem *>(component.create(context));
  if (!this->dataPtr->pluginItem)
  {
    ignerr << "Failed to instantiate QML file [" << qmlFile << "]." << std::endl
           << "* Are you sure it's been added to the .qrc file?" << std::endl
           << "* Are you sure the file is valid QML? "
           << "You can check with the `qmlscene` tool" << std::endl;
    return;
  }

  // Keep card state to set when creating card
  int x;
  if (_pluginElem->QueryAttribute("x", &x) == tinyxml2::XML_SUCCESS)
    this->dataPtr->x = x;

  int y;
  if (_pluginElem->QueryAttribute("y", &y) == tinyxml2::XML_SUCCESS)
    this->dataPtr->y = y;

  int z;
  if (_pluginElem->QueryAttribute("z", &z) == tinyxml2::XML_SUCCESS)
    this->dataPtr->z = z;

  int height;
  if (_pluginElem->QueryAttribute("height", &height) == tinyxml2::XML_SUCCESS)
    this->dataPtr->height = height;

  int width;
  if (_pluginElem->QueryAttribute("width", &width) == tinyxml2::XML_SUCCESS)
    this->dataPtr->width = width;

  const char *cardFrame = _pluginElem->Attribute("card_frame");
  if (cardFrame)
    this->dataPtr->cardFrame = cardFrame;

  // Delete later
  if (_pluginElem->Attribute("delete_later"))
  {
    // Store param
    _pluginElem->QueryBoolAttribute("delete_later",
        &this->dataPtr->deleteLaterRequested);

    // Use it
    if (this->dataPtr->deleteLaterRequested)
      this->DeleteLater();
  }

  // Read default params
  if (auto titleElem = _pluginElem->FirstChildElement("title"))
    this->title = titleElem->GetText();

  // Setup default context menu
//  this->setContextMenuPolicy(Qt::CustomContextMenu);
//  this->connect(this,
//      SIGNAL(customContextMenuRequested(const QPoint &)),
//      this, SLOT(ShowContextMenu(const QPoint &)));

  // Load custom configuration
  this->LoadConfig(_pluginElem);
}

/////////////////////////////////////////////////
std::string Plugin::ConfigStr()
{
  // TODO: When plugins override this function they will lose the card updates,
  // must refactor config handling

  // Convert string to XML
  tinyxml2::XMLDocument doc;
  doc.Parse(this->configStr.c_str());

  auto pluginElem = doc.FirstChildElement("plugin");
  if (!pluginElem)
  {
    ignerr << "Missing <plugin> element, not updating config string."
           << std::endl;
    return this->configStr;
  }

  // Update arguments
  pluginElem->SetAttribute("x", this->CardItem()->x());
  pluginElem->SetAttribute("y", this->CardItem()->y());
  pluginElem->SetAttribute("z", this->CardItem()->z());
  pluginElem->SetAttribute("height", this->CardItem()->height());
  pluginElem->SetAttribute("width", this->CardItem()->width());

  // Then convert XML back to string
  tinyxml2::XMLPrinter printer;
  if (!pluginElem->Accept(&printer))
  {
    ignwarn << "There was an error parsing the plugin element for " <<
        "[" << this->title << "]." << std::endl;
  }
  else
  {
    this->configStr = std::string(printer.CStr());
  }

  return this->configStr;
}

/////////////////////////////////////////////////
void Plugin::ShowContextMenu(const QPoint &_pos)
{
  // Close action
//  QAction closeAct(QString::fromStdString("Close [" + this->title + "]"),
//      this);
//  this->connect(&closeAct, SIGNAL(triggered()), this->parent(),
//      SLOT(close()));

  // Context menu
//  QMenu contextMenu(tr("Context menu"), this);
//  contextMenu.addAction(&closeAct);
//  contextMenu.exec(this->mapToGlobal(_pos));
}

/////////////////////////////////////////////////
// void Plugin::changeEvent(QEvent *_e)
// {
//   if (_e->type() == QEvent::ParentChange && this->parent() &&
//       this->dataPtr->deleteLater)
//   {
//     qobject_cast<QWidget *>(this->parent())->close();
//   }
// }

/////////////////////////////////////////////////
void Plugin::DeleteLater()
{
  this->dataPtr->deleteLaterRequested = true;
//  if (this->parent())
//  {
//    qobject_cast<QWidget *>(this->parent())->close();
//  }
//  else
//  {
//    this->dataPtr->deleteLater = true;
//  }
}

/////////////////////////////////////////////////
bool Plugin::DeleteLaterRequested() const
{
  return this->dataPtr->deleteLaterRequested;
}

/////////////////////////////////////////////////
QQuickItem *Plugin::PluginItem() const
{
  return this->dataPtr->pluginItem;
}

/////////////////////////////////////////////////
QQuickItem *Plugin::CardItem() const
{
  if (!this->dataPtr->pluginItem)
    return nullptr;

  // If already created, just return it
  if (this->dataPtr->cardItem)
    return this->dataPtr->cardItem;

  // Instantiate a card
  std::string qmlFile(":qml/Card.qml");
  QQmlComponent cardComp(App()->Engine(),
      QString(QString::fromStdString(qmlFile)));
  auto cardItem = qobject_cast<QQuickItem *>(cardComp.create());
  if (!cardItem)
  {
    ignerr << "Internal error: Failed to instantiate QML file [" << qmlFile
           << "]" << std::endl;
    return nullptr;
  }

  // C++ ownership
  QQmlEngine::setObjectOwnership(cardItem, QQmlEngine::CppOwnership);

  // Get card parts
  auto cardContentItem = cardItem->findChild<QQuickItem *>("content");
  if (!cardContentItem)
  {
    ignerr << "Null card content QQuickItem!" << std::endl;
    return nullptr;
  }

  auto cardToolbarItem = cardItem->findChild<QQuickItem *>("cardToolbar");
  if (!cardToolbarItem)
  {
    ignerr << "Null toolbar content QQuickItem!" << std::endl;
    return nullptr;
  }

  // Add plugin to card content
  this->dataPtr->pluginItem->setParentItem(cardContentItem);

  // Configure card
  // Adjust size to accomodate plugin if not explicitly set in config
  auto pluginWidth = this->dataPtr->width > 0 ?
      this->dataPtr->width :
      this->dataPtr->pluginItem->property("width").toInt();
  auto pluginHeight = this->dataPtr->height > 0 ?
      this->dataPtr->height :
      (this->dataPtr->pluginItem->property("height").toInt() +
       cardToolbarItem->property("height").toInt());

  cardItem->setProperty("pluginName",
      QString::fromStdString(this->Title()));
  cardItem->setProperty("cardFrame",
      QString::fromStdString(this->dataPtr->cardFrame));
  cardItem->setProperty("width", pluginWidth);
  cardItem->setProperty("height", pluginHeight);
  if (this->dataPtr->x >= 0)
    cardItem->setProperty("x", this->dataPtr->x);
  if (this->dataPtr->y >= 0)
    cardItem->setProperty("y", this->dataPtr->y);
  if (this->dataPtr->z >= 0)
    cardItem->setProperty("z", this->dataPtr->z);

  this->dataPtr->cardItem = cardItem;

  return cardItem;
}

