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
#include <ignition/common/MaterialDensity.hh>

#include "ignition/gui/BoolWidget.hh"
#include "ignition/gui/CollapsibleWidget.hh"
#include "ignition/gui/ColorWidget.hh"
#include "ignition/gui/Conversions.hh"
#include "ignition/gui/DoubleWidget.hh"
#include "ignition/gui/Helpers.hh"
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
bool MessageWidget::SetIntWidgetValue(const std::string &_name, int _value)
{
  auto iter = this->dataPtr->configWidgets.find(_name);

  if (iter != this->dataPtr->configWidgets.end())
    return this->UpdateIntWidget(iter->second, _value);

  return false;
}

/////////////////////////////////////////////////
bool MessageWidget::SetUIntWidgetValue(const std::string &_name,
    unsigned int _value)
{
  auto iter = this->dataPtr->configWidgets.find(_name);

  if (iter != this->dataPtr->configWidgets.end())
    return this->UpdateUIntWidget(iter->second, _value);

  return false;
}

/////////////////////////////////////////////////
bool MessageWidget::SetGeometryWidgetValue(const std::string &_name,
    const std::string &_value, const math::Vector3d &_dimensions,
    const std::string &_uri)
{
  auto iter = this->dataPtr->configWidgets.find(_name);

  if (iter != this->dataPtr->configWidgets.end())
    return this->UpdateGeometryWidget(iter->second, _value, _dimensions, _uri);

  return false;
}

/////////////////////////////////////////////////
bool MessageWidget::SetDensityWidgetValue(const std::string &_name,
    const double _value)
{
  auto iter = this->dataPtr->configWidgets.find(_name);

  if (iter != this->dataPtr->configWidgets.end())
    return this->UpdateDensityWidget(iter->second, _value);

  return false;
}

/////////////////////////////////////////////////
bool MessageWidget::SetEnumWidgetValue(const std::string &_name,
    const std::string &_value)
{
  auto iter = this->dataPtr->configWidgets.find(_name);

  if (iter != this->dataPtr->configWidgets.end())
    return this->UpdateEnumWidget(iter->second, _value);

  return false;
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
int MessageWidget::IntWidgetValue(const std::string &_name) const
{
  int value = 0;
  std::map <std::string, PropertyWidget *>::const_iterator iter =
      this->dataPtr->configWidgets.find(_name);

  if (iter != this->dataPtr->configWidgets.end())
    value = this->IntWidgetValue(iter->second);
  return value;
}

/////////////////////////////////////////////////
unsigned int MessageWidget::UIntWidgetValue(const std::string &_name) const
{
  unsigned int value = 0;
  std::map <std::string, PropertyWidget *>::const_iterator iter =
      this->dataPtr->configWidgets.find(_name);

  if (iter != this->dataPtr->configWidgets.end())
    value = this->UIntWidgetValue(iter->second);
  return value;
}

/////////////////////////////////////////////////
double MessageWidget::DensityWidgetValue(const std::string &_name) const
{
  double value = 0.0;
  auto iter = this->dataPtr->configWidgets.find(_name);

  if (iter != this->dataPtr->configWidgets.end())
  {
    DensityWidget *widget =
        qobject_cast<DensityWidget *>(iter->second);

    if (widget)
      value = widget->Density();
  }
  return value;
}

/////////////////////////////////////////////////
std::string MessageWidget::GeometryWidgetValue(const std::string &_name,
    math::Vector3d &_dimensions, std::string &_uri) const
{
  std::string type;
  std::map <std::string, PropertyWidget *>::const_iterator iter =
      this->dataPtr->configWidgets.find(_name);

  if (iter != this->dataPtr->configWidgets.end())
    type = this->GeometryWidgetValue(iter->second, _dimensions, _uri);
  return type;
}

/////////////////////////////////////////////////
std::string MessageWidget::EnumWidgetValue(const std::string &_name) const
{
  std::string value;
  auto iter = this->dataPtr->configWidgets.find(_name);

  if (iter != this->dataPtr->configWidgets.end())
    value = this->EnumWidgetValue(iter->second);
  return value;
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
          configChildWidget = new DoubleWidget(name, _level);

          // TODO: handle this better
          if (name == "mass")
          {
            auto valueSpinBox = qobject_cast<QDoubleSpinBox *>(
                configChildWidget->widgets[0]);
            if (valueSpinBox)
            {
              this->connect(valueSpinBox, SIGNAL(valueChanged(double)),
                  this, SLOT(OnMassValueChanged(double)));
            }
          }
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
          configChildWidget = new DoubleWidget(name, _level);
          newFieldWidget = configChildWidget;
        }

        configChildWidget->SetValue(value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT64:
      {
        int64_t value = ref->GetInt64(*_msg, field);
        if (newWidget)
        {
          configChildWidget = this->CreateIntWidget(name, _level);
          newFieldWidget = configChildWidget;
        }
        this->UpdateIntWidget(configChildWidget, value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT64:
      {
        uint64_t value = ref->GetUInt64(*_msg, field);
        if (newWidget)
        {
          configChildWidget = this->CreateUIntWidget(name, _level);
          newFieldWidget = configChildWidget;
        }
        this->UpdateUIntWidget(configChildWidget, value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT32:
      {
        int32_t value = ref->GetInt32(*_msg, field);
        if (newWidget)
        {
          configChildWidget = this->CreateIntWidget(name, _level);
          newFieldWidget = configChildWidget;
        }
        this->UpdateIntWidget(configChildWidget, value);
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT32:
      {
        uint32_t value = ref->GetUInt32(*_msg, field);
        if (newWidget)
        {
          configChildWidget = this->CreateUIntWidget(name, _level);
          newFieldWidget = configChildWidget;
        }
        this->UpdateUIntWidget(configChildWidget, value);
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
            configChildWidget = this->CreateGeometryWidget(name, _level);
            newFieldWidget = configChildWidget;
          }

          // type
          auto valueDescriptor =
              valueMsg->GetDescriptor();
          auto typeField =
              valueDescriptor->FindFieldByName("type");

          if (valueMsg->GetReflection()->HasField(*valueMsg, typeField))
          {
            const google::protobuf::EnumValueDescriptor *typeValueDescriptor =
                valueMsg->GetReflection()->GetEnum(*valueMsg, typeField);

            std::string geometryTypeStr;
            if (typeValueDescriptor)
            {
              geometryTypeStr =
                  QString(typeValueDescriptor->name().c_str()).toLower().
                  toStdString();
            }

            math::Vector3d dimensions;
            // dimensions
            for (int k = 0; k < valueDescriptor->field_count() ; ++k)
            {
              auto geomField =
                  valueDescriptor->field(k);

              if (geomField->is_repeated())
                  continue;

              if (geomField->type() !=
                  google::protobuf::FieldDescriptor::TYPE_MESSAGE ||
                  !valueMsg->GetReflection()->HasField(*valueMsg, geomField))
                continue;

              auto geomValueMsg =
                  valueMsg->GetReflection()->MutableMessage(
                  valueMsg, geomField);
              auto geomValueDescriptor =
                  geomValueMsg->GetDescriptor();

              std::string geomMsgName = geomField->message_type()->name();
              if (geomMsgName == "BoxGeom" || geomMsgName == "MeshGeom")
              {
                int fieldIdx = (geomMsgName == "BoxGeom") ? 0 : 1;
                auto geomDimMsg =
                    geomValueMsg->GetReflection()->MutableMessage(
                    geomValueMsg, geomValueDescriptor->field(fieldIdx));
                dimensions = this->ParseVector3d(geomDimMsg);
                break;
              }
              else if (geomMsgName == "CylinderGeom")
              {
                auto geomRadiusField =
                    geomValueDescriptor->FindFieldByName("radius");
                double radius = geomValueMsg->GetReflection()->GetDouble(
                    *geomValueMsg, geomRadiusField);
                auto geomLengthField =
                    geomValueDescriptor->FindFieldByName("length");
                double length = geomValueMsg->GetReflection()->GetDouble(
                    *geomValueMsg, geomLengthField);
                dimensions.X(radius * 2.0);
                dimensions.Y(dimensions.X());
                dimensions.Z(length);
                break;
              }
              else if (geomMsgName == "SphereGeom")
              {
                auto geomRadiusField =
                    geomValueDescriptor->FindFieldByName("radius");
                double radius = geomValueMsg->GetReflection()->GetDouble(
                    *geomValueMsg, geomRadiusField);
                dimensions.X(radius * 2.0);
                dimensions.Y(dimensions.X());
                dimensions.Z(dimensions.X());
                break;
              }
              else if (geomMsgName == "PolylineGeom")
              {
                continue;
              }
            }
            this->UpdateGeometryWidget(configChildWidget,
                geometryTypeStr, dimensions);
          }
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
        else if (field->message_type()->name() == "Density")
        {
          if (newWidget)
          {
            configChildWidget = this->CreateDensityWidget(name, _level);
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
          this->UpdateDensityWidget(configChildWidget, density);
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
            newFieldWidget = this->CreateCollapsibleWidget(name, childWidget,
                _level);
          }
        }

        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_ENUM:
      {
        const google::protobuf::EnumValueDescriptor *value =
            ref->GetEnum(*_msg, field);

        if (!value)
        {
          ignerr << "Error retrieving enum value for '" << name << "'"
              << std::endl;
          break;
        }

        if (newWidget)
        {
          std::vector<std::string> enumValues;
          const google::protobuf::EnumDescriptor *descriptor = value->type();
          if (!descriptor)
            break;

          for (int j = 0; j < descriptor->value_count(); ++j)
          {
            const google::protobuf::EnumValueDescriptor *valueDescriptor =
                descriptor->value(j);
            if (valueDescriptor)
              enumValues.push_back(valueDescriptor->name());
          }
          configChildWidget =
              this->CreateEnumWidget(name, enumValues, _level);

          if (!configChildWidget)
          {
            ignerr << "Error creating an enum widget for '" << name << "'"
                << std::endl;
            break;
          }

          newFieldWidget = configChildWidget;
        }
        this->UpdateEnumWidget(configChildWidget, value->name());
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
        CollapsibleWidget *groupWidget =
            qobject_cast<CollapsibleWidget *>(newFieldWidget);
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
CollapsibleWidget *MessageWidget::CreateCollapsibleWidget(const std::string &_name,
    PropertyWidget *_childWidget, const int _level)
{
  // Button label
  auto buttonLabel = new QLabel(
      tr(humanReadable(_name).c_str()));
  buttonLabel->setToolTip(tr(_name.c_str()));

  // Button icon
  auto buttonIcon = new QCheckBox();
  buttonIcon->setChecked(true);
  buttonIcon->setStyleSheet(
      "QCheckBox::indicator::unchecked {\
        image: url(:/images/right_arrow.png);\
      }\
      QCheckBox::indicator::checked {\
        image: url(:/images/down_arrow.png);\
      }");

  // Button layout
  auto buttonLayout = new QHBoxLayout();
  buttonLayout->addItem(new QSpacerItem(20*_level, 1,
      QSizePolicy::Fixed, QSizePolicy::Fixed));
  buttonLayout->addWidget(buttonLabel);
  buttonLayout->addWidget(buttonIcon);
  buttonLayout->setAlignment(buttonIcon, Qt::AlignRight);

  // Button frame
  auto buttonFrame = new QFrame();
  buttonFrame->setFrameStyle(QFrame::Box);
  buttonFrame->setLayout(buttonLayout);

  // Set color for top level button
  if (_level == 0)
  {
    buttonFrame->setStyleSheet(
        "QWidget\
        {\
          background-color: " + kBgColors[0] +
        "}");
  }

  // Child widgets are contained in a group box which can be collapsed
  CollapsibleWidget *groupWidget = new CollapsibleWidget;
  groupWidget->setStyleSheet(
      "QGroupBox {\
        border : 0;\
        margin : 0;\
        padding : 0;\
      }");

  this->connect(buttonIcon, SIGNAL(toggled(bool)),
      groupWidget, SLOT(Toggle(bool)));

  // Set the child widget
  groupWidget->childWidget = _childWidget;
  _childWidget->setParent(groupWidget);
  _childWidget->setContentsMargins(0, 0, 0, 0);

  // Set color for children
  if (_level == 0)
  {
    _childWidget->setStyleSheet(
        "QWidget\
        {\
          background-color: " + kBgColors[1] +
        "}");
  }
  else if (_level == 1)
  {
    _childWidget->setStyleSheet(
        "QWidget\
        {\
          background-color: " + kBgColors[2] +
        "}");
  }
  else if (_level == 2)
  {
    _childWidget->setStyleSheet(
        "QWidget\
        {\
          background-color: " + kBgColors[3] +
        "}");
  }

  // Collapsible Layout
  auto configCollapsibleLayout = new QGridLayout;
  configCollapsibleLayout->setContentsMargins(0, 0, 0, 0);
  configCollapsibleLayout->setSpacing(0);
  configCollapsibleLayout->addWidget(buttonFrame, 0, 0);
  configCollapsibleLayout->addWidget(_childWidget, 1, 0);
  groupWidget->setLayout(configCollapsibleLayout);

  // Start collapsed
  groupWidget->Toggle(false);

  return groupWidget;
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
PropertyWidget *MessageWidget::CreateUIntWidget(const std::string &_key,
    const int _level)
{
  // ChildWidget
  PropertyWidget *widget = new PropertyWidget();

  // Label
  auto keyLabel = new QLabel(tr(humanReadable(_key).c_str()));
  keyLabel->setToolTip(tr(_key.c_str()));

  // SpinBox
  auto valueSpinBox = new QSpinBox(widget);
  valueSpinBox->setRange(0, 1e8);
  valueSpinBox->setAlignment(Qt::AlignRight);
  this->connect(valueSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnUIntValueChanged()));

  // Layout
  auto widgetLayout = new QHBoxLayout;
  if (_level != 0)
  {
    widgetLayout->addItem(new QSpacerItem(20*_level, 1,
        QSizePolicy::Fixed, QSizePolicy::Fixed));
  }
  widgetLayout->addWidget(keyLabel);
  widgetLayout->addWidget(valueSpinBox);

  // ChildWidget
  widget->setLayout(widgetLayout);
  widget->setFrameStyle(QFrame::Box);

  widget->widgets.push_back(valueSpinBox);

  return widget;
}

/////////////////////////////////////////////////
PropertyWidget *MessageWidget::CreateIntWidget(const std::string &_key,
    const int _level)
{
  // ChildWidget
  PropertyWidget *widget = new PropertyWidget();

  // Label
  auto keyLabel = new QLabel(tr(humanReadable(_key).c_str()));
  keyLabel->setToolTip(tr(_key.c_str()));

  // SpinBox
  auto valueSpinBox = new QSpinBox(widget);
  valueSpinBox->setRange(-1e8, 1e8);
  valueSpinBox->setAlignment(Qt::AlignRight);
  this->connect(valueSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnIntValueChanged()));

  // Layout
  auto widgetLayout = new QHBoxLayout;
  if (_level != 0)
  {
    widgetLayout->addItem(new QSpacerItem(20*_level, 1,
        QSizePolicy::Fixed, QSizePolicy::Fixed));
  }
  widgetLayout->addWidget(keyLabel);
  widgetLayout->addWidget(valueSpinBox);

  // ChildWidget
  widget->setLayout(widgetLayout);
  widget->setFrameStyle(QFrame::Box);

  widget->widgets.push_back(valueSpinBox);

  return widget;
}

/////////////////////////////////////////////////
PropertyWidget *MessageWidget::CreateGeometryWidget(
    const std::string &/*_key*/, const int _level)
{
  // ChildWidget
  GeometryWidget *widget = new GeometryWidget;

  // Geometry ComboBox
  auto geometryLabel = new QLabel(tr("Geometry"));
  geometryLabel->setToolTip(tr("geometry"));
  auto geometryComboBox = new QComboBox(widget);
  geometryComboBox->addItem(tr("box"));
  geometryComboBox->addItem(tr("cylinder"));
  geometryComboBox->addItem(tr("sphere"));
  geometryComboBox->addItem(tr("mesh"));
  geometryComboBox->addItem(tr("polyline"));
  this->connect(geometryComboBox, SIGNAL(currentIndexChanged(const int)), this,
      SLOT(OnGeometryValueChanged(const int)));

  // Size XYZ
  double min = 0;
  double max = 0;
  rangeFromKey("length", min, max);

  auto geomSizeXSpinBox = new QDoubleSpinBox(widget);
  geomSizeXSpinBox->setRange(min, max);
  geomSizeXSpinBox->setSingleStep(0.01);
  geomSizeXSpinBox->setDecimals(6);
  geomSizeXSpinBox->setValue(1.000);
  geomSizeXSpinBox->setAlignment(Qt::AlignRight);
  geomSizeXSpinBox->setMaximumWidth(100);
  this->connect(geomSizeXSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnGeometryValueChanged()));

  auto geomSizeYSpinBox = new QDoubleSpinBox(widget);
  geomSizeYSpinBox->setRange(min, max);
  geomSizeYSpinBox->setSingleStep(0.01);
  geomSizeYSpinBox->setDecimals(6);
  geomSizeYSpinBox->setValue(1.000);
  geomSizeYSpinBox->setAlignment(Qt::AlignRight);
  geomSizeYSpinBox->setMaximumWidth(100);
  this->connect(geomSizeYSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnGeometryValueChanged()));

  auto geomSizeZSpinBox = new QDoubleSpinBox(widget);
  geomSizeZSpinBox->setRange(min, max);
  geomSizeZSpinBox->setSingleStep(0.01);
  geomSizeZSpinBox->setDecimals(6);
  geomSizeZSpinBox->setValue(1.000);
  geomSizeZSpinBox->setAlignment(Qt::AlignRight);
  geomSizeZSpinBox->setMaximumWidth(100);
  this->connect(geomSizeZSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnGeometryValueChanged()));

  auto geomSizeXLabel = new QLabel(tr("X"));
  auto geomSizeYLabel = new QLabel(tr("Y"));
  auto geomSizeZLabel = new QLabel(tr("Z"));
  geomSizeXLabel->setStyleSheet("QLabel{color: " + kRedColor + ";}");
  geomSizeYLabel->setStyleSheet("QLabel{color: " + kGreenColor + ";}");
  geomSizeZLabel->setStyleSheet("QLabel{color: " + kBlueColor + ";}");
  geomSizeXLabel->setToolTip(tr("x"));
  geomSizeYLabel->setToolTip(tr("y"));
  geomSizeZLabel->setToolTip(tr("z"));

  std::string unit = unitFromKey("length");
  auto geomSizeXUnitLabel = new QLabel(QString::fromStdString(unit));
  auto geomSizeYUnitLabel = new QLabel(QString::fromStdString(unit));
  auto geomSizeZUnitLabel = new QLabel(QString::fromStdString(unit));

  auto geomSizeLayout = new QHBoxLayout;
  geomSizeLayout->addWidget(geomSizeXLabel);
  geomSizeLayout->addWidget(geomSizeXSpinBox);
  geomSizeLayout->addWidget(geomSizeXUnitLabel);
  geomSizeLayout->addWidget(geomSizeYLabel);
  geomSizeLayout->addWidget(geomSizeYSpinBox);
  geomSizeLayout->addWidget(geomSizeYUnitLabel);
  geomSizeLayout->addWidget(geomSizeZLabel);
  geomSizeLayout->addWidget(geomSizeZSpinBox);
  geomSizeLayout->addWidget(geomSizeZUnitLabel);

  geomSizeLayout->setAlignment(geomSizeXLabel, Qt::AlignRight);
  geomSizeLayout->setAlignment(geomSizeYLabel, Qt::AlignRight);
  geomSizeLayout->setAlignment(geomSizeZLabel, Qt::AlignRight);

  // Uri
  auto geomFilenameLabel = new QLabel(tr("Uri"));
  geomFilenameLabel->setToolTip(tr("uri"));
  auto geomFilenameLineEdit = new QLineEdit(widget);
  this->connect(geomFilenameLineEdit, SIGNAL(editingFinished()), this,
      SLOT(OnGeometryValueChanged()));
  auto geomFilenameButton = new QPushButton(tr("..."));
  geomFilenameButton->setMaximumWidth(30);

  auto geomFilenameLayout = new QHBoxLayout;
  geomFilenameLayout->addWidget(geomFilenameLabel);
  geomFilenameLayout->addWidget(geomFilenameLineEdit);
  geomFilenameLayout->addWidget(geomFilenameButton);

  auto geomSizeFilenameLayout = new QVBoxLayout;
  geomSizeFilenameLayout->addLayout(geomSizeLayout);
  geomSizeFilenameLayout->addLayout(geomFilenameLayout);

  auto geomSizeWidget = new QWidget(widget);
  geomSizeWidget->setLayout(geomSizeFilenameLayout);

  // Radius / Length
  auto geomRadiusLabel = new QLabel(tr("Radius"));
  auto geomLengthLabel = new QLabel(tr("Length"));
  auto geomRadiusUnitLabel = new QLabel(QString::fromStdString(unit));
  auto geomLengthUnitLabel = new QLabel(QString::fromStdString(unit));
  geomRadiusLabel->setToolTip(tr("radius"));
  geomLengthLabel->setToolTip(tr("length"));

  auto geomRadiusSpinBox = new QDoubleSpinBox(widget);
  geomRadiusSpinBox->setRange(min, max);
  geomRadiusSpinBox->setSingleStep(0.01);
  geomRadiusSpinBox->setDecimals(6);
  geomRadiusSpinBox->setValue(0.500);
  geomRadiusSpinBox->setAlignment(Qt::AlignRight);
  geomRadiusSpinBox->setMaximumWidth(100);
  this->connect(geomRadiusSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnGeometryValueChanged()));

  auto geomLengthSpinBox = new QDoubleSpinBox(widget);
  geomLengthSpinBox->setRange(min, max);
  geomLengthSpinBox->setSingleStep(0.01);
  geomLengthSpinBox->setDecimals(6);
  geomLengthSpinBox->setValue(1.000);
  geomLengthSpinBox->setAlignment(Qt::AlignRight);
  geomLengthSpinBox->setMaximumWidth(100);
  this->connect(geomLengthSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnGeometryValueChanged()));

  auto geomRLLayout = new QHBoxLayout;
  geomRLLayout->addWidget(geomRadiusLabel);
  geomRLLayout->addWidget(geomRadiusSpinBox);
  geomRLLayout->addWidget(geomRadiusUnitLabel);
  geomRLLayout->addWidget(geomLengthLabel);
  geomRLLayout->addWidget(geomLengthSpinBox);
  geomRLLayout->addWidget(geomLengthUnitLabel);

  geomRLLayout->setAlignment(geomRadiusLabel, Qt::AlignRight);
  geomRLLayout->setAlignment(geomLengthLabel, Qt::AlignRight);

  auto geomRLWidget = new QWidget;
  geomRLWidget->setLayout(geomRLLayout);

  // Dimensions
  auto geomDimensionWidget = new QStackedWidget(widget);
  geomDimensionWidget->insertWidget(0, geomSizeWidget);

  geomDimensionWidget->insertWidget(1, geomRLWidget);
  geomDimensionWidget->setCurrentIndex(0);
  geomDimensionWidget->setSizePolicy(
      QSizePolicy::Minimum, QSizePolicy::Minimum);

  // This is inside a group
  int level = _level + 1;

  // Layout
  auto widgetLayout = new QGridLayout;
  widgetLayout->addItem(new QSpacerItem(20*level, 1,
      QSizePolicy::Fixed, QSizePolicy::Fixed), 0, 0);
  widgetLayout->addWidget(geometryLabel, 0, 1);
  widgetLayout->addWidget(geometryComboBox, 0, 2, 1, 2);
  widgetLayout->addWidget(geomDimensionWidget, 2, 1, 1, 3);

  // ChildWidget
  widget->setFrameStyle(QFrame::Box);
  widget->geomDimensionWidget = geomDimensionWidget;
  widget->geomLengthSpinBox = geomLengthSpinBox;
  widget->geomLengthLabel = geomLengthLabel;
  widget->geomLengthUnitLabel = geomLengthUnitLabel;
  widget->geomFilenameLabel = geomFilenameLabel;
  widget->geomFilenameLineEdit = geomFilenameLineEdit;
  widget->geomFilenameButton = geomFilenameButton;

  geomFilenameLabel->setVisible(false);
  geomFilenameLineEdit->setVisible(false);
  geomFilenameButton->setVisible(false);

  this->connect(geometryComboBox, SIGNAL(currentIndexChanged(const QString)),
      widget, SLOT(OnGeometryTypeChanged(const QString)));

  this->connect(geomFilenameButton, SIGNAL(clicked()),
      widget, SLOT(OnSelectFile()));

  this->connect(widget, SIGNAL(GeometryChanged()),
      this, SLOT(OnGeometryChanged()));

  this->connect(geomSizeXSpinBox, SIGNAL(valueChanged(double)),
      widget, SLOT(OnGeometrySizeChanged(double)));

  this->connect(geomSizeYSpinBox, SIGNAL(valueChanged(double)),
      widget, SLOT(OnGeometrySizeChanged(double)));

  this->connect(geomSizeZSpinBox, SIGNAL(valueChanged(double)),
      widget, SLOT(OnGeometrySizeChanged(double)));

  this->connect(geomRadiusSpinBox, SIGNAL(valueChanged(double)),
      widget, SLOT(OnGeometrySizeChanged(double)));

  this->connect(geomLengthSpinBox, SIGNAL(valueChanged(double)),
      widget, SLOT(OnGeometrySizeChanged(double)));

  widget->setLayout(widgetLayout);
  widget->widgets.push_back(geometryComboBox);
  widget->widgets.push_back(geomSizeXSpinBox);
  widget->widgets.push_back(geomSizeYSpinBox);
  widget->widgets.push_back(geomSizeZSpinBox);
  widget->widgets.push_back(geomRadiusSpinBox);
  widget->widgets.push_back(geomLengthSpinBox);
  widget->widgets.push_back(geomFilenameLineEdit);
  widget->widgets.push_back(geomFilenameButton);

  return widget;
}

/////////////////////////////////////////////////
PropertyWidget *MessageWidget::CreateEnumWidget(
    const std::string &_key, const std::vector<std::string> &_values,
    const int _level)
{
  // Label
  auto enumLabel = new QLabel(humanReadable(_key).c_str());
  enumLabel->setToolTip(tr(_key.c_str()));

  // ComboBox
  auto enumComboBox = new QComboBox;

  for (unsigned int i = 0; i < _values.size(); ++i)
    enumComboBox->addItem(tr(_values[i].c_str()));

  // Layout
  auto widgetLayout = new QHBoxLayout;
  if (_level != 0)
  {
    widgetLayout->addItem(new QSpacerItem(20*_level, 1,
        QSizePolicy::Fixed, QSizePolicy::Fixed));
  }
  widgetLayout->addWidget(enumLabel);
  widgetLayout->addWidget(enumComboBox);

  // ChildWidget
  EnumWidget *widget = new EnumWidget();
  widget->setLayout(widgetLayout);
  widget->setFrameStyle(QFrame::Box);
  this->connect(enumComboBox, SIGNAL(currentIndexChanged(const QString &)),
      widget, SLOT(EnumChanged(const QString &)));

  widget->widgets.push_back(enumComboBox);

  // connect enum config widget event so that we can fire another
  // event from MessageWidget that has the name of this field
  this->connect(widget,
      SIGNAL(EnumValueChanged(const QString &)), this,
      SLOT(OnEnumValueChanged(const QString &)));

  return widget;
}

/////////////////////////////////////////////////
PropertyWidget *MessageWidget::CreateDensityWidget(
    const std::string &/*_key*/, const int _level)
{
  auto densityLabel = new QLabel(tr("Density"));
  densityLabel->setToolTip(tr("density"));

  auto comboBox = new QComboBox;
  size_t minLen = 0;

//  for (const auto &it : common::MaterialDensity::Materials())
//  {
//    minLen = std::max(minLen,
//        common::EnumIface<common::MaterialDensity::Type>::Str(
//        it.first).length());
//
//    comboBox->addItem(tr(
//          common::EnumIface<common::MaterialDensity::Type>::Str(
//          it.first).c_str()),
//        QVariant::fromValue(it.second));
//  }

  comboBox->addItem(tr("Custom..."));
  // Longest entry plus check box and space
  comboBox->setMinimumContentsLength(minLen+2);

  double min = 0;
  double max = 0;
  rangeFromKey("density", min, max);

  auto spinBox = new QDoubleSpinBox;
  spinBox->setRange(min, max);
  spinBox->setSingleStep(0.1);
  spinBox->setDecimals(1);
  spinBox->setValue(1.0);
  spinBox->setAlignment(Qt::AlignRight);
  spinBox->setMaximumWidth(100);

  std::string unit = unitFromKey("density");
  auto unitLabel = new QLabel(QString::fromStdString(unit));

  auto widgetLayout = new QHBoxLayout;

  widgetLayout->addSpacing((_level+1)*20);
  widgetLayout->addWidget(densityLabel);
  widgetLayout->addStretch();
  widgetLayout->addWidget(comboBox);
  widgetLayout->addWidget(spinBox);
  widgetLayout->addWidget(unitLabel);

  DensityWidget *widget = new DensityWidget;
  widget->setFrameStyle(QFrame::Box);
  widget->setLayout(widgetLayout);

  widget->comboBox = comboBox;
  widget->spinBox = spinBox;

  this->connect(comboBox, SIGNAL(currentIndexChanged(const QString &)),
      widget, SLOT(OnComboBoxChanged(const QString &)));

  this->connect(spinBox, SIGNAL(valueChanged(const QString &)),
      widget, SLOT(OnSpinBoxChanged(const QString &)));

  this->connect(widget, SIGNAL(DensityValueChanged(const double)),
      this, SLOT(OnDensityValueChanged(const double)));

  widget->widgets.push_back(comboBox);
  widget->widgets.push_back(spinBox);

  return widget;
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
        auto valueSpinBox =
            qobject_cast<QDoubleSpinBox *>(childWidget->widgets[0]);
        ref->SetDouble(_msg, field, valueSpinBox->value());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_FLOAT:
      {
        auto valueSpinBox =
            qobject_cast<QDoubleSpinBox *>(childWidget->widgets[0]);
        ref->SetFloat(_msg, field, valueSpinBox->value());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT64:
      {
        auto valueSpinBox =
            qobject_cast<QSpinBox *>(childWidget->widgets[0]);
        ref->SetInt64(_msg, field, valueSpinBox->value());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT64:
      {
        auto valueSpinBox =
            qobject_cast<QSpinBox *>(childWidget->widgets[0]);
        ref->SetUInt64(_msg, field, valueSpinBox->value());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_INT32:
      {
        auto valueSpinBox =
            qobject_cast<QSpinBox *>(childWidget->widgets[0]);
        ref->SetInt32(_msg, field, valueSpinBox->value());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_UINT32:
      {
        auto valueSpinBox =
            qobject_cast<QSpinBox *>(childWidget->widgets[0]);
        ref->SetUInt32(_msg, field, valueSpinBox->value());
        break;
      }
      case google::protobuf::FieldDescriptor::TYPE_BOOL:
      {
        auto valueRadioButton =
            qobject_cast<QRadioButton *>(childWidget->widgets[0]);
        ref->SetBool(_msg, field, valueRadioButton->isChecked());
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
          // manually retrieve values from widgets in order to update
          // the message fields.
          auto valueComboBox =
              qobject_cast<QComboBox *>(childWidget->widgets[0]);
          std::string geomType = valueComboBox->currentText().toStdString();

          auto valueDescriptor = valueMsg->GetDescriptor();
          const google::protobuf::Reflection *geomReflection =
              valueMsg->GetReflection();
          auto typeField = valueDescriptor->FindFieldByName("type");
          const google::protobuf::EnumDescriptor *typeEnumDescriptor =
              typeField->enum_type();

          if (geomType == "box" || geomType == "mesh")
          {
            double sizeX = qobject_cast<QDoubleSpinBox *>(
                childWidget->widgets[1])->value();
            double sizeY = qobject_cast<QDoubleSpinBox *>(
                childWidget->widgets[2])->value();
            double sizeZ = qobject_cast<QDoubleSpinBox *>(
                childWidget->widgets[3])->value();
            math::Vector3d geomSize(sizeX, sizeY, sizeZ);

            // set type
            std::string typeStr =
                QString(tr(geomType.c_str())).toUpper().toStdString();
            const google::protobuf::EnumValueDescriptor *geometryType =
                typeEnumDescriptor->FindValueByName(typeStr);
            geomReflection->SetEnum(valueMsg, typeField, geometryType);

            // set dimensions
            auto geomFieldDescriptor =
              valueDescriptor->FindFieldByName(geomType);
            auto geomValueMsg =
                geomReflection->MutableMessage(valueMsg, geomFieldDescriptor);

            int fieldIdx = (geomType == "box") ? 0 : 1;
            auto geomDimensionMsg =
                geomValueMsg->GetReflection()->MutableMessage(geomValueMsg,
                geomValueMsg->GetDescriptor()->field(fieldIdx));
            // FIXME: fix header
            // this->UpdateVector3dMsg(geomDimensionMsg, geomSize);

            if (geomType == "mesh")
            {
              std::string uri = qobject_cast<QLineEdit *>(
                   childWidget->widgets[6])->text().toStdString();
              auto uriFieldDescriptor =
                  geomValueMsg->GetDescriptor()->field(0);
              geomValueMsg->GetReflection()->SetString(geomValueMsg,
                  uriFieldDescriptor, uri);
            }
          }
          else if (geomType == "cylinder")
          {
            double radius = qobject_cast<QDoubleSpinBox *>(
                childWidget->widgets[4])->value();
            double length = qobject_cast<QDoubleSpinBox *>(
                childWidget->widgets[5])->value();

            // set type
            const google::protobuf::EnumValueDescriptor *geometryType =
                typeEnumDescriptor->FindValueByName("CYLINDER");
            geomReflection->SetEnum(valueMsg, typeField, geometryType);

            // set radius and length
            auto geomFieldDescriptor =
              valueDescriptor->FindFieldByName(geomType);
            auto geomValueMsg =
                geomReflection->MutableMessage(valueMsg, geomFieldDescriptor);

            // FIXME: skipping header
            auto geomRadiusField =
                geomValueMsg->GetDescriptor()->field(1);
            geomValueMsg->GetReflection()->SetDouble(geomValueMsg,
                geomRadiusField, radius);
            auto geomLengthField =
                geomValueMsg->GetDescriptor()->field(2);
            geomValueMsg->GetReflection()->SetDouble(geomValueMsg,
                geomLengthField, length);
          }
          else if (geomType == "sphere")
          {
            double radius = qobject_cast<QDoubleSpinBox *>(
                childWidget->widgets[4])->value();

            // set type
            const google::protobuf::EnumValueDescriptor *geometryType =
                typeEnumDescriptor->FindValueByName("SPHERE");
            geomReflection->SetEnum(valueMsg, typeField, geometryType);

            // set radius
            auto geomFieldDescriptor =
              valueDescriptor->FindFieldByName(geomType);
            auto geomValueMsg =
                geomReflection->MutableMessage(valueMsg, geomFieldDescriptor);

            auto geomRadiusField =
                geomValueMsg->GetDescriptor()->field(0);
            geomValueMsg->GetReflection()->SetDouble(geomValueMsg,
                geomRadiusField, radius);
          }
          else if (geomType == "polyline")
          {
            const google::protobuf::EnumValueDescriptor *geometryType =
                typeEnumDescriptor->FindValueByName("POLYLINE");
            geomReflection->SetEnum(valueMsg, typeField, geometryType);
          }
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
        else if (field->message_type()->name() == "Density")
        {
          DensityWidget *densityWidget =
              qobject_cast<DensityWidget *>(childWidget);

          auto valueDescriptor =
              valueMsg->GetDescriptor();

          auto densityField =
                          valueDescriptor->FindFieldByName("density");

          valueMsg->GetReflection()->SetDouble(valueMsg, densityField,
              densityWidget->Density());
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
        auto valueComboBox =
            qobject_cast<QComboBox *>(childWidget->widgets[0]);
        if (valueComboBox)
        {
          std::string valueStr = valueComboBox->currentText().toStdString();
          const google::protobuf::EnumDescriptor *enumDescriptor =
              field->enum_type();
          if (enumDescriptor)
          {
            const google::protobuf::EnumValueDescriptor *enumValue =
                enumDescriptor->FindValueByName(valueStr);
            if (enumValue)
              ref->SetEnum(_msg, field, enumValue);
            else
              ignerr << "Unable to find enum value: '" << valueStr << "'"
                  << std::endl;
          }
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
bool MessageWidget::UpdateIntWidget(PropertyWidget *_widget,  int _value)
{
  if (_widget->widgets.size() == 1u)
  {
    qobject_cast<QSpinBox *>(_widget->widgets[0])->setValue(_value);
    return true;
  }
  else
  {
    ignerr << "Error updating Int widget" << std::endl;
  }
  return false;
}

/////////////////////////////////////////////////
bool MessageWidget::UpdateUIntWidget(PropertyWidget *_widget,
    unsigned int _value)
{
  if (_widget->widgets.size() == 1u)
  {
    qobject_cast<QSpinBox *>(_widget->widgets[0])->setValue(_value);
    return true;
  }
  else
  {
    ignerr << "Error updating UInt widget" << std::endl;
  }
  return false;
}

/////////////////////////////////////////////////
bool MessageWidget::UpdateGeometryWidget(PropertyWidget *_widget,
    const std::string &_value, const math::Vector3d &_dimensions,
    const std::string &_uri)
{
  if (_widget->widgets.size() != 8u)
  {
    ignerr << "Error updating Geometry widget " << std::endl;
    return false;
  }

  QComboBox *valueComboBox = qobject_cast<QComboBox *>(_widget->widgets[0]);
  int index = valueComboBox->findText(tr(_value.c_str()));

  if (index < 0)
  {
    ignerr << "Error updating Geometry widget: '" << _value <<
      "' not found" << std::endl;
    return false;
  }

  qobject_cast<QComboBox *>(_widget->widgets[0])->setCurrentIndex(index);

  bool isMesh =  _value == "mesh";
  if (_value == "box" || isMesh)
  {
    qobject_cast<QDoubleSpinBox *>(_widget->widgets[1])->setValue(
        _dimensions.X());
    qobject_cast<QDoubleSpinBox *>(_widget->widgets[2])->setValue(
        _dimensions.Y());
    qobject_cast<QDoubleSpinBox *>(_widget->widgets[3])->setValue(
        _dimensions.Z());
  }
  else if (_value == "cylinder")
  {
    qobject_cast<QDoubleSpinBox *>(_widget->widgets[4])->setValue(
        _dimensions.X()*0.5);
    qobject_cast<QDoubleSpinBox *>(_widget->widgets[5])->setValue(
        _dimensions.Z());
  }
  else if (_value == "sphere")
  {
    qobject_cast<QDoubleSpinBox *>(_widget->widgets[4])->setValue(
        _dimensions.X()*0.5);
  }
  else if (_value == "polyline")
  {
    // do nothing
  }

  if (isMesh)
    qobject_cast<QLineEdit *>(_widget->widgets[6])->setText(tr(_uri.c_str()));

  emit GeometryChanged();
  return true;
}

/////////////////////////////////////////////////
bool MessageWidget::UpdateEnumWidget(PropertyWidget *_widget,
    const std::string &_value)
{
  if (_widget->widgets.size() != 1u)
  {
    ignerr << "Error updating Enum widget" << std::endl;
    return false;
  }

  QComboBox *valueComboBox = qobject_cast<QComboBox *>(_widget->widgets[0]);
  if (!valueComboBox)
  {
    ignerr << "Error updating Enum widget" << std::endl;
    return false;
  }

  int index = valueComboBox->findText(tr(_value.c_str()));

  if (index < 0)
  {
    ignerr << "Error updating Enum widget: '" << _value <<
      "' not found" << std::endl;
    return false;
  }

  qobject_cast<QComboBox *>(_widget->widgets[0])->setCurrentIndex(index);

  return true;
}

/////////////////////////////////////////////////
bool MessageWidget::UpdateDensityWidget(PropertyWidget *_widget,
          const double _value)
{
  DensityWidget *densityWidget =
      qobject_cast<DensityWidget *>(_widget);

  if (densityWidget)
  {
    densityWidget->SetDensity(_value);
    return true;
  }
  return false;
}

/////////////////////////////////////////////////
int MessageWidget::IntWidgetValue(PropertyWidget *_widget) const
{
  int value = 0;
  if (_widget->widgets.size() == 1u)
  {
    value = qobject_cast<QSpinBox *>(_widget->widgets[0])->value();
  }
  else
  {
    ignerr << "Error getting value from Int widget" << std::endl;
  }
  return value;
}

/////////////////////////////////////////////////
unsigned int MessageWidget::UIntWidgetValue(PropertyWidget *_widget) const
{
  unsigned int value = 0;
  if (_widget->widgets.size() == 1u)
  {
    value = qobject_cast<QSpinBox *>(_widget->widgets[0])->value();
  }
  else
  {
    ignerr << "Error getting value from UInt widget" << std::endl;
  }
  return value;
}

/////////////////////////////////////////////////
std::string MessageWidget::GeometryWidgetValue(PropertyWidget *_widget,
    math::Vector3d &_dimensions, std::string &_uri) const
{
  std::string value;
  if (_widget->widgets.size() != 8u)
  {
    ignerr << "Error getting value from Geometry widget " << std::endl;
    return value;
  }

  QComboBox *valueComboBox = qobject_cast<QComboBox *>(_widget->widgets[0]);
  value = valueComboBox->currentText().toStdString();

  bool isMesh = value == "mesh";
  if (value == "box" || isMesh)
  {
    _dimensions.X(qobject_cast<QDoubleSpinBox *>(_widget->widgets[1])->value());
    _dimensions.Y(qobject_cast<QDoubleSpinBox *>(_widget->widgets[2])->value());
    _dimensions.Z(qobject_cast<QDoubleSpinBox *>(_widget->widgets[3])->value());
  }
  else if (value == "cylinder")
  {
    _dimensions.X(
        qobject_cast<QDoubleSpinBox *>(_widget->widgets[4])->value()*2.0);
    _dimensions.Y(_dimensions.X());
    _dimensions.Z(qobject_cast<QDoubleSpinBox *>(_widget->widgets[5])->value());
  }
  else if (value == "sphere")
  {
    _dimensions.X(
        qobject_cast<QDoubleSpinBox *>(_widget->widgets[4])->value()*2.0);
    _dimensions.Y(_dimensions.X());
    _dimensions.Z(_dimensions.X());
  }
  else if (value == "polyline")
  {
    // do nothing
  }
  else
  {
    ignerr << "Error getting geometry dimensions for type: '" << value << "'"
        << std::endl;
  }

  if (isMesh)
    _uri = qobject_cast<QLineEdit *>(_widget->widgets[6])->text().toStdString();

  return value;
}

/////////////////////////////////////////////////
std::string MessageWidget::EnumWidgetValue(PropertyWidget *_widget) const
{
  std::string value;
  if (_widget->widgets.size() != 1u)
  {
    ignerr << "Error getting value from Enum widget " << std::endl;
    return value;
  }

  QComboBox *valueComboBox = qobject_cast<QComboBox *>(_widget->widgets[0]);
  value = valueComboBox->currentText().toStdString();

  return value;
}

/////////////////////////////////////////////////
void MessageWidget::OnItemSelection(QTreeWidgetItem *_item,
                                   const int /*_column*/)
{
  if (_item && _item->childCount() > 0)
    _item->setExpanded(!_item->isExpanded());
}

/////////////////////////////////////////////////
void MessageWidget::OnUIntValueChanged()
{
  auto spin = qobject_cast<QSpinBox *>(QObject::sender());

  if (!spin)
    return;

  auto widget = qobject_cast<PropertyWidget *>(spin->parent());

  if (!widget)
    return;

  this->ValueChanged(widget->scopedName.c_str(),
      this->UIntWidgetValue(widget));
}

/////////////////////////////////////////////////
void MessageWidget::OnIntValueChanged()
{
  auto spin =
      qobject_cast<QSpinBox *>(QObject::sender());

  if (!spin)
    return;

  PropertyWidget *widget =
      qobject_cast<PropertyWidget *>(spin->parent());

  if (!widget)
    return;

  this->ValueChanged(widget->scopedName.c_str(),
      this->IntWidgetValue(widget));
}

/////////////////////////////////////////////////
void MessageWidget::OnGeometryValueChanged()
{
  QWidget *senderWidget = qobject_cast<QWidget *>(QObject::sender());

  if (!senderWidget)
    return;

  PropertyWidget *widget;
  while (senderWidget->parent() != nullptr)
  {
    senderWidget = qobject_cast<QWidget *>(senderWidget->parent());
    widget = qobject_cast<PropertyWidget *>(senderWidget);
    if (widget)
      break;
  }

  if (!widget)
    return;

  math::Vector3d dimensions;
  std::string uri;
  std::string value = this->GeometryWidgetValue(widget, dimensions, uri);

  emit GeometryValueChanged(widget->scopedName.c_str(), value, dimensions,
      uri);
}

/////////////////////////////////////////////////
void MessageWidget::OnGeometryValueChanged(const int /*_value*/)
{
  auto combo =
      qobject_cast<QComboBox *>(QObject::sender());

  if (!combo)
    return;

  GeometryWidget *widget =
      qobject_cast<GeometryWidget *>(combo->parent());

  if (!widget)
    return;

  math::Vector3d dimensions;
  std::string uri;
  std::string value = this->GeometryWidgetValue(widget, dimensions, uri);

  emit GeometryValueChanged(widget->scopedName.c_str(), value, dimensions, uri);
}

/////////////////////////////////////////////////
void MessageWidget::OnEnumValueChanged(const QString &_value)
{
  PropertyWidget *widget =
      qobject_cast<PropertyWidget *>(QObject::sender());

  if (!widget)
    return;

  emit EnumValueChanged(widget->scopedName.c_str(), _value);
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
void MessageWidget::OnDensityValueChanged(const double _value)
{
  emit DensityValueChanged(_value);
}

/////////////////////////////////////////////////
void MessageWidget::OnMassValueChanged(const double _value)
{
  emit MassValueChanged(_value);
}

/////////////////////////////////////////////////
void MessageWidget::OnGeometryChanged()
{
  emit GeometryChanged();
}

/////////////////////////////////////////////////
void GeometryWidget::OnGeometryTypeChanged(const QString &_text)
{
  QWidget *widget= qobject_cast<QWidget *>(QObject::sender());

  if (widget)
  {
    std::string textStr = _text.toStdString();
    bool isMesh = (textStr == "mesh");
    if (textStr == "box" || isMesh)
    {
      this->geomDimensionWidget->show();
      this->geomDimensionWidget->setCurrentIndex(0);
    }
    else if (textStr == "cylinder")
    {
      this->geomDimensionWidget->show();
      this->geomDimensionWidget->setCurrentIndex(1);
      this->geomLengthSpinBox->show();
      this->geomLengthLabel->show();
      this->geomLengthUnitLabel->show();
    }
    else if (textStr == "sphere")
    {
      this->geomDimensionWidget->show();
      this->geomDimensionWidget->setCurrentIndex(1);
      this->geomLengthSpinBox->hide();
      this->geomLengthLabel->hide();
      this->geomLengthUnitLabel->hide();
    }
    else if (textStr == "polyline")
    {
      this->geomDimensionWidget->hide();
    }

    this->geomFilenameLabel->setVisible(isMesh);
    this->geomFilenameLineEdit->setVisible(isMesh);
    this->geomFilenameButton->setVisible(isMesh);
  }
  emit GeometryChanged();
}

/////////////////////////////////////////////////
void GeometryWidget::OnGeometrySizeChanged(const double /*_value*/)
{
  emit GeometryChanged();
}

/////////////////////////////////////////////////
void GeometryWidget::OnSelectFile()
{
  QWidget *widget= qobject_cast<QWidget *>(QObject::sender());

  if (widget)
  {
    QFileDialog fd(this, tr("Select mesh file"), QDir::homePath(),
      tr("Mesh files (*.dae *.stl)"));
    fd.setFilter(QDir::AllDirs | QDir::Hidden);
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
        Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);
    if (fd.exec())
    {
      if (!fd.selectedFiles().isEmpty())
      {
        QString file = fd.selectedFiles().at(0);
        if (!file.isEmpty())
        {
          dynamic_cast<QLineEdit *>(this->geomFilenameLineEdit)->setText(file);
        }
      }
    }
  }
}

/////////////////////////////////////////////////
void DensityWidget::OnComboBoxChanged(const QString &/*_text*/)
{
  QVariant variant = this->comboBox->itemData(this->comboBox->currentIndex());
  this->SetDensity(variant.toDouble());
}

/////////////////////////////////////////////////
void DensityWidget::OnSpinBoxChanged(const QString &/*_text*/)
{
  this->SetDensity(this->spinBox->value());
}

/////////////////////////////////////////////////
void DensityWidget::SetDensity(const double _density)
{
  bool comboSigState = this->comboBox->blockSignals(true);
  bool spinSigState = this->spinBox->blockSignals(true);
  {
    common::MaterialDensity::Type type;
    double matDensity;

    // Get the material closest to _density
    std::tie(type, matDensity) = common::MaterialDensity::Nearest(
        _density, 1.0);

    if (matDensity >= 0)
    {
//    this->comboBox->setCurrentIndex(
//        this->comboBox->findText(tr(
//            common::EnumIface<common::MaterialDensity::Type>::Str(
//            type).c_str())));
    }
    else
    {
      this->comboBox->setCurrentIndex(
          this->comboBox->count()-1);
    }

    this->spinBox->setValue(_density);
    this->density = _density;
  }
  this->comboBox->blockSignals(comboSigState);
  this->spinBox->blockSignals(spinSigState);

  emit DensityValueChanged(this->density);
}

/////////////////////////////////////////////////
double DensityWidget::Density() const
{
  return this->density;
}

/////////////////////////////////////////////////
void EnumWidget::EnumChanged(const QString &_value)
{
  emit EnumValueChanged(_value);
}

/////////////////////////////////////////////////
bool MessageWidget::ClearEnumWidget(const std::string &_name)
{
  // Find widget
  auto iter = this->dataPtr->configWidgets.find(_name);

  if (iter == this->dataPtr->configWidgets.end())
    return false;

  EnumWidget *enumWidget = dynamic_cast<EnumWidget *>(iter->second);

  if (enumWidget->widgets.size() != 1u)
  {
    ignerr << "Enum config widget has wrong number of widgets." << std::endl;
    return false;
  }

  QComboBox *valueComboBox = qobject_cast<QComboBox *>(enumWidget->widgets[0]);
  if (!valueComboBox)
  {
    ignerr << "Enum config widget doesn't have a QComboBox." << std::endl;
    return false;
  }

  // Clear
  valueComboBox->blockSignals(true);
  valueComboBox->clear();
  valueComboBox->blockSignals(false);
  return true;
}

/////////////////////////////////////////////////
bool MessageWidget::AddItemEnumWidget(const std::string &_name,
    const std::string &_itemText)
{
  // Find widget
  auto iter = this->dataPtr->configWidgets.find(_name);

  if (iter == this->dataPtr->configWidgets.end())
    return false;

  EnumWidget *enumWidget = dynamic_cast<EnumWidget *>(iter->second);

  if (enumWidget->widgets.size() != 1u)
  {
    ignerr << "Enum config widget has wrong number of widgets." << std::endl;
    return false;
  }

  QComboBox *valueComboBox = qobject_cast<QComboBox *>(enumWidget->widgets[0]);
  if (!valueComboBox)
  {
    ignerr << "Enum config widget doesn't have a QComboBox." << std::endl;
    return false;
  }

  // Add item
  valueComboBox->blockSignals(true);
  valueComboBox->addItem(QString::fromStdString(_itemText));
  valueComboBox->blockSignals(false);

  return true;
}

/////////////////////////////////////////////////
bool MessageWidget::RemoveItemEnumWidget(const std::string &_name,
    const std::string &_itemText)
{
  // Find widget
  auto iter = this->dataPtr->configWidgets.find(_name);

  if (iter == this->dataPtr->configWidgets.end())
    return false;

  EnumWidget *enumWidget = dynamic_cast<EnumWidget *>(iter->second);

  if (enumWidget->widgets.size() != 1u)
  {
    ignerr << "Enum config widget has wrong number of widgets." << std::endl;
    return false;
  }

  QComboBox *valueComboBox = qobject_cast<QComboBox *>(enumWidget->widgets[0]);
  if (!valueComboBox)
  {
    ignerr << "Enum config widget doesn't have a QComboBox." << std::endl;
    return false;
  }

  // Remove item if exists, otherwise return false
  int index = valueComboBox->findText(QString::fromStdString(
      _itemText));
  if (index < 0)
    return false;

  valueComboBox->blockSignals(true);
  valueComboBox->removeItem(index);
  valueComboBox->blockSignals(false);

  return true;
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

/////////////////////////////////////////////////
DensityWidget::DensityWidget()
{
}
