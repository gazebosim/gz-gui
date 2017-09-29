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
  this->setLayout(mainLayout);

  // Generate widgets from the message and add to the layout
  this->Parse(this->dataPtr->msg, "", this);

  // set up event filter for scrollable widgets to make sure they don't steal
  // focus when embedded in a QScrollArea.
  auto spinBoxes = this->findChildren<QAbstractSpinBox *>();
  for (int i = 0; i < spinBoxes.size(); ++i)
  {
    spinBoxes[i]->installEventFilter(this);
    spinBoxes[i]->setFocusPolicy(Qt::StrongFocus);
  }
  auto comboBoxes = this->findChildren<QComboBox *>();
  for (int i = 0; i < comboBoxes.size(); ++i)
  {
    comboBoxes[i]->installEventFilter(this);
    comboBoxes[i]->setFocusPolicy(Qt::StrongFocus);
  }
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
  // Update message with current widget state
  this->FillMsg(this->dataPtr->msg);

  return this->dataPtr->msg;
}

/////////////////////////////////////////////////
bool MessageWidget::WidgetVisible(const std::string &_name) const
{
  auto w = this->PropertyWidgetByName(_name);
  if (!w)
    return false;

  auto collapsbleParent = qobject_cast<CollapsibleWidget *>(w->parent());
  if (collapsbleParent)
    return collapsbleParent->isVisible();

  return w->isVisible();
}

/////////////////////////////////////////////////
void MessageWidget::SetWidgetVisible(const std::string &_name,
    const bool _visible)
{
  auto w = this->PropertyWidgetByName(_name);
  if (!w)
    return;

  auto collapsbleParent = qobject_cast<CollapsibleWidget *>(w->parent());
  if (collapsbleParent)
    collapsbleParent->setVisible(_visible);
  else
    w->setVisible(_visible);
}

/////////////////////////////////////////////////
bool MessageWidget::ReadOnly() const
{
  // Not read-only if there's at least one enabled widget
  for (auto p : this->dataPtr->properties)
  {
    auto collapsible = qobject_cast<CollapsibleWidget *>(p.second);
    if (!collapsible && p.second->isEnabled())
      return false;
  }

  return true;
}

/////////////////////////////////////////////////
void MessageWidget::SetReadOnly(const bool _readOnly)
{
  for (auto p : this->dataPtr->properties)
  {
    auto collapsible = qobject_cast<CollapsibleWidget *>(p.second);
    if (!collapsible)
      p.second->setEnabled(!_readOnly);
  }
}

/////////////////////////////////////////////////
bool MessageWidget::WidgetReadOnly(const std::string &_name) const
{
  auto w = this->PropertyWidgetByName(_name);
  if (!w)
    return false;

  auto collapsbleParent = qobject_cast<CollapsibleWidget *>(w->parent());
  if (collapsbleParent)
    return !collapsbleParent->isEnabled();

  return !w->isEnabled();
}

/////////////////////////////////////////////////
void MessageWidget::SetWidgetReadOnly(const std::string &_name,
    const bool _readOnly)
{
  auto w = this->PropertyWidgetByName(_name);
  if (!w)
    return;

  auto collapsbleParent = qobject_cast<CollapsibleWidget *>(w->parent());
  if (collapsbleParent)
  {
    collapsbleParent->setEnabled(!_readOnly);

    // Qt docs: "Disabling a widget implicitly disables all its children.
    // Enabling respectively enables all child widgets unless they have
    // been explicitly disabled."
    auto childWidgets = collapsbleParent->findChildren<QWidget *>();
    for (auto widget : childWidgets)
      widget->setEnabled(!_readOnly);

    return;
  }
  w->setEnabled(!_readOnly);
}

/////////////////////////////////////////////////
bool MessageWidget::SetPropertyValue(const std::string &_name,
                                     const QVariant _value)
{
  auto w = this->PropertyWidgetByName(_name);
  if (!w)
    return false;

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
    // Value from msg
    auto msg = dynamic_cast<msgs::Geometry *>(_msg);
    QVariant variant;
    variant.setValue(*msg);

    // If creating new widget
    if (!propertyWidget)
    {
      propertyWidget = new GeometryWidget();
      _parent->layout()->addWidget(propertyWidget);
      this->AddPropertyWidget(_scopedName, propertyWidget);
    }

    // Set value
    propertyWidget->SetValue(variant);

    return true;
  }

  // Pose3d
  if (messageType == "ignition.msgs.Pose")
  {
    // Value from msg
    auto msg = dynamic_cast<msgs::Pose *>(_msg);
    QVariant v;
    v.setValue(msgs::Convert(*msg));

    // If creating new widget
    if (!propertyWidget)
    {
      propertyWidget = new Pose3dWidget();
      _parent->layout()->addWidget(propertyWidget);
      this->AddPropertyWidget(_scopedName, propertyWidget);
    }

    // Set value
    propertyWidget->SetValue(v);

    return true;
  }

  // Vector3d
  if (messageType == "ignition.msgs.Vector3d")
  {
    // Value from msg
    auto msg = dynamic_cast<msgs::Vector3d *>(_msg);
    QVariant v;
    v.setValue(msgs::Convert(*msg));

    // If creating new widget
    if (!propertyWidget)
    {
      propertyWidget = new Vector3dWidget(descriptor->name());
      _parent->layout()->addWidget(propertyWidget);
      this->AddPropertyWidget(_scopedName, propertyWidget);
    }

    // Set value
    propertyWidget->SetValue(v);

    return true;
  }

  // Color
  if (messageType == "ignition.msgs.Color")
  {
    // Value from msg
    auto msg = dynamic_cast<msgs::Color *>(_msg);
    QVariant v;
    v.setValue(msgs::Convert(*msg));

    // If creating new widget
    if (!propertyWidget)
    {
      propertyWidget = new ColorWidget();
      _parent->layout()->addWidget(propertyWidget);
      this->AddPropertyWidget(_scopedName, propertyWidget);
    }

    // Set value
    propertyWidget->SetValue(v);

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
      // Value from field
      double value = reflection->GetDouble(*_msg, fieldDescriptor);
      if (!math::equal(value, value))
        value = 0;

      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new NumberWidget(fieldName, 0, NumberWidget::DOUBLE);
        _parent->layout()->addWidget(propertyWidget);
        this->AddPropertyWidget(scopedName, propertyWidget);
      }

      // Set value
      propertyWidget->SetValue(value);

      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_FLOAT)
    {
      // Value from field
      float value = reflection->GetFloat(*_msg, fieldDescriptor);
      if (!math::equal(value, value))
        value = 0;

      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new NumberWidget(fieldName, 0, NumberWidget::DOUBLE);
        _parent->layout()->addWidget(propertyWidget);
        this->AddPropertyWidget(scopedName, propertyWidget);
      }

      // Set value
      propertyWidget->SetValue(value);

      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT64)
    {
      // Value from field
      int value = reflection->GetInt64(*_msg, fieldDescriptor);

      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new NumberWidget(fieldName, 0, NumberWidget::INT);
        _parent->layout()->addWidget(propertyWidget);
        this->AddPropertyWidget(scopedName, propertyWidget);
      }

      // Set value
      propertyWidget->SetValue(value);

      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT64)
    {
      // Value from field
      unsigned int value = reflection->GetUInt64(*_msg, fieldDescriptor);

      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new NumberWidget(fieldName, 0, NumberWidget::UINT);
        _parent->layout()->addWidget(propertyWidget);
        this->AddPropertyWidget(scopedName, propertyWidget);
      }

      // Set value
      propertyWidget->SetValue(value);

      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_INT32)
    {
      // Value from field
      int value = reflection->GetInt32(*_msg, fieldDescriptor);

      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new NumberWidget(fieldName, 0, NumberWidget::INT);
        _parent->layout()->addWidget(propertyWidget);
        this->AddPropertyWidget(scopedName, propertyWidget);
      }

      // Set value
      propertyWidget->SetValue(value);

      continue;
    }

    if (fieldType == google::protobuf::FieldDescriptor::TYPE_UINT32)
    {
      // Value from field
      unsigned int value = reflection->GetUInt32(*_msg, fieldDescriptor);

      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new NumberWidget(fieldName, 0, NumberWidget::UINT);
        _parent->layout()->addWidget(propertyWidget);
        this->AddPropertyWidget(scopedName, propertyWidget);
      }

      // Set value
      propertyWidget->SetValue(value);

      continue;
    }

    // Boolean
    if (fieldType == google::protobuf::FieldDescriptor::TYPE_BOOL)
    {
      // Value from field
      bool value = reflection->GetBool(*_msg, fieldDescriptor);

      // If creating new widget
      if (!propertyWidget)
      {
        propertyWidget = new BoolWidget(fieldName, 0);
        _parent->layout()->addWidget(propertyWidget);
        this->AddPropertyWidget(scopedName, propertyWidget);
      }

      // Set value
      propertyWidget->SetValue(value);

      continue;
    }

    // String
    if (fieldType == google::protobuf::FieldDescriptor::TYPE_STRING)
    {
      // Value from field
      std::string value = reflection->GetString(*_msg, fieldDescriptor);
      QVariant v;
      v.setValue(value);

      // If creating new widget
      if (!propertyWidget)
      {
        // Choose either a one-line or a multi-line widget according to name
        std::string type = "line";
        if (fieldName == "innerxml")
          type = "plain";

        propertyWidget = new StringWidget(fieldName, 0, type);
        _parent->layout()->addWidget(propertyWidget);
        this->AddPropertyWidget(scopedName, propertyWidget);
      }

      // Set value
      propertyWidget->SetValue(v);

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
        propertyWidget = new EnumWidget(fieldName, enumValues, 0);
        _parent->layout()->addWidget(propertyWidget);
        this->AddPropertyWidget(scopedName, propertyWidget);
      }

      // Set value
      QVariant v;
      v.setValue(value->name());
      propertyWidget->SetValue(v);

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
    const std::string &_name)
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

    std::string scopedName = _name.empty() ? name : _name + "::" + name;
    if (this->dataPtr->properties.find(scopedName) ==
        this->dataPtr->properties.end())
      continue;

    // don't update msgs field that are associated with read-only widgets
    if (this->WidgetReadOnly(scopedName))
      continue;

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
      case google::protobuf::FieldDescriptor::TYPE_UINT64:
      {
        reflection->SetUInt64(_msg, fieldDescriptor, variant.toUInt());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT32:
      {
        reflection->SetInt32(_msg, fieldDescriptor, variant.toInt());
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

        break;
      }
      // Nested messages
      case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
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
  if (this->dataPtr->properties.find(_name) !=
      this->dataPtr->properties.end())
  {
    ignerr << "This config widget already has a child named [" << _name << "]. "
       << "Names must be unique. Not adding child." << std::endl;
    return false;
  }

  _child->scopedName = _name;
  this->dataPtr->properties[_name] = _child;

  // Forward widget's ValueChanged signal
  this->connect(_child, &PropertyWidget::ValueChanged,
      [this, _name](const QVariant _value)
      {this->ValueChanged(_name, _value);});

  return true;
}

/////////////////////////////////////////////////
unsigned int MessageWidget::PropertyWidgetCount() const
{
  return this->dataPtr->properties.size();
}

/////////////////////////////////////////////////
bool MessageWidget::eventFilter(QObject *_obj, QEvent *_event)
{
  // Only handle spins and combos
  auto spinBox = qobject_cast<QAbstractSpinBox *>(_obj);
  auto comboBox = qobject_cast<QComboBox *>(_obj);
  if (spinBox || comboBox)
  {
    QWidget *widget = qobject_cast<QWidget *>(_obj);
    if (_event->type() == QEvent::Wheel)
    {
      if (widget->focusPolicy() == Qt::WheelFocus)
      {
        _event->accept();
        return false;
      }
      else
      {
        _event->ignore();
        return true;
      }
    }
    else if (_event->type() == QEvent::FocusIn)
    {
      widget->setFocusPolicy(Qt::WheelFocus);
    }
    else if (_event->type() == QEvent::FocusOut)
    {
      widget->setFocusPolicy(Qt::StrongFocus);
    }
  }
  return QObject::eventFilter(_obj, _event);
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

