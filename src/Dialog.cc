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

#include <tinyxml2.h>

#include <gz/common/Console.hh>
#include "gz/gui/Application.hh"
#include "gz/gui/Dialog.hh"

namespace gz::gui
{
class DialogPrivate
{
  /// \brief Pointer to quick window
  public: QQuickWindow *quickWindow{nullptr};
};

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
    // We'd only get here if the QML file is malformed
    // LCOV_EXCL_START
    gzerr << "Internal error: Failed to instantiate QML file [" << qmlFile
           << "]" << std::endl;
    // LCOV_EXCL_STOP
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
    // We'd only get here if the QML file is malformed
    // LCOV_EXCL_START
    gzerr << "Internal error: Null dialog root item!" << std::endl;
    // LCOV_EXCL_STOP
  }

  return dialogItem;
}

/////////////////////////////////////////////////
bool Dialog::UpdateConfigAttribute(const std::string &_path,
  const std::string &_attribute, const bool _value) const
{
  if (!common::exists(_path))
  {
    gzerr << "Missing config file" << std::endl;
    return false;
  }

  // Use tinyxml to read config
  tinyxml2::XMLDocument doc;
  auto success = !doc.LoadFile(_path.c_str());
  if (!success)
  {
    gzerr << "Failed to load file [" << _path << "]: XMLError"
           << std::endl;
    return false;
  }

  // Update attribute value for the correct dialog
  bool updated{false};
  for (auto dialogElem = doc.FirstChildElement("dialog");
    dialogElem != nullptr;
    dialogElem = dialogElem->NextSiblingElement("dialog"))
  {
    if (dialogElem->Attribute("name") == this->objectName().toStdString())
    {
      dialogElem->SetAttribute(_attribute.c_str(), _value);
      updated = true;
    }
  }

  // Create new <dialog> if missing
  if (!updated)
  {
    auto dialogElem = doc.NewElement("dialog");
    dialogElem->SetAttribute("name", this->objectName().toStdString().c_str());
    dialogElem->SetAttribute(_attribute.c_str(), _value);
    doc.InsertEndChild(dialogElem);
  }

  // Write config file
  if (doc.SaveFile(_path.c_str()) != tinyxml2::XML_SUCCESS)
  {
    // LCOV_EXCL_START
    gzerr << "Failed to save file: " << _path
           << ".\nCheck file permissions.\n";
    // LCOV_EXCL_STOP
  }

  return true;
}

/////////////////////////////////////////////////
void Dialog::SetDefaultConfig(const std::string &)
{
  gzwarn << "Dialog::SetDefaultConfig has no effect." << std::endl;
}

/////////////////////////////////////////////////
std::string Dialog::ReadConfigAttribute(const std::string &_path,
  const std::string &_attribute) const
{
  if (!common::exists(_path))
  {
    return std::string();
  }

  tinyxml2::XMLDocument doc;
  auto success = !doc.LoadFile(_path.c_str());
  if (!success)
  {
    gzerr << "Failed to load file [" << _path << "]: XMLError"
          << std::endl;
    return std::string();
  }

  // Process each dialog
  // If multiple attributes share the same name, return the first one
  std::string dialogName = this->objectName().toStdString();
  for (auto dialogElem = doc.FirstChildElement("dialog");
      dialogElem != nullptr;
      dialogElem = dialogElem->NextSiblingElement("dialog"))
  {
    if (dialogElem->Attribute("name") == dialogName &&
        dialogElem->Attribute(_attribute.c_str()))
    {
      return dialogElem->Attribute(_attribute.c_str());
    }
  }

  return std::string();
}
}  // namespace gz::gui
