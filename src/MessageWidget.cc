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
#include <regex>
#include <string>
#include <unordered_set>

#include <ignition/common/Console.hh>
#include <ignition/common/EnumIface.hh>

#include <ignition/math/Helpers.hh>

#include <ignition/msgs.hh>

#include "ignition/gui/BoolWidget.hh"
#include "ignition/gui/CollapsibleWidget.hh"
#include "ignition/gui/ColorWidget.hh"
#include "ignition/gui/Conversions.hh"
#include "ignition/gui/Enums.hh"
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
      /// \brief A map of unique scoped names to corresponding widgets.
      public: std::map <std::string, PropertyWidget *> properties;

      /// \brief A copy of the message used to build the widget. Helps
      /// creating new messages.
      public: google::protobuf::Message *msg = nullptr;

      /// \brief Whether all widgets should be read-only.
      public: bool readOnly = false;

      /// \brief List of all properties which should be read-only
      public: std::unordered_set<std::string> readOnlyProperties;

      /// \brief List of all properties which should be hidden
      public: std::unordered_set<std::string> hiddenProperties;

      /// \brief The Ignition Transport topic associated to the topic.
      public: std::string topic = "";
    };
  }
}

/////////////////////////////////////////////////
// Get the family name (remove "::number::" from scoped name for repeated
// fields)
std::string familyName(const std::string &_scopedName)
{
  std::regex regMiddle("::[0-9]+::");
  std::regex regEnd("::[0-9]+$");
  auto familyName = std::regex_replace(_scopedName, regMiddle, "::$2");
  familyName = std::regex_replace(familyName, regEnd, "$2");
  return familyName;
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
bool MessageWidget::PropertyVisible(const std::string &_name) const
{
  auto w = this->PropertyWidgetByName(_name);
  if (!w)
  {
    ignwarn << "Failed to find widget named [" << _name << "]" << std::endl;
    return false;
  }

  return w->isVisible();
}

/////////////////////////////////////////////////
bool MessageWidget::SetPropertyVisible(const std::string &_name,
    const bool _visible)
{
  // Keep list in case widget is added later
  if (!_visible)
    this->dataPtr->hiddenProperties.insert(_name);
  else
    this->dataPtr->hiddenProperties.erase(_name);

  auto w = this->PropertyWidgetByName(_name);
  if (!w)
  {
    bool result = false;

    // Iterate over all properties and affect those which have the same family
    // name
    for (auto p : this->dataPtr->properties)
    {
      if (familyName(p.first) == _name)
      {
        p.second->setVisible(_visible);
        result = true;
      }
    }
    return result;
  }

  w->setVisible(_visible);
  return true;
}

/////////////////////////////////////////////////
bool MessageWidget::ReadOnly() const
{
  // Not read-only if there's at least one enabled widget
  for (auto p : this->dataPtr->properties)
  {
    if (!p.second->ReadOnly())
      return false;
  }

  return true;
}

/////////////////////////////////////////////////
bool MessageWidget::SetReadOnly(const bool _readOnly)
{
  this->dataPtr->readOnly = _readOnly;

  for (auto p : this->dataPtr->properties)
    p.second->SetReadOnly(_readOnly, false);

  return true;
}

/////////////////////////////////////////////////
bool MessageWidget::PropertyReadOnly(const std::string &_name) const
{
  auto w = this->PropertyWidgetByName(_name);
  if (!w)
  {
    ignwarn << "Failed to find widget named [" << _name << "]" << std::endl;
    return false;
  }

  return w->ReadOnly();
}

/////////////////////////////////////////////////
bool MessageWidget::SetPropertyReadOnly(const std::string &_name,
    const bool _readOnly)
{
  // Keep list in case widget is added later
  if (_readOnly)
    this->dataPtr->readOnlyProperties.insert(_name);
  else
    this->dataPtr->readOnlyProperties.erase(_name);

  auto w = this->PropertyWidgetByName(_name);
  if (!w)
  {
    bool result = false;

    // Iterate over all properties and affect those which have the same family
    // name
    for (auto p : this->dataPtr->properties)
    {
      if (familyName(p.first) == _name)
      {
        p.second->SetReadOnly(_readOnly);
        result = true;
      }
    }
    return result;
  }

  w->SetReadOnly(_readOnly);
  return true;
}

/////////////////////////////////////////////////
std::string MessageWidget::Topic() const
{
  return this->dataPtr->topic;
}

/////////////////////////////////////////////////
void MessageWidget::SetTopic(const std::string &_topic)
{
  this->dataPtr->topic = _topic;
}


/////////////////////////////////////////////////
bool MessageWidget::SetPropertyValue(const std::string &_name,
                                     const QVariant _value)
{
  auto w = this->PropertyWidgetByName(_name);
  if (!w)
  {
    ignwarn << "Failed to find widget named [" << _name << "]" << std::endl;
    return false;
  }

  return w->SetValue(_value);
}

/////////////////////////////////////////////////
QVariant MessageWidget::PropertyValue(const std::string &_name) const
{
  auto w = this->PropertyWidgetByName(_name);
  if (!w)
    return QVariant();

  return w->Value();
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
    // If creating new widget
    if (!propertyWidget)
    {
      propertyWidget = new GeometryWidget();
      this->AddPropertyWidget(_scopedName, propertyWidget, _parent);
    }

    if (auto collapsible = qobject_cast<CollapsibleWidget *>(propertyWidget))
      propertyWidget = collapsible->findChild<PropertyWidget *>();

    // Set value
    auto msg = dynamic_cast<const msgs::Geometry *>(_msg);
    propertyWidget->SetValue(QVariant::fromValue(*msg));

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

    // Scoped name
    auto fieldName = fieldDescriptor->name();
    auto scopedName = _scopedName.empty() ?
        fieldName : _scopedName + "::" + fieldName;

    // Get the widget if it exists
    propertyWidget = this->PropertyWidgetByName(scopedName);

    // Handle each field type
    auto fieldType = fieldDescriptor->type();

    // Repeated fields
    if (fieldDescriptor->is_repeated())
    {
      // Create collapsible
      auto collapsible = qobject_cast<CollapsibleWidget *>(propertyWidget);
      if (!collapsible)
      {
        collapsible = new CollapsibleWidget(fieldName);
        _parent->layout()->addWidget(collapsible);
      }

      // Parse all fields in the message
      int count = 0;
      for (; count < reflection->FieldSize(*_msg, fieldDescriptor); ++count)
      {
        // Append number to name
        auto name = scopedName + "::" + std::to_string(count);

        // Get widget
        auto repProp = this->PropertyWidgetByName(name);

        // If it's a repeated message
        if (fieldType == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
        {
          // Create a collapsible per repetition
          auto repCollapsible = qobject_cast<CollapsibleWidget *>(repProp);
          if (!repCollapsible)
          {
            repCollapsible = new CollapsibleWidget(std::to_string(count));
            collapsible->layout()->addWidget(repCollapsible);
          }

          // Parse message
          auto &valueMsg = reflection->GetRepeatedMessage(*_msg,
              fieldDescriptor, count);
          this->Parse(&valueMsg, name, repCollapsible);

          // Collapse the first time it was created
          if (!repProp)
          {
            repCollapsible->Toggle(false);
            this->AddPropertyWidget(name, repCollapsible, collapsible);
          }
        }
        // Numbers
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_DOUBLE)
        {
          // If creating new widget
          if (!repProp)
          {
            repProp = new NumberWidget(std::to_string(count),
                NumberType::DOUBLE);
            this->AddPropertyWidget(name, repProp, collapsible);
          }

          // Set value
          auto value = reflection->GetRepeatedDouble(*_msg, fieldDescriptor,
            count);
          if (!math::equal(value, value))
            value = 0;
          repProp->SetValue(value);
        }
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_FLOAT)
        {
          // If creating new widget
          if (!repProp)
          {
            repProp = new NumberWidget(std::to_string(count),
                NumberType::DOUBLE);
            this->AddPropertyWidget(name, repProp, collapsible);
          }

          // Set value
          auto value = reflection->GetRepeatedFloat(*_msg, fieldDescriptor,
            count);
          if (!math::equal(value, value))
            value = 0;
          repProp->SetValue(value);
        }
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT64)
        {
          // If creating new widget
          if (!repProp)
          {
            repProp = new NumberWidget(std::to_string(count), NumberType::INT);
            this->AddPropertyWidget(name, repProp, collapsible);
          }

          // Set value
          int value = reflection->GetRepeatedInt64(*_msg, fieldDescriptor,
            count);
          repProp->SetValue(value);
        }
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT32)
        {
          // If creating new widget
          if (!repProp)
          {
            repProp = new NumberWidget(std::to_string(count), NumberType::INT);
            this->AddPropertyWidget(name, repProp, collapsible);
          }

          // Set value
          auto value = reflection->GetRepeatedInt32(*_msg, fieldDescriptor,
            count);
          repProp->SetValue(value);
        }
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT64)
        {
          // If creating new widget
          if (!repProp)
          {
            repProp = new NumberWidget(std::to_string(count), NumberType::UINT);
            this->AddPropertyWidget(name, repProp, collapsible);
          }

          // Set value
          unsigned int value = reflection->GetRepeatedUInt64(*_msg,
              fieldDescriptor, count);
          repProp->SetValue(value);
        }
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT32)
        {
          // If creating new widget
          if (!repProp)
          {
            repProp = new NumberWidget(std::to_string(count), NumberType::UINT);
            this->AddPropertyWidget(name, repProp, collapsible);
          }

          // Set value
          auto value = reflection->GetRepeatedUInt32(*_msg, fieldDescriptor,
            count);
          repProp->SetValue(value);
        }
        // Boolean
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_BOOL)
        {
          // If creating new widget
          if (!repProp)
          {
            repProp = new BoolWidget(std::to_string(count));
            this->AddPropertyWidget(name, repProp, collapsible);
          }

          // Set value
          auto value = reflection->GetRepeatedBool(*_msg, fieldDescriptor,
            count);
          repProp->SetValue(value);
        }
        // String
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_STRING)
        {
          // If creating new widget
          if (!repProp)
          {
            repProp = new StringWidget(std::to_string(count));
            this->AddPropertyWidget(name, repProp, collapsible);
          }

          // Set value
          auto value = reflection->GetRepeatedString(*_msg,
            fieldDescriptor, count);
          repProp->SetValue(QVariant::fromValue(value));
        }
        // Enum
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_ENUM)
        {
          // Value from field
          auto value = reflection->GetRepeatedEnum(*_msg, fieldDescriptor,
            count);

          // If creating new widget
          if (!repProp)
          {
            // Get all possible enum values
            std::vector<std::string> enumValues;
            auto enumDescriptor = value->type();
            for (int j = 0; j < enumDescriptor->value_count(); ++j)
            {
              auto valueDescriptor = enumDescriptor->value(j);
              if (valueDescriptor)
                enumValues.push_back(valueDescriptor->name());
            }

            repProp = new EnumWidget(std::to_string(count), enumValues);
            this->AddPropertyWidget(name, repProp, collapsible);
          }

          // Set value
          repProp->SetValue(QVariant::fromValue(value->name()));
        }
        // Others
        else
        {
          ignwarn << "Unhandled message type [" << fieldType << "]"
                  << std::endl;
        }
      }

      // Drop repetitions which disappeared
      auto layoutCount = collapsible->ContentCount();
      for (; count < static_cast<int>(layoutCount); ++count)
      {
        auto name = scopedName + "::" + std::to_string(count);
        this->RemovePropertyWidget(name);
      }

      // Collapse the first time it was created
      if (!propertyWidget)
      {
        collapsible->Toggle(false);
        this->AddPropertyWidget(scopedName, collapsible, _parent);
      }

      continue;
    }

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
      // Value from field
      auto value = reflection->GetEnum(*_msg, fieldDescriptor);

      // If creating new widget
      if (!propertyWidget)
      {
        // Get all possible enum values
        std::vector<std::string> enumValues;
        auto enumDescriptor = value->type();
        for (int j = 0; j < enumDescriptor->value_count(); ++j)
        {
          auto valueDescriptor = enumDescriptor->value(j);
          if (valueDescriptor)
            enumValues.push_back(valueDescriptor->name());
        }

        // Create enum widget
        propertyWidget = new EnumWidget(fieldName, enumValues);
        this->AddPropertyWidget(scopedName, propertyWidget, _parent);
      }

      // Set value
      propertyWidget->SetValue(QVariant::fromValue(value->name()));

      continue;
    }

    // Nested messages
    if (fieldType == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
    {
      // Create collapsible
      auto collapsible = qobject_cast<CollapsibleWidget *>(propertyWidget);
      if (!collapsible)
      {
        collapsible = new CollapsibleWidget(fieldName);
        _parent->layout()->addWidget(collapsible);
      }

      // Generate / update widget from message
      auto &valueMsg = reflection->GetMessage(*_msg, fieldDescriptor);
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

  _msg->Clear();

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
    auto fieldType = fieldDescriptor->type();

    // Handle repeated fields and repeated nested messages
    if (fieldDescriptor->is_repeated())
    {
      int c = 0;
      while (auto prop = this->PropertyWidgetByName(scopedName + "::" +
          std::to_string(c)))
      {
        variant = prop->Value();

        if (fieldType == google::protobuf::FieldDescriptor::TYPE_DOUBLE)
        {
          reflection->AddDouble(_msg, fieldDescriptor, variant.toDouble());
        }
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_FLOAT)
        {
          reflection->AddFloat(_msg, fieldDescriptor, variant.toFloat());
        }
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT64)
        {
          reflection->AddInt64(_msg, fieldDescriptor, variant.toInt());
        }
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT32)
        {
          reflection->AddInt32(_msg, fieldDescriptor, variant.toInt());
        }
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT64)
        {
          reflection->AddUInt64(_msg, fieldDescriptor, variant.toUInt());
        }
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT32)
        {
          reflection->AddUInt32(_msg, fieldDescriptor, variant.toUInt());
        }
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_BOOL)
        {
          reflection->AddBool(_msg, fieldDescriptor, variant.toBool());
        }
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_STRING)
        {
          reflection->AddString(_msg, fieldDescriptor,
              variant.value<std::string>());
        }
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_ENUM)
        {
          auto str = variant.value<std::string>();

          // Convert string into protobuf enum
          auto enumDescriptor = fieldDescriptor->enum_type();
          if (!enumDescriptor)
          {
            ignwarn << "Failed to get enum descriptor." << std::endl;
            continue;
          }

          auto enumValue = enumDescriptor->FindValueByName(str);
          if (enumValue)
            reflection->AddEnum(_msg, fieldDescriptor, enumValue);
          else
          {
            ignwarn << "Unable to find enum value [" << str << "]" << std::endl;
            continue;
          }
        }
        else if (fieldType == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
        {
          auto msg = reflection->AddMessage(_msg, fieldDescriptor);
          this->FillMsg(msg, scopedName + "::" + std::to_string(c));
        }
        else
        {
          ignwarn << "Unhandled field type [" << fieldType << "]" << std::endl;
        }

        c++;
      }
      continue;
    }

    // Numbers
    if (fieldType == google::protobuf::FieldDescriptor::TYPE_DOUBLE)
    {
      reflection->SetDouble(_msg, fieldDescriptor, variant.toDouble());
    }
    else if (fieldType == google::protobuf::FieldDescriptor::TYPE_FLOAT)
    {
      reflection->SetFloat(_msg, fieldDescriptor, variant.toFloat());
    }
    else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT64)
    {
      reflection->SetInt64(_msg, fieldDescriptor, variant.toInt());
    }
    else if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT32)
    {
      reflection->SetInt32(_msg, fieldDescriptor, variant.toInt());
    }
    else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT64)
    {
      reflection->SetUInt64(_msg, fieldDescriptor, variant.toUInt());
    }
    else if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT32)
    {
      reflection->SetUInt32(_msg, fieldDescriptor, variant.toUInt());
    }
    // Boolean
    else if (fieldType == google::protobuf::FieldDescriptor::TYPE_BOOL)
    {
      reflection->SetBool(_msg, fieldDescriptor, variant.toBool());
    }
    // String
    else if (fieldType == google::protobuf::FieldDescriptor::TYPE_STRING)
    {
      reflection->SetString(_msg, fieldDescriptor,
          variant.value<std::string>());
    }
    // Enum
    else if (fieldType == google::protobuf::FieldDescriptor::TYPE_ENUM)
    {
      auto str = variant.value<std::string>();

      // Convert string into protobuf enum
      auto enumDescriptor = fieldDescriptor->enum_type();
      if (!enumDescriptor)
        continue;

      auto enumValue = enumDescriptor->FindValueByName(str);
      if (enumValue)
        reflection->SetEnum(_msg, fieldDescriptor, enumValue);
      else
        ignerr << "Unable to find enum value [" << str << "]" << std::endl;
    }
    // Nested messages
    else if (fieldType == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
    {
      auto mutableMsg = reflection->MutableMessage(_msg, fieldDescriptor);

      // Geometry
      if (fieldDescriptor->message_type()->name() == "Geometry")
      {
        mutableMsg->CopyFrom(variant.value<msgs::Geometry>());
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
  // to a collapsible and then the collapsible is added to the parent
  // collapsible
  this->dataPtr->properties[_name] = _property;

  // Replace :: with /
  std::string adjustedScopedName;
  ignition::common::replaceAll(adjustedScopedName, _name, "::", "/");

  // Needed for drag and drop.
  std::string uri = this->Topic() + "?p=/" + adjustedScopedName;
  _property->SetDragAndDropURI(uri);

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

    collapsibleParent->AppendContent(w);
  }
  else
  {
    _parent->layout()->addWidget(_property);
  }

  // Read only and visibility
  auto family = familyName(_name);

  if (this->dataPtr->readOnlyProperties.find(family) !=
      this->dataPtr->readOnlyProperties.end())
  {
    _property->SetReadOnly(true);
  }
  else if (this->dataPtr->readOnly)
  {
    _property->SetReadOnly(true, false);
  }

  _property->setVisible(this->dataPtr->hiddenProperties.find(family) ==
      this->dataPtr->hiddenProperties.end());

  return true;
}

/////////////////////////////////////////////////
bool MessageWidget::RemovePropertyWidget(const std::string &_name)
{
  auto widget = this->PropertyWidgetByName(_name);

  if (!widget)
    return false;

  // Remove all its children from property list
  for (const auto &prop : this->dataPtr->properties)
  {
    if (prop.first.find(_name) == 0)
      this->dataPtr->properties.erase(prop.first);
  }

  QWidget *toDelete = widget;

  if (!qobject_cast<PropertyWidget *>(widget->parent()))
    toDelete = qobject_cast<QWidget *>(widget->parent());

  toDelete->setParent(nullptr);
  toDelete->deleteLater();

  return true;
}

/////////////////////////////////////////////////
unsigned int MessageWidget::PropertyWidgetCount() const
{
  return this->dataPtr->properties.size();
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

