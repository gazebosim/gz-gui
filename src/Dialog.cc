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
#include "ignition/gui/Dialog.hh"

namespace ignition
{
  namespace gui
  {
    class DialogPrivate
    {
      /// \brief default dialog config
      public: std::string config{""};

      /// \brief Pointer to quick window
      public: QQuickWindow *quickWindow{nullptr};
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
Dialog::Dialog()
  : dataPtr(new DialogPrivate)
{
  // Load QML and keep pointer to generated QQuickWindow
  std::string qmlFile("qrc:qml/StandaloneDialog.qml");
  App()->Engine()->load(QUrl(QString::fromStdString(qmlFile)));

  this->dataPtr->quickWindow = qobject_cast<QQuickWindow *>(
      App()->Engine()->rootObjects().value(0));
  if (!this->dataPtr->quickWindow)
  {
    ignerr << "Internal error: Failed to instantiate QML file [" << qmlFile
           << "]" << std::endl;
    return;
  }
}

/////////////////////////////////////////////////
Dialog::~Dialog()
{
}

/////////////////////////////////////////////////
QQuickWindow *Dialog::QuickWindow() const
{
  return this->dataPtr->quickWindow;
}

/////////////////////////////////////////////////
QQuickItem *Dialog::RootItem() const
{
  auto dialogItem = this->dataPtr->quickWindow->findChild<QQuickItem *>();
  if (!dialogItem)
  {
    ignerr << "Internal error: Null dialog root item!" << std::endl;
  }

  return dialogItem;
}

/////////////////////////////////////////////////
bool Dialog::UpdateConfigAttribute(const std::string &_path,
  const std::string &_attribute, const bool _value) const
{
  if (_path.empty())
  {
    ignerr << "Missing config file" << std::endl;
    return false;
  }

  // Use tinyxml to read config
  tinyxml2::XMLDocument doc;
  auto success = !doc.LoadFile(_path.c_str());
  if (!success)
  {
      ignerr << "Failed to load file [" << _path << "]: XMLError"
             << std::endl;
    return false;
  }

  // Update attribute value for the correct dialog
  for (auto dialogElem = doc.FirstChildElement("dialog");
    dialogElem != nullptr;
    dialogElem = dialogElem->NextSiblingElement("dialog"))
  {
    if(dialogElem->Attribute("name") == this->objectName().toStdString())
    {
      dialogElem->SetAttribute(_attribute.c_str(), _value);
    }
  }

  // Write config file
  tinyxml2::XMLPrinter printer;
  doc.Print(&printer);

  std::string config = printer.CStr();
  std::ofstream out(_path.c_str(), std::ios::out);
  if (!out)
  {
    ignerr << "Unable to open file: " << _path <<
      ".\nCheck file permissions.\n";
  }
  else
    out << config;

  return true;
}

/////////////////////////////////////////////////
void Dialog::SetDefaultConfig(const std::string &_config)
{
  this->dataPtr->config = _config;
}

/////////////////////////////////////////////////
std::string Dialog::ReadAttributeValue(const std::string &_path,
  const std::string &_attribute) const
{
  tinyxml2::XMLDocument doc;
  std::string value {""};
  std::string config = "<?xml version=\"1.0\"?>\n\n";
  tinyxml2::XMLPrinter defaultPrinter;
  bool configExists{true};

  // Check if the passed in config file exists.
  // (If the default config path doesn't exist yet, it's expected behavior.
  // It will be created the first time now.)
  if (!common::exists(_path))
  {
    configExists = false;
    doc.Parse(this->dataPtr->config.c_str());
    // Process each dialog
    for (auto dialogElem = doc.FirstChildElement("dialog");
      dialogElem != nullptr;
      dialogElem = dialogElem->NextSiblingElement("dialog"))
    {
      if(dialogElem->Attribute("name") == this->objectName().toStdString())
      {
        value = dialogElem->Attribute(_attribute.c_str());
      }
    }
  }
  else
  {
    auto success = !doc.LoadFile(_path.c_str());
    if (!success)
    {
      return "";
    }

    // Process each existing dialog
    for (auto dialogElem = doc.FirstChildElement("dialog");
      dialogElem != nullptr;
      dialogElem = dialogElem->NextSiblingElement("dialog"))
    {
      if(dialogElem->Attribute("name") == this->objectName().toStdString())
      {
        if (dialogElem->Attribute(_attribute.c_str()))
          value = dialogElem->Attribute(_attribute.c_str());
      }
    }

    // config exists but attribute not there read from default config
    if (value.empty())
    {
      tinyxml2::XMLDocument missingDoc;
      missingDoc.Parse(this->dataPtr->config.c_str());

      for (auto dialogElem = doc.FirstChildElement("dialog");
        dialogElem != nullptr;
        dialogElem = dialogElem->NextSiblingElement("dialog"))
        {
          if(dialogElem->Attribute("name") == this->objectName().toStdString())
          {
            if(dialogElem->Attribute(_attribute.c_str()))
              value = dialogElem->Attribute(_attribute.c_str());
          }
        }
      missingDoc.Print(&defaultPrinter);
    }
  }

  // Write config file
  tinyxml2::XMLPrinter printer;
  doc.Print(&printer);

  // Don't write the xml version decleration if file exists
  if (configExists)
  {
    config = "";
  }

  igndbg << "Setting dialog " << this->objectName().toStdString()
    << " default config." << std::endl;
  config += printer.CStr();
  config += defaultPrinter.CStr();
  std::ofstream out(_path.c_str(), std::ios::out);
  if (!out)
  {
    ignerr << "Unable to open file: " << _path <<
      ".\nCheck file permissions.\n";
    return "";
  }
  else
    out << config;

  return value;
}
