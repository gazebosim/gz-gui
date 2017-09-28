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
      public: google::protobuf::Message *msg;
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
MessageWidget::MessageWidget()
  : dataPtr(new MessageWidgetPrivate())
{
  this->dataPtr->msg = nullptr;
  this->setObjectName("configWidget");
}

/////////////////////////////////////////////////
MessageWidget::~MessageWidget()
{
  delete this->dataPtr->msg;
}

/////////////////////////////////////////////////
void MessageWidget::Load(const google::protobuf::Message *_msg)
{
  this->dataPtr->msg = _msg->New();
  this->dataPtr->msg->CopyFrom(*_msg);

  // Generate a widget from the message
  auto widget = this->Parse(this->dataPtr->msg, 0);

  // Layout
  auto mainLayout = new QVBoxLayout;
  mainLayout->setAlignment(Qt::AlignTop);
  mainLayout->addWidget(widget);
  this->setLayout(mainLayout);

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
void MessageWidget::UpdateFromMsg(const google::protobuf::Message *_msg)
{
  this->dataPtr->msg->CopyFrom(*_msg);
  this->Parse(this->dataPtr->msg, true);
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

  auto groupWidget = qobject_cast<CollapsibleWidget *>(w->parent());
  if (groupWidget)
    return groupWidget->isVisible();

  return w->isVisible();
}

/////////////////////////////////////////////////
void MessageWidget::SetWidgetVisible(const std::string &_name,
    const bool _visible)
{
  auto w = this->PropertyWidgetByName(_name);
  if (!w)
    return;

  auto groupWidget = qobject_cast<CollapsibleWidget *>(w->parent());
  if (groupWidget)
    groupWidget->setVisible(_visible);
  else
    w->setVisible(_visible);
}

/////////////////////////////////////////////////
bool MessageWidget::WidgetReadOnly(const std::string &_name) const
{
  auto w = this->PropertyWidgetByName(_name);
  if (!w)
    return false;

  auto groupWidget = qobject_cast<CollapsibleWidget *>(w->parent());
  if (groupWidget)
    return !groupWidget->isEnabled();

  return !w->isEnabled();
}

/////////////////////////////////////////////////
void MessageWidget::SetWidgetReadOnly(const std::string &_name,
    const bool _readOnly)
{
  auto w = this->PropertyWidgetByName(_name);
  if (!w)
    return;

  auto groupWidget = qobject_cast<CollapsibleWidget *>(w->parent());
  if (groupWidget)
  {
    groupWidget->setEnabled(!_readOnly);

    // Qt docs: "Disabling a widget implicitly disables all its children.
    // Enabling respectively enables all child widgets unless they have
    // been explicitly disabled."
    auto childWidgets = groupWidget->findChildren<QWidget *>();
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
QWidget *MessageWidget::Parse(google::protobuf::Message *_msg,
    bool _update, const std::string &_name, const int _level)
{
  std::vector<QWidget *> newWidgets;

  auto d = _msg->GetDescriptor();
  if (!d)
  {
    ignerr << "Failed to get message descriptor" << std::endl;
    return nullptr;
  }

  auto ref = _msg->GetReflection();
  if (!ref)
  {
    ignerr << "Failed to get message reflection" << std::endl;
    return nullptr;
  }

  // FIXME: Does not handle top-level special messages like Vector3d
  for (int i = 0; i < d->field_count() ; ++i)
  {
    auto field = d->field(i);

    if (!field)
      return nullptr;

    // TODO parse repeated fields
    if (field->is_repeated())
      continue;

    auto name = field->name();

    if (_update && !ref->HasField(*_msg, field))
      continue;

    QWidget *newFieldWidget = nullptr;
    PropertyWidget *propertyWidget = nullptr;

    // If updating an existing widget
    bool newWidget = true;
    auto scopedName = _name.empty() ? name : _name + "::" + name;
    if (this->dataPtr->properties.find(scopedName) !=
        this->dataPtr->properties.end())
    {
      newWidget = false;
      propertyWidget = this->dataPtr->properties[scopedName];
    }

    // Handle each field type
    switch (field->type())
    {
      // Numbers
      case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
      {
        double value = ref->GetDouble(*_msg, field);
        if (!math::equal(value, value))
          value = 0;
        if (newWidget)
        {
          propertyWidget = new NumberWidget(name, _level, NumberWidget::DOUBLE);
          newFieldWidget = propertyWidget;
        }

        propertyWidget->SetValue(value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_FLOAT:
      {
        float value = ref->GetFloat(*_msg, field);
        if (!math::equal(value, value))
          value = 0;
        if (newWidget)
        {
          propertyWidget =
              new NumberWidget(name, _level, NumberWidget::DOUBLE);
          newFieldWidget = propertyWidget;
        }

        propertyWidget->SetValue(value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT64:
      {
        int value = ref->GetInt64(*_msg, field);
        if (newWidget)
        {
          propertyWidget = new NumberWidget(name, _level, NumberWidget::INT);
          newFieldWidget = propertyWidget;
        }
        propertyWidget->SetValue(value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT64:
      {
        unsigned int value = ref->GetUInt64(*_msg, field);
        if (newWidget)
        {
          propertyWidget = new NumberWidget(name, _level, NumberWidget::UINT);
          newFieldWidget = propertyWidget;
        }
        propertyWidget->SetValue(value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT32:
      {
        int value = ref->GetInt32(*_msg, field);
        if (newWidget)
        {
          propertyWidget = new NumberWidget(name, _level, NumberWidget::INT);
          newFieldWidget = propertyWidget;
        }
        propertyWidget->SetValue(value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT32:
      {
        unsigned int value = ref->GetUInt32(*_msg, field);
        if (newWidget)
        {
          propertyWidget = new NumberWidget(name, _level, NumberWidget::UINT);
          newFieldWidget = propertyWidget;
        }
        propertyWidget->SetValue(value);
        break;
      }
      // Boolean
      case google::protobuf::FieldDescriptor::TYPE_BOOL:
      {
        bool value = ref->GetBool(*_msg, field);
        if (newWidget)
        {
          propertyWidget = new BoolWidget(name, _level);
          newFieldWidget = propertyWidget;
        }

        propertyWidget->SetValue(value);
        break;
      }
      // String
      case google::protobuf::FieldDescriptor::TYPE_STRING:
      {
        std::string value = ref->GetString(*_msg, field);
        if (newWidget)
        {
          // Choose either a one-line or a multi-line widget according to name
          std::string type = "line";
          if (name == "innerxml")
            type = "plain";

          propertyWidget = new StringWidget(name, _level, type);
          newFieldWidget = propertyWidget;
        }

        QVariant v;
        v.setValue(value);
        propertyWidget->SetValue(v);
        break;
      }
      // Enum
      case google::protobuf::FieldDescriptor::TYPE_ENUM:
      {
        auto value = ref->GetEnum(*_msg, field);

        if (!value)
        {
          ignerr << "Error retrieving enum value for '" << name << "'"
                 << std::endl;
          break;
        }

        if (newWidget)
        {
          // Get values from message descriptor
          std::vector<std::string> enumValues;
          auto descriptor = value->type();
          if (!descriptor)
            break;

          for (int j = 0; j < descriptor->value_count(); ++j)
          {
            auto valueDescriptor = descriptor->value(j);
            if (valueDescriptor)
              enumValues.push_back(valueDescriptor->name());
          }

          // Create enum widget
          propertyWidget = new EnumWidget(name, enumValues, _level);
          newFieldWidget = propertyWidget;
        }
        QVariant v;
        v.setValue(value->name());
        propertyWidget->SetValue(v);
        break;
      }
      // Nested messages
      case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
      {
        auto valueMsg = ref->MutableMessage(_msg, field);

        // Geometry
        if (field->message_type()->name() == "Geometry")
        {
          if (newWidget)
          {
            propertyWidget = new GeometryWidget(name, _level);
            newFieldWidget = propertyWidget;
          }

          auto value = dynamic_cast<msgs::Geometry *>(valueMsg);
          QVariant v;
          v.setValue(*value);
          propertyWidget->SetValue(v);
        }
        // Pose
        else if (field->message_type()->name() == "Pose")
        {
          if (newWidget)
          {
            propertyWidget = new Pose3dWidget(name, _level);
            newFieldWidget = propertyWidget;
          }

          auto poseMsg = dynamic_cast<msgs::Pose *>(valueMsg);
          auto value = msgs::Convert(*poseMsg);

          QVariant v;
          v.setValue(value);
          propertyWidget->SetValue(v);
        }
        // Vector3d
        else if (field->message_type()->name() == "Vector3d")
        {
          if (newWidget)
          {
            propertyWidget = new Vector3dWidget(name, _level);
            newFieldWidget = propertyWidget;
          }

          auto vector3dMsg = dynamic_cast<msgs::Vector3d *>(valueMsg);
          auto value = msgs::Convert(*vector3dMsg);

          QVariant v;
          v.setValue(value);
          propertyWidget->SetValue(v);
        }
        // Color
        else if (field->message_type()->name() == "Color")
        {
          if (newWidget)
          {
            propertyWidget = new ColorWidget(name, _level);
            newFieldWidget = propertyWidget;
          }

          auto colorMsg = dynamic_cast<msgs::Color *>(valueMsg);
          auto value = msgs::Convert(*colorMsg);

          QVariant v;
          v.setValue(value);
          propertyWidget->SetValue(v);
        }
        // Parse other message types recursively
        else
        {
          auto groupBoxWidget =
              this->Parse(valueMsg, _update, scopedName, _level+1);
          if (groupBoxWidget)
          {
            newFieldWidget = new PropertyWidget();
            auto groupBoxLayout = new QVBoxLayout;
            groupBoxLayout->setContentsMargins(0, 0, 0, 0);
            groupBoxLayout->addWidget(groupBoxWidget);
            newFieldWidget->setLayout(groupBoxLayout);
            qobject_cast<PropertyWidget *>(newFieldWidget)->
                widgets.push_back(groupBoxWidget);
          }
        }

        // When creating a new widget, make it collapsible
        if (newWidget)
        {
          auto childWidget = qobject_cast<PropertyWidget *>(newFieldWidget);
          if (childWidget)
          {
            newFieldWidget = new CollapsibleWidget(name, childWidget, _level);
          }
        }

        break;
      }
      default:
      {
        ignwarn << "Skipping field type [" << field->message_type()->name()
                << "]" << std::endl;
      }
        break;
    }

    if (newWidget && newFieldWidget)
    {
      newWidgets.push_back(newFieldWidget);

      // store the newly created widget in a map with a unique scoped name.
      if (qobject_cast<CollapsibleWidget *>(newFieldWidget))
      {
        auto groupWidget = qobject_cast<CollapsibleWidget *>(newFieldWidget);
        auto childWidget = qobject_cast<PropertyWidget *>(
            groupWidget->childWidget);
        this->AddPropertyWidget(scopedName, childWidget);
      }
      else if (qobject_cast<PropertyWidget *>(newFieldWidget))
      {
        this->AddPropertyWidget(scopedName,
            qobject_cast<PropertyWidget *>(newFieldWidget));
      }
    }
  }

  if (!newWidgets.empty())
  {
    auto widgetLayout = new QVBoxLayout;
    widgetLayout->setContentsMargins(0, 0, 0, 0);
    widgetLayout->setSpacing(0);
    widgetLayout->setAlignment(Qt::AlignTop);

    for (auto w : newWidgets)
      widgetLayout->addWidget(w);

    // create a group box to hold child widgets.
    auto widget = new QGroupBox();
    widget->setLayout(widgetLayout);
    return widget;
  }

  // TODO: We actually get here often, should we?
  return nullptr;
}

/////////////////////////////////////////////////
bool MessageWidget::FillMsg(google::protobuf::Message *_msg,
    const std::string &_name)
{
  // Get descriptor of given message
  auto d = _msg->GetDescriptor();
  if (!d)
    return false;

  // Iterate over its fields
  auto count = d->field_count();
  for (int i = 0; i < count ; ++i)
  {
    auto field = d->field(i);
    if (!field)
      continue;

    auto ref = _msg->GetReflection();
    if (!ref)
      continue;

    std::string name = field->name();

    // Update each field in the message
    // TODO update repeated fields
    if (field->is_repeated() /*|| !ref->HasField(*_msg, field)*/)
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

    switch (field->type())
    {
      // Numbers
      case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
      {
        ref->SetDouble(_msg, field, variant.toDouble());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_FLOAT:
      {
        ref->SetFloat(_msg, field, variant.toDouble());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT64:
      {
        ref->SetInt64(_msg, field, variant.toInt());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT64:
      {
        ref->SetUInt64(_msg, field, variant.toUInt());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT32:
      {
        ref->SetInt32(_msg, field, variant.toInt());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT32:
      {
        ref->SetUInt32(_msg, field, variant.toUInt());
        break;
      }
      // Boolean
      case google::protobuf::FieldDescriptor::TYPE_BOOL:
      {
        ref->SetBool(_msg, field, variant.toBool());
        break;
      }
      // String
      case google::protobuf::FieldDescriptor::TYPE_STRING:
      {
        ref->SetString(_msg, field, variant.value<std::string>());
        break;
      }
      // Enum
      case google::protobuf::FieldDescriptor::TYPE_ENUM:
      {
        auto str = variant.value<std::string>();

        // Convert string into protobuf enum
        auto enumDescriptor = field->enum_type();
        if (!enumDescriptor)
          continue;

        auto enumValue = enumDescriptor->FindValueByName(str);
        if (enumValue)
          ref->SetEnum(_msg, field, enumValue);
        else
          ignerr << "Unable to find enum value [" << str << "]" << std::endl;

        break;
      }
      // Nested messages
      case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
      {
        auto mutableMsg = ref->MutableMessage(_msg, field);

        // Geometry
        if (field->message_type()->name() == "Geometry")
        {
          mutableMsg->CopyFrom(variant.value<msgs::Geometry>());
        }
        // Pose
        else if (field->message_type()->name() == "Pose")
        {
          mutableMsg->CopyFrom(msgs::Convert(variant.value<math::Pose3d>()));
        }
        // Vector3d
        else if (field->message_type()->name() == "Vector3d")
        {
          mutableMsg->CopyFrom(msgs::Convert(variant.value<math::Vector3d>()));
        }
        // Color
        else if (field->message_type()->name() == "Color")
        {
          mutableMsg->CopyFrom(msgs::Convert(variant.value<math::Color>()));
        }
        // Recursively fill other types
        else
        {
          auto valueMsg = (ref->MutableMessage(_msg, field));
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
  if (_name.empty() || _child == nullptr)
  {
    ignerr << "Given name or child is invalid. Not adding child widget."
          << std::endl;
    return false;
  }
  if (this->dataPtr->properties.find(_name) !=
      this->dataPtr->properties.end())
  {
    ignerr << "This config widget already has a child with that name. " <<
       "Names must be unique. Not adding child." << std::endl;
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

  ignerr << "Widget [" << _name << "] not found" << std::endl;
  return nullptr;
}

