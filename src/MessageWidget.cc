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

#include <map>
#include <string>

#include <ignition/common/Console.hh>
#include <ignition/common/EnumIface.hh>

#include <ignition/math/Helpers.hh>

#include <ignition/msgs.hh>

#include "ignition/gui/BoolWidget.hh"
#include "ignition/gui/CollapsibleWidget.hh"
#include "ignition/gui/ColorWidget.hh"
#include "ignition/gui/Conversions.hh"
#include "ignition/gui/Enums.hh"
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

      /// \brief A copy of the message used to build the widget. Helps
      /// creating new messages.
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
  if (!_msg)
  {
    ignerr << "Null message passed, widget wasn't properly constructed"
           << std::endl;
    return;
  }

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
    ignerr << "The widget's message is null, this widget is invalid."
           << std::endl;
    return false;
  }

  if (!_msg)
  {
    ignwarn << "Null message, not updating widget" << std::endl;
    return false;
  }

  auto currentType = this->dataPtr->msg->GetDescriptor()->full_name();
  auto newType = _msg->GetDescriptor()->full_name();

  if (currentType != newType)
  {
    ignerr << "Trying to load a [" << newType
           << "] message into a widget previously filled with [" << currentType
           << "]" << std::endl;
    return false;
  }

  return this->Parse(_msg, "", this);
}

/////////////////////////////////////////////////
google::protobuf::Message *MessageWidget::Msg() const
{
  this->FillMsg(this->dataPtr->msg);
  return this->dataPtr->msg;
}

/////////////////////////////////////////////////
bool MessageWidget::Parse(const google::protobuf::Message *_msg,
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
    // If creating new widget
    if (!propertyWidget)
    {
      propertyWidget = new Pose3dWidget();
      this->AddPropertyWidget(_scopedName, propertyWidget, _parent);
    }

    if (auto collapsible = qobject_cast<CollapsibleWidget *>(propertyWidget))
      propertyWidget = collapsible->findChild<PropertyWidget *>();

    // Set value
    auto msg = dynamic_cast<const msgs::Pose *>(_msg);
    propertyWidget->SetValue(QVariant::fromValue(msgs::Convert(*msg)));

    return true;
  }

  // Vector3d
  if (messageType == "ignition.msgs.Vector3d")
  {
    // If creating new widget
    if (!propertyWidget)
    {
      propertyWidget = new Vector3dWidget(descriptor->name());
      this->AddPropertyWidget(_scopedName, propertyWidget, _parent);
    }

    if (auto collapsible = qobject_cast<CollapsibleWidget *>(propertyWidget))
      propertyWidget = collapsible->findChild<PropertyWidget *>();

    // Set value
    auto msg = dynamic_cast<const msgs::Vector3d *>(_msg);
    propertyWidget->SetValue(QVariant::fromValue(msgs::Convert(*msg)));

    return true;
  }

  // Color
  if (messageType == "ignition.msgs.Color")
  {
    // If creating new widget
    if (!propertyWidget)
    {
      propertyWidget = new ColorWidget();
      this->AddPropertyWidget(_scopedName, propertyWidget, _parent);
    }

    if (auto collapsible = qobject_cast<CollapsibleWidget *>(propertyWidget))
      propertyWidget = collapsible->findChild<PropertyWidget *>();

    // Set value
    auto msg = dynamic_cast<const msgs::Color *>(_msg);
    propertyWidget->SetValue(QVariant::fromValue(msgs::Convert(*msg)));

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
      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new NumberWidget(fieldName, NumberType::DOUBLE);
        this->AddPropertyWidget(scopedName, propertyWidget, _parent);
      }

      // Set value
      double value = reflection->GetDouble(*_msg, fieldDescriptor);
      if (!math::equal(value, value))
        value = 0;
      propertyWidget->SetValue(value);

      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_FLOAT)
    {
      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new NumberWidget(fieldName, NumberType::DOUBLE);
        this->AddPropertyWidget(scopedName, propertyWidget, _parent);
      }

      // Set value
      float value = reflection->GetFloat(*_msg, fieldDescriptor);
      if (!math::equal(value, value))
        value = 0;
      propertyWidget->SetValue(value);

      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT64)
    {
      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new NumberWidget(fieldName, NumberType::INT);
        this->AddPropertyWidget(scopedName, propertyWidget, _parent);
      }

      // Set value
      int value = reflection->GetInt64(*_msg, fieldDescriptor);
      propertyWidget->SetValue(value);

      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT32)
    {
      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new NumberWidget(fieldName, NumberType::INT);
        this->AddPropertyWidget(scopedName, propertyWidget, _parent);
      }

      // Set value
      int value = reflection->GetInt32(*_msg, fieldDescriptor);
      propertyWidget->SetValue(value);

      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT64)
    {
      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new NumberWidget(fieldName, NumberType::UINT);
        this->AddPropertyWidget(scopedName, propertyWidget, _parent);
      }

      // Set value
      unsigned int value = reflection->GetUInt64(*_msg, fieldDescriptor);
      propertyWidget->SetValue(value);

      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT32)
    {
      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new NumberWidget(fieldName, NumberType::UINT);
        this->AddPropertyWidget(scopedName, propertyWidget, _parent);
      }

      // Set value
      unsigned int value = reflection->GetUInt32(*_msg, fieldDescriptor);
      propertyWidget->SetValue(value);

      continue;
    }

    // Boolean
    if (fieldType == google::protobuf::FieldDescriptor::TYPE_BOOL)
    {
      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new BoolWidget(fieldName);
        this->AddPropertyWidget(scopedName, propertyWidget, _parent);
      }

      // Set value
      bool value = reflection->GetBool(*_msg, fieldDescriptor);
      propertyWidget->SetValue(value);

      continue;
    }

    // String
    if (fieldType == google::protobuf::FieldDescriptor::TYPE_STRING)
    {
      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new StringWidget(fieldName);
        this->AddPropertyWidget(scopedName, propertyWidget, _parent);
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
      auto &valueMsg = reflection->GetMessage(*_msg, fieldDescriptor);

      // Create collapsible
      auto collapsible = qobject_cast<CollapsibleWidget *>(propertyWidget);
      if (!collapsible)
      {
        collapsible = new CollapsibleWidget(fieldName);
        _parent->layout()->addWidget(collapsible);
      }

      // Generate / update widget
      this->Parse(&valueMsg, scopedName, collapsible);

      // Collapse the first time it was created
      if (!propertyWidget)
      {
        collapsible->Toggle(false);
        this->AddPropertyWidget(scopedName, collapsible, _parent);
      }
    }
  }

  return true;
}

/////////////////////////////////////////////////
bool MessageWidget::FillMsg(google::protobuf::Message *_msg,
    const std::string &_parentScopedName) const
{
  if (!_msg)
    return false;

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
        reflection->SetDouble(_msg, fieldDescriptor, variant.toDouble());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_FLOAT:
      {
        reflection->SetFloat(_msg, fieldDescriptor, variant.toDouble());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT64:
      {
        reflection->SetInt64(_msg, fieldDescriptor, variant.toInt());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT32:
      {
        reflection->SetInt32(_msg, fieldDescriptor, variant.toInt());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT64:
      {
        reflection->SetUInt64(_msg, fieldDescriptor, variant.toUInt());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT32:
      {
        reflection->SetUInt32(_msg, fieldDescriptor, variant.toUInt());
        break;
      }
      // Boolean
      case google::protobuf::FieldDescriptor::TYPE_BOOL:
      {
        reflection->SetBool(_msg, fieldDescriptor, variant.toBool());
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
        auto mutableMsg = reflection->MutableMessage(_msg, fieldDescriptor);

        // Geometry
        if (fieldDescriptor->message_type()->name() == "Geometry")
        {
          // Coming soon
        }
        // Pose
        else if (fieldDescriptor->message_type()->name() == "Pose")
        {
          mutableMsg->CopyFrom(msgs::Convert(variant.value<math::Pose3d>()));
        }
        // Vector3d
        else if (fieldDescriptor->message_type()->name() == "Vector3d")
        {
          mutableMsg->CopyFrom(msgs::Convert(variant.value<math::Vector3d>()));
        }
        // Color
        else if (fieldDescriptor->message_type()->name() == "Color")
        {
          mutableMsg->CopyFrom(msgs::Convert(variant.value<math::Color>()));
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
    PropertyWidget *_property, QWidget *_parent)
{
  if (_property == nullptr)
  {
    ignerr << "Null property, not adding widget." << std::endl;
    return false;
  }

  // If there's already an entry on the map, it will be overriden. This
  // is expected in the case of nested special messages, which are added first
  // to a collapsible and then the collapsible is added to the parent collapsible
  this->dataPtr->properties[_name] = _property;

  // Forward widget's ValueChanged signal
  auto collapsibleSelf = qobject_cast<CollapsibleWidget *>(_property);
  if (!collapsibleSelf)
  {
    this->connect(_property, &PropertyWidget::ValueChanged,
        [this, _name](const QVariant _value)
        {this->ValueChanged(_name, _value);});
  }

  // If inside a collapsible, add indentation
  auto collapsibleParent = qobject_cast<CollapsibleWidget *>(_parent);
  if (collapsibleParent)
  {
    auto hLayout = new QHBoxLayout();
    hLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed,
        QSizePolicy::Fixed));
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    hLayout->addWidget(_property);

    auto w = new QWidget();
    w->setLayout(hLayout);

    _parent->layout()->addWidget(w);
  }
  else
  {
    _parent->layout()->addWidget(_property);
  }

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

