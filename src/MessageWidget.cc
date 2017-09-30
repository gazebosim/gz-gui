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

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include <ignition/math/Helpers.hh>

#include <ignition/msgs.hh>

#include <ignition/common/Console.hh>
#include <ignition/common/EnumIface.hh>

#include "ignition/gui/BoolWidget.hh"
#include "ignition/gui/CollapsibleWidget.hh"
#include "ignition/gui/ColorWidget.hh"
#include "ignition/gui/Conversions.hh"
#include "ignition/gui/EnumWidget.hh"
#include "ignition/gui/GeometryWidget.hh"
#include "ignition/gui/Helpers.hh"
#include "ignition/gui/NumberWidget.hh"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/Pose3dWidget.hh"
#include "ignition/gui/QtMetatypes.hh"
#include "ignition/gui/StringWidget.hh"
#include "ignition/gui/Vector3dWidget.hh"

#include "ignition/gui/MessageWidget.hh"

namespace ignition
{
  namespace gui
  {
    /// \brief Private data for the MessageWidget class.
    class MessageWidgetPrivate
    {
      /// \brief A map of unique scoped names to correpsonding widgets.
      public: std::map <std::string, PropertyWidget *> properties;

      /// \brief A copy of the message with fields to be configured by widgets.
      public: google::protobuf::Message *msg = nullptr;
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
MessageWidget::MessageWidget(const google::protobuf::Message *_msg)
  : dataPtr(new MessageWidgetPrivate())
{
  this->dataPtr->msg = _msg->New();
  this->dataPtr->msg->CopyFrom(*_msg);

  // Layout
  auto mainLayout = new QVBoxLayout;
  mainLayout->setAlignment(Qt::AlignTop);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(mainLayout);

  // Generate widgets from the message and add to the layout
  this->Parse(this->dataPtr->msg, "", this);
}

/////////////////////////////////////////////////
MessageWidget::~MessageWidget()
{
  delete this->dataPtr->msg;
}

/////////////////////////////////////////////////
bool MessageWidget::UpdateFromMsg(const google::protobuf::Message *_msg)
{
  if (!this->dataPtr->msg)
  {
    ignerr << "You need to load a message before being able to update"
           << std::endl;
    return false;
  }

  if (!_msg)
  {
    ignwarn << "Null message" << std::endl;
    return false;
  }

  auto currentType = this->dataPtr->msg->GetDescriptor()->full_name();
  auto newType = this->dataPtr->msg->GetDescriptor()->full_name();

  if (currentType != newType)
  {
    ignerr << "Trying to load a [" << newType
           << "] message into a widget previously filled with [" << currentType
           << "]" << std::endl;
    return false;
  }

  this->dataPtr->msg->CopyFrom(*_msg);
  return this->Parse(this->dataPtr->msg, "", this);
}

/////////////////////////////////////////////////
google::protobuf::Message *MessageWidget::Msg()
{
  this->FillMsg(this->dataPtr->msg);
  return this->dataPtr->msg;
}

/////////////////////////////////////////////////
bool MessageWidget::Parse(google::protobuf::Message *_msg,
    const std::string &_scopedName, QWidget *_parent)
{
  auto descriptor = _msg->GetDescriptor();
  if (!descriptor)
  {
    ignerr << "Failed to get message descriptor" << std::endl;
    return false;
  }

  auto propertyWidget = this->PropertyWidgetByName(_scopedName);

  // Geometry
  auto messageType = descriptor->full_name();
  if (messageType == "ignition.msgs.Geometry")
  {
    // Coming soon
    return true;
  }

  // Pose3d
  if (messageType == "ignition.msgs.Pose")
  {
    // Coming soon
    return true;
  }

  // Vector3d
  if (messageType == "ignition.msgs.Vector3d")
  {
    // Coming soon
    return true;
  }

  // Color
  if (messageType == "ignition.msgs.Color")
  {
    // Coming soon
    return true;
  }

  auto reflection = _msg->GetReflection();
  if (!reflection)
  {
    ignerr << "Failed to get message reflection" << std::endl;
    return false;
  }

  // For other message types, recursively parse their fields
  for (int i = 0; i < descriptor->field_count() ; ++i)
  {
    auto fieldDescriptor = descriptor->field(i);

    if (!fieldDescriptor)
    {
      ignerr << "Failed to get field descriptor" << std::endl;
      continue;
    }

    // TODO parse repeated fields
    if (fieldDescriptor->is_repeated())
      continue;

    // Scoped name
    auto fieldName = fieldDescriptor->name();
    auto scopedName = _scopedName.empty() ?
        fieldName : _scopedName + "::" + fieldName;

    // Get the widget if it exists
    propertyWidget = this->PropertyWidgetByName(scopedName);

    // Handle each field type
    auto fieldType = fieldDescriptor->type();

    // Numbers
    if (fieldType == google::protobuf::FieldDescriptor::TYPE_DOUBLE)
    {
      // Coming soon
      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_FLOAT)
    {
      // Coming soon
      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT64)
    {
      // Coming soon
      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT32)
    {
      // Coming soon
      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT64)
    {
      // Coming soon
      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT32)
    {
      // Coming soon
      continue;
    }

    // Boolean
    if (fieldType == google::protobuf::FieldDescriptor::TYPE_BOOL)
    {
      // Coming soon
      continue;
    }

    // String
    if (fieldType == google::protobuf::FieldDescriptor::TYPE_STRING)
    {
      // If creating new widget
      if (!propertyWidget)
      {
        // Choose either a one-line or a multi-line widget according to name
        auto type = StringWidget::StringType::LINE;
        if (fieldName == "innerxml")
          type = StringWidget::StringType::TEXT;

        propertyWidget = new StringWidget(fieldName, type);
        _parent->layout()->addWidget(propertyWidget);
        this->AddPropertyWidget(scopedName, propertyWidget);
      }

      // Set value
      std::string value = reflection->GetString(*_msg, fieldDescriptor);
      propertyWidget->SetValue(QVariant::fromValue(value));

      continue;
    }

    // Enum
    if (fieldType == google::protobuf::FieldDescriptor::TYPE_ENUM)
    {
      // Coming soon
      continue;
    }

    // Nested messages
    if (fieldType == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
    {
      // Get nested message
      auto valueMsg = reflection->MutableMessage(_msg, fieldDescriptor);

      auto collapsible = qobject_cast<CollapsibleWidget *>(propertyWidget);
      if (!collapsible)
      {
        // Create collapsible
        collapsible = new CollapsibleWidget(fieldName);
        _parent->layout()->addWidget(collapsible);
      }

      // Generate / update widget
      this->Parse(valueMsg, scopedName, collapsible);

      // Collapse the first time it was created
      if (!propertyWidget)
      {
        collapsible->Toggle(false);
        // Special messages added themselves
        if (!this->PropertyWidgetByName(scopedName))
          this->AddPropertyWidget(scopedName, collapsible);
      }
    }
  }

  return false;
}

/////////////////////////////////////////////////
bool MessageWidget::FillMsg(google::protobuf::Message *_msg,
    const std::string &_parentScopedName)
{
  // Get descriptor of given message
  auto descriptor = _msg->GetDescriptor();
  if (!descriptor)
    return false;

  // Iterate over its fields
  auto count = descriptor->field_count();
  for (int i = 0; i < count ; ++i)
  {
    auto fieldDescriptor = descriptor->field(i);
    if (!fieldDescriptor)
      continue;

    auto reflection = _msg->GetReflection();
    if (!reflection)
      continue;

    std::string name = fieldDescriptor->name();

    // Update each field in the message
    // TODO update repeated fields
    if (fieldDescriptor->is_repeated())
      continue;

    auto scopedName = _parentScopedName.empty() ? name
        : _parentScopedName + "::" + name;

    // Skip if we don't have a widget with this name
    if (this->dataPtr->properties.find(scopedName) ==
        this->dataPtr->properties.end())
    {
      continue;
    }

    auto childWidget = this->dataPtr->properties[scopedName];
    auto variant = childWidget->Value();

    switch (fieldDescriptor->type())
    {
      // Numbers
      case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
      {
        // Coming soon
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_FLOAT:
      {
        // Coming soon
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT64:
      {
        // Coming soon
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT32:
      {
        // Coming soon
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT64:
      {
        // Coming soon
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT32:
      {
        // Coming soon
        break;
      }
      // Boolean
      case google::protobuf::FieldDescriptor::TYPE_BOOL:
      {
        // Coming soon
        break;
      }
      // String
      case google::protobuf::FieldDescriptor::TYPE_STRING:
      {
        reflection->SetString(_msg, fieldDescriptor,
            variant.value<std::string>());
        break;
      }
      // Enum
      case google::protobuf::FieldDescriptor::TYPE_ENUM:
      {
        // Coming soon
        break;
      }
      // Nested messages
      case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
      {
        // Geometry
        if (fieldDescriptor->message_type()->name() == "Geometry")
        {
          // Coming soon
        }
        // Pose
        else if (fieldDescriptor->message_type()->name() == "Pose")
        {
          // Coming soon
        }
        // Vector3d
        else if (fieldDescriptor->message_type()->name() == "Vector3d")
        {
          // Coming soon
        }
        // Color
        else if (fieldDescriptor->message_type()->name() == "Color")
        {
          // Coming soon
        }
        // Recursively fill other types
        else
        {
          auto valueMsg = (reflection->MutableMessage(_msg, fieldDescriptor));
          this->FillMsg(valueMsg, scopedName);
        }

        break;
      }
      default:
        break;
    }
  }
  return true;
}

/////////////////////////////////////////////////
bool MessageWidget::AddPropertyWidget(const std::string &_name,
    PropertyWidget *_child)
{
  if (_child == nullptr)
  {
    ignerr << "Null child, not adding widget." << std::endl;
    return false;
  }

  if (this->dataPtr->properties.find(_name) != this->dataPtr->properties.end())
  {
    ignerr << "This config widget already has a child named [" << _name << "]. "
       << "Names must be unique. Not adding child." << std::endl;
    return false;
  }

  this->dataPtr->properties[_name] = _child;

  // Forward widget's ValueChanged signal
  this->connect(_child, &PropertyWidget::ValueChanged,
      [this, _name](const QVariant _value)
      {this->ValueChanged(_name, _value);});

  return true;
}

/////////////////////////////////////////////////
PropertyWidget *MessageWidget::PropertyWidgetByName(
    const std::string &_name) const
{
  auto iter = this->dataPtr->properties.find(_name);
  if (iter != this->dataPtr->properties.end())
    return iter->second;

  return nullptr;
}

