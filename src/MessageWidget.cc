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

#include <ignition/common/Console.hh>
#include <ignition/common/EnumIface.hh>

#include "ignition/gui/BoolWidget.hh"
#include "ignition/gui/CollapsibleWidget.hh"
#include "ignition/gui/ColorWidget.hh"
#include "ignition/gui/Conversions.hh"
#include "ignition/gui/DensityWidget.hh"
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
      public: std::map <std::string, PropertyWidget *> configWidgets;

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

  auto widget = this->Parse(this->dataPtr->msg, 0);
  auto mainLayout = new QVBoxLayout;
  mainLayout->setAlignment(Qt::AlignTop);
  mainLayout->addWidget(widget);

  this->setLayout(mainLayout);

  // set up event filter for scrollable widgets to make sure they don't steal
  // focus when embedded in a QScrollArea.
  QList<QAbstractSpinBox *> spinBoxes =
      this->findChildren<QAbstractSpinBox *>();
  for (int i = 0; i < spinBoxes.size(); ++i)
  {
    spinBoxes[i]->installEventFilter(this);
    spinBoxes[i]->setFocusPolicy(Qt::StrongFocus);
  }
  QList<QComboBox *> comboBoxes =
      this->findChildren<QComboBox *>();
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
  this->UpdateMsg(this->dataPtr->msg);
  return this->dataPtr->msg;
}

/////////////////////////////////////////////////
bool MessageWidget::WidgetVisible(const std::string &_name) const
{
  auto iter = this->dataPtr->configWidgets.find(_name);
  if (iter != this->dataPtr->configWidgets.end())
  {
    auto groupWidget = qobject_cast<CollapsibleWidget *>(iter->second->parent());
    if (groupWidget)
      return groupWidget->isVisible();

    return iter->second->isVisible();
  }
  return false;
}

/////////////////////////////////////////////////
void MessageWidget::SetWidgetVisible(const std::string &_name, bool _visible)
{
  auto iter = this->dataPtr->configWidgets.find(_name);
  if (iter != this->dataPtr->configWidgets.end())
  {
    auto groupWidget = qobject_cast<CollapsibleWidget *>(iter->second->parent());
    if (groupWidget)
    {
      groupWidget->setVisible(_visible);
      return;
    }
    iter->second->setVisible(_visible);
  }
}

/////////////////////////////////////////////////
bool MessageWidget::WidgetReadOnly(const std::string &_name) const
{
  auto iter = this->dataPtr->configWidgets.find(_name);
  if (iter != this->dataPtr->configWidgets.end())
  {
    auto groupWidget = qobject_cast<CollapsibleWidget *>(iter->second->parent());
    if (groupWidget)
      return !groupWidget->isEnabled();

    return !iter->second->isEnabled();
  }
  return false;
}

/////////////////////////////////////////////////
void MessageWidget::SetWidgetReadOnly(const std::string &_name, bool _readOnly)
{
  auto iter = this->dataPtr->configWidgets.find(_name);
  if (iter != this->dataPtr->configWidgets.end())
  {
    auto groupWidget = qobject_cast<CollapsibleWidget *>(iter->second->parent());
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
    iter->second->setEnabled(!_readOnly);
  }
}

/////////////////////////////////////////////////
bool MessageWidget::SetPropertyValue(const std::string &_name,
    const QVariant _value)
{
  auto iter = this->dataPtr->configWidgets.find(_name);
  if (iter == this->dataPtr->configWidgets.end())
    return false;

  return iter->second->SetValue(_value);
}

/////////////////////////////////////////////////
QVariant MessageWidget::PropertyValue(const std::string &_name) const
{
  std::map <std::string, PropertyWidget *>::const_iterator iter =
      this->dataPtr->configWidgets.find(_name);

  if (iter == this->dataPtr->configWidgets.end())
    return QVariant();

  return iter->second->Value();
}

/////////////////////////////////////////////////
QWidget *MessageWidget::Parse(google::protobuf::Message *_msg,
  bool _update, const std::string &_name, const int _level)
{
  std::vector<QWidget *> newWidgets;

  auto d = _msg->GetDescriptor();
  if (!d)
    return nullptr;
  unsigned int count = d->field_count();

  // FIXME: Does not handle top-level special messages like Vector3d
  for (unsigned int i = 0; i < count ; ++i)
  {
    auto field = d->field(i);

    if (!field)
      return nullptr;

    const google::protobuf::Reflection *ref = _msg->GetReflection();

    if (!ref)
      return nullptr;

    std::string name = field->name();

    // Parse each field in the message
    // TODO parse repeated fields
    if (field->is_repeated())
      continue;

    if (_update && !ref->HasField(*_msg, field))
      continue;

    QWidget *newFieldWidget = nullptr;
    PropertyWidget *configChildWidget = nullptr;

    bool newWidget = true;
    std::string scopedName = _name.empty() ? name : _name + "::" + name;
    if (this->dataPtr->configWidgets.find(scopedName) !=
        this->dataPtr->configWidgets.end())
    {
      newWidget = false;
      configChildWidget = this->dataPtr->configWidgets[scopedName];
    }

    switch (field->type())
    {
      case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
      {
        double value = ref->GetDouble(*_msg, field);
        if (!math::equal(value, value))
          value = 0;
        if (newWidget)
        {
          configChildWidget =
              new NumberWidget(name, _level, NumberWidget::DOUBLE);

          // TODO: handle this better
//          if (name == "mass")
//          {
//            auto valueSpinBox = qobject_cast<QDoubleSpinBox *>(
//                configChildWidget->widgets[0]);
//            if (valueSpinBox)
//            {
//              this->connect(valueSpinBox, SIGNAL(valueChanged(double)),
//                  this, SLOT(OnMassValueChanged(double)));
//            }
//          }
          newFieldWidget = configChildWidget;
        }

        configChildWidget->SetValue(value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_FLOAT:
      {
        float value = ref->GetFloat(*_msg, field);
        if (!math::equal(value, value))
          value = 0;
        if (newWidget)
        {
          configChildWidget =
              new NumberWidget(name, _level, NumberWidget::DOUBLE);
          newFieldWidget = configChildWidget;
        }

        configChildWidget->SetValue(value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT64:
      {
        int value = ref->GetInt64(*_msg, field);
        if (newWidget)
        {
          configChildWidget = new NumberWidget(name, _level, NumberWidget::INT);
          newFieldWidget = configChildWidget;
        }
        configChildWidget->SetValue(value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT64:
      {
        unsigned int value = ref->GetUInt64(*_msg, field);
        if (newWidget)
        {
          configChildWidget =
              new NumberWidget(name, _level, NumberWidget::UINT);
          newFieldWidget = configChildWidget;
        }
        configChildWidget->SetValue(value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT32:
      {
        int value = ref->GetInt32(*_msg, field);
        if (newWidget)
        {
          configChildWidget =
              new NumberWidget(name, _level, NumberWidget::INT);
          newFieldWidget = configChildWidget;
        }
        configChildWidget->SetValue(value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT32:
      {
        unsigned int value = ref->GetUInt32(*_msg, field);
        if (newWidget)
        {
          configChildWidget =
              new NumberWidget(name, _level, NumberWidget::UINT);
          newFieldWidget = configChildWidget;
        }
        configChildWidget->SetValue(value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_BOOL:
      {
        bool value = ref->GetBool(*_msg, field);
        if (newWidget)
        {
          configChildWidget = new BoolWidget(name, _level);
          newFieldWidget = configChildWidget;
        }

        configChildWidget->SetValue(value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_STRING:
      {
        std::string value = ref->GetString(*_msg, field);
        if (newWidget)
        {
          // Choose either a one-line or a multi-line widget according to name
          std::string type = "line";
          if (name == "innerxml")
            type = "plain";

          configChildWidget = new StringWidget(name, _level, type);
          newFieldWidget = configChildWidget;
        }

        QVariant v;
        v.setValue(value);
        configChildWidget->SetValue(v);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
      {
        auto valueMsg = ref->MutableMessage(_msg, field);

        // parse and create custom geometry widgets
        if (field->message_type()->name() == "Geometry")
        {
          if (newWidget)
          {
            configChildWidget = new GeometryWidget(name, _level);
            newFieldWidget = configChildWidget;
          }

          auto value = dynamic_cast<msgs::Geometry *>(valueMsg);
          QVariant v;
          v.setValue(*value);
          configChildWidget->SetValue(v);
        }
        // parse and create custom pose widgets
        else if (field->message_type()->name() == "Pose")
        {
          if (newWidget)
          {
            configChildWidget = new Pose3dWidget(name, _level);
            newFieldWidget = configChildWidget;
          }

          math::Pose3d value;
          auto valueDescriptor = valueMsg->GetDescriptor();
          int valueMsgFieldCount = valueDescriptor->field_count();
          for (int j = 0; j < valueMsgFieldCount; ++j)
          {
            auto valueField = valueDescriptor->field(j);

            if (valueField->type() !=
                google::protobuf::FieldDescriptor::TYPE_MESSAGE)
              continue;

            if (valueField->message_type()->name() == "Vector3d")
            {
              // pos
              auto posValueMsg = valueMsg->GetReflection()->MutableMessage(
                  valueMsg, valueField);
              auto vec3 = this->ParseVector3d(posValueMsg);
              value.Pos() = vec3;
            }
            else if (valueField->message_type()->name() == "Quaternion")
            {
              // rot
              auto quatValueMsg = valueMsg->GetReflection()->MutableMessage(
                  valueMsg, valueField);
              auto quatValueDescriptor = quatValueMsg->GetDescriptor();
              std::vector<double> quatValues;
              // FIXME: skipping header
              for (unsigned int k = 1; k < 5; ++k)
              {
                auto quatValueField = quatValueDescriptor->field(k);
                quatValues.push_back(quatValueMsg->GetReflection()->GetDouble(
                    *quatValueMsg, quatValueField));
              }
              math::Quaterniond quat(quatValues[3], quatValues[0],
                  quatValues[1], quatValues[2]);
              value.Rot() = quat;
            }
            else
            {
              // FIXME: header
            }
          }

          QVariant v;
          v.setValue(value);
          configChildWidget->SetValue(v);
        }
        // parse and create custom vector3 widgets
        else if (field->message_type()->name() == "Vector3d")
        {
          if (newWidget)
          {
            configChildWidget = new Vector3dWidget(name, _level);
            newFieldWidget = configChildWidget;
          }

          math::Vector3d vec3 = this->ParseVector3d(valueMsg);

          QVariant v;
          v.setValue(vec3);

          configChildWidget->SetValue(v);
        }
        // parse and create custom color widgets
        else if (field->message_type()->name() == "Color")
        {
          if (newWidget)
          {
            configChildWidget = new ColorWidget(name, _level);
            newFieldWidget = configChildWidget;
          }

          math::Color color;
          auto valueDescriptor = valueMsg->GetDescriptor();
          std::vector<double> values;
          // FIXME: skipping header
          for (unsigned int j = 1; j <= configChildWidget->widgets.size(); ++j)
          {
            auto valueField = valueDescriptor->field(j);
            if (valueMsg->GetReflection()->HasField(*valueMsg, valueField))
            {
              values.push_back(valueMsg->GetReflection()->GetFloat(
                  *valueMsg, valueField));
            }
            // TODO: fill with default color values instead
            else
              values.push_back(0);
          }
          color.R(values[0]);
          color.G(values[1]);
          color.B(values[2]);
          color.A(values[3]);

          QVariant v;
          v.setValue(color);
          configChildWidget->SetValue(v);
        }
        // parse and create custom density widgets
        // TODO: How do we get here?
        else if (field->message_type()->name() == "Density")
        {
          if (newWidget)
          {
            configChildWidget = new DensityWidget(name, _level);
            newFieldWidget = configChildWidget;
          }
          auto valueDescriptor = valueMsg->GetDescriptor();

          double density = 1.0;

          int valueMsgFieldCount = valueDescriptor->field_count();
          for (int j = 0; j < valueMsgFieldCount ; ++j)
          {
            auto valueField = valueDescriptor->field(j);

            if (valueField && valueField->name() == "density")
              density = valueMsg->GetReflection()->GetDouble(
                  *valueMsg, valueField);
          }
          configChildWidget->SetValue(density);
        }
        else
        {
          // parse the message fields recursively
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

        if (newWidget)
        {
          // Make it into a group widget
          PropertyWidget *childWidget =
              qobject_cast<PropertyWidget *>(newFieldWidget);
          if (childWidget)
          {
            newFieldWidget = new CollapsibleWidget(name, childWidget, _level);
          }
        }

        break;
      }
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
          configChildWidget = new EnumWidget(name, enumValues, _level);

          if (!configChildWidget)
          {
            ignerr << "Error creating an enum widget for '" << name << "'"
                << std::endl;
            break;
          }

          newFieldWidget = configChildWidget;
        }
        QVariant v;
        v.setValue(value->name());
        configChildWidget->SetValue(v);
        break;
      }
      default:
        break;
    }

    // Style widgets without parent (level 0)
    if (newFieldWidget && _level == 0 &&
        !qobject_cast<CollapsibleWidget *>(newFieldWidget))
    {
      newFieldWidget->setStyleSheet(
          "QWidget\
          {\
            background-color: " + kBgColors[0] +
          "}");
    }

    if (newWidget && newFieldWidget)
    {
      newWidgets.push_back(newFieldWidget);

      // store the newly created widget in a map with a unique scoped name.
      if (qobject_cast<CollapsibleWidget *>(newFieldWidget))
      {
        auto groupWidget = qobject_cast<CollapsibleWidget *>(newFieldWidget);
        PropertyWidget *childWidget = qobject_cast<PropertyWidget *>(
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
    // create a group box to hold child widgets.
    auto widget = new QGroupBox();
    auto widgetLayout = new QVBoxLayout;

    for (unsigned int i = 0; i < newWidgets.size(); ++i)
    {
      widgetLayout->addWidget(newWidgets[i]);
    }

    widgetLayout->setContentsMargins(0, 0, 0, 0);
    widgetLayout->setSpacing(0);
    widgetLayout->setAlignment(Qt::AlignTop);
    widget->setLayout(widgetLayout);
    return widget;
  }

  return nullptr;
}

/////////////////////////////////////////////////
math::Vector3d MessageWidget::ParseVector3d(
    const google::protobuf::Message *_msg) const
{
  math::Vector3d vec3;
  auto valueDescriptor = _msg->GetDescriptor();
  std::vector<double> values;

  // FIX: skipping header
  for (unsigned int i = 1; i < 4; ++i)
  {
    auto valueField = valueDescriptor->field(i);

    values.push_back(_msg->GetReflection()->GetDouble(*_msg, valueField));
  }
  vec3.X(values[0]);
  vec3.Y(values[1]);
  vec3.Z(values[2]);
  return vec3;
}

/////////////////////////////////////////////////
void MessageWidget::UpdateMsg(google::protobuf::Message *_msg,
    const std::string &_name)
{
  auto d = _msg->GetDescriptor();
  if (!d)
    return;
  auto count = d->field_count();

  for (int i = 0; i < count ; ++i)
  {
    auto field = d->field(i);

    if (!field)
      return;

    auto ref = _msg->GetReflection();

    if (!ref)
      return;

    std::string name = field->name();

    // Update each field in the message
    // TODO update repeated fields
    if (field->is_repeated() /*|| !ref->HasField(*_msg, field)*/)
      continue;

    std::string scopedName = _name.empty() ? name : _name + "::" + name;
    if (this->dataPtr->configWidgets.find(scopedName) ==
        this->dataPtr->configWidgets.end())
      continue;

    // don't update msgs field that are associated with read-only widgets
    if (this->WidgetReadOnly(scopedName))
      continue;

    auto childWidget = this->dataPtr->configWidgets[scopedName];

    switch (field->type())
    {
      case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
      {
        auto v = childWidget->Value();
        ref->SetDouble(_msg, field, v.toDouble());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_FLOAT:
      {
        auto v = childWidget->Value();
        ref->SetFloat(_msg, field, v.toDouble());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT64:
      {
        auto v = childWidget->Value();
        ref->SetInt64(_msg, field, v.toInt());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT64:
      {
        auto v = childWidget->Value();
        ref->SetUInt64(_msg, field, v.toUInt());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT32:
      {
        auto v = childWidget->Value();
        ref->SetInt32(_msg, field, v.toInt());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT32:
      {
        auto v = childWidget->Value();
        ref->SetUInt32(_msg, field, v.toUInt());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_BOOL:
      {
        auto v = childWidget->Value();
        ref->SetBool(_msg, field, v.toBool());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_STRING:
      {
        if (qobject_cast<QLineEdit *>(childWidget->widgets[0]))
        {
          auto valueLineEdit =
            qobject_cast<QLineEdit *>(childWidget->widgets[0]);
          ref->SetString(_msg, field, valueLineEdit->text().toStdString());
        }
        else if (qobject_cast<QPlainTextEdit *>(childWidget->widgets[0]))
        {
          auto valueTextEdit =
              qobject_cast<QPlainTextEdit *>(childWidget->widgets[0]);
          ref->SetString(_msg, field,
              valueTextEdit->toPlainText().toStdString());
        }
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
      {
        auto valueMsg = (ref->MutableMessage(_msg, field));

        // update geometry msg field
        if (field->message_type()->name() == "Geometry")
        {
          auto v = childWidget->Value();
          auto geomMsg = ref->MutableMessage(_msg, field);
          geomMsg->CopyFrom(v.value<msgs::Geometry>());
        }
        // update pose msg field
        else if (field->message_type()->name() == "Pose")
        {
          auto valueDescriptor = valueMsg->GetDescriptor();
          int valueMsgFieldCount = valueDescriptor->field_count();

          // loop through the message fields to update:
          // a vector3d field (position)
          // and quaternion field (orientation)
          // FIXME: skipping header
          for (int j = 0; j < valueMsgFieldCount; ++j)
          {
            auto valueField = valueDescriptor->field(j);

            if (valueField->type() !=
                google::protobuf::FieldDescriptor::TYPE_MESSAGE)
              continue;

            // Take values from all 6 widgets
            std::vector<double> values;
            for (auto widget : childWidget->widgets)
            {
              auto valueSpinBox = qobject_cast<QDoubleSpinBox *>(widget);
              values.push_back(valueSpinBox->value());
            }

            // Position
            if (valueField->message_type()->name() == "Vector3d")
            {
              auto posValueMsg = valueMsg->GetReflection()->MutableMessage(
                  valueMsg, valueField);
              math::Vector3d vec3(values[0], values[1], values[2]);
              this->UpdateVector3dMsg(posValueMsg, vec3);
            }
            // Orientation
            else if (valueField->message_type()->name() == "Quaternion")
            {
              auto quatValueMsg = valueMsg->GetReflection()->MutableMessage(
                  valueMsg, valueField);
              math::Quaterniond quat(values[3], values[4], values[5]);

              std::vector<double> quatValues;
              quatValues.push_back(quat.X());
              quatValues.push_back(quat.Y());
              quatValues.push_back(quat.Z());
              quatValues.push_back(quat.W());
              auto quatValueDescriptor = quatValueMsg->GetDescriptor();
              for (unsigned int k = 0; k < quatValues.size(); ++k)
              {
                // FIXME: skipping header
                auto quatValueField = quatValueDescriptor->field(k+1);
                quatValueMsg->GetReflection()->SetDouble(quatValueMsg,
                    quatValueField, quatValues[k]);
              }
            }
            else
            {
              // FIXME: skipping header
            }
          }
        }
        else if (field->message_type()->name() == "Vector3d")
        {
          std::vector<double> values;
          for (unsigned int j = 0; j < 3; ++j)
          {
            auto valueSpinBox =
                qobject_cast<QDoubleSpinBox *>(childWidget->widgets[j]);
            values.push_back(valueSpinBox->value());
          }
          math::Vector3d vec3(values[0], values[1], values[2]);
          this->UpdateVector3dMsg(valueMsg, vec3);
        }
        else if (field->message_type()->name() == "Color")
        {
          auto valueDescriptor = valueMsg->GetDescriptor();
          // FIXME: skipping header
          for (unsigned int j = 0; j < childWidget->widgets.size(); ++j)
          {
            auto valueSpinBox =
                qobject_cast<QDoubleSpinBox *>(childWidget->widgets[j]);
            auto valueField = valueDescriptor->field(j+1);
            valueMsg->GetReflection()->SetFloat(valueMsg, valueField,
                valueSpinBox->value());
          }
        }
        // TODO: density?!
        else if (field->message_type()->name() == "Density")
        {
          auto densityWidget = qobject_cast<DensityWidget *>(childWidget);
          auto valueDescriptor = valueMsg->GetDescriptor();
          auto densityField = valueDescriptor->FindFieldByName("density");
          valueMsg->GetReflection()->SetDouble(valueMsg, densityField,
              densityWidget->Value().toDouble());
        }
        else
        {
          // update the message fields recursively
          this->UpdateMsg(valueMsg, scopedName);
        }

        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_ENUM:
      {
        auto value = childWidget->Value().value<std::string>();

        // Convert string into protobuf enum
        auto enumDescriptor = field->enum_type();
        if (enumDescriptor)
        {
          auto enumValue = enumDescriptor->FindValueByName(value);
          if (enumValue)
            ref->SetEnum(_msg, field, enumValue);
          else
            ignerr << "Unable to find enum value: '" << value << "'"
                << std::endl;
        }
        break;
      }
      default:
        break;
    }
  }
}

/////////////////////////////////////////////////
void MessageWidget::UpdateVector3dMsg(google::protobuf::Message *_msg,
    const math::Vector3d &_value)
{
  auto valueDescriptor = _msg->GetDescriptor();

  std::vector<double> values;
  values.push_back(_value.X());
  values.push_back(_value.Y());
  values.push_back(_value.Z());

  for (unsigned int i = 0; i < 3; ++i)
  {
    // FIXME: skipping header
    auto valueField = valueDescriptor->field(i+1);
    if (valueField->type() != google::protobuf::FieldDescriptor::TYPE_DOUBLE)
    {
      ignerr << "Bad field [" << i+1 << "]!" << std::endl;
      continue;
    }
    _msg->GetReflection()->SetDouble(_msg, valueField, values[i]);
  }
}

/////////////////////////////////////////////////
void MessageWidget::OnItemSelection(QTreeWidgetItem *_item,
                                   const int /*_column*/)
{
  if (_item && _item->childCount() > 0)
    _item->setExpanded(!_item->isExpanded());
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
  if (this->dataPtr->configWidgets.find(_name) !=
      this->dataPtr->configWidgets.end())
  {
    ignerr << "This config widget already has a child with that name. " <<
       "Names must be unique. Not adding child." << std::endl;
    return false;
  }

  _child->scopedName = _name;
  this->dataPtr->configWidgets[_name] = _child;

  // Forward widget's ValueChanged signal
  this->connect(_child, &PropertyWidget::ValueChanged,
      [this, _name](const QVariant _value)
      {this->ValueChanged(_name, _value);});

  return true;
}

/////////////////////////////////////////////////
unsigned int MessageWidget::PropertyWidgetCount() const
{
  return this->dataPtr->configWidgets.size();
}

/////////////////////////////////////////////////
bool MessageWidget::eventFilter(QObject *_obj, QEvent *_event)
{
  QAbstractSpinBox *spinBox = qobject_cast<QAbstractSpinBox *>(_obj);
  QComboBox *comboBox = qobject_cast<QComboBox *>(_obj);
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
void MessageWidget::InsertLayout(QLayout *_layout, int _pos)
{
  QGroupBox *box = qobject_cast<QGroupBox *>(
      this->layout()->itemAt(0)->widget());
  if (!box)
    return;

  QVBoxLayout *boxLayout = qobject_cast<QVBoxLayout *>(box->layout());
  if (!boxLayout)
    return;

  boxLayout->insertLayout(_pos, _layout);
}

/////////////////////////////////////////////////
PropertyWidget *MessageWidget::PropertyWidgetByName(
    const std::string &_name) const
{
  auto iter = this->dataPtr->configWidgets.find(_name);

  if (iter != this->dataPtr->configWidgets.end())
    return iter->second;
  else
    return nullptr;
}

/////////////////////////////////////////////////
QString MessageWidget::StyleSheet(const std::string &_type, const int _level)
{
  if (_type == "normal")
  {
    return "QWidget\
        {\
          background-color: " + kBgColors[_level] + ";\
          color: #4c4c4c;\
        }\
        QLabel\
        {\
          color: #d0d0d0;\
        }";
  }
  else if (_type == "warning")
  {
    return "QWidget\
      {\
        background-color: " + kBgColors[_level] + ";\
        color: " + kRedColor + ";\
      }";
  }
  else if (_type == "active")
  {
    return "QWidget\
      {\
        background-color: " + kBgColors[_level] + ";\
        color: " + kGreenColor + ";\
      }";
  }
  ignwarn << "Requested unknown style sheet type [" << _type << "]" << std::endl;
  return "";
}

