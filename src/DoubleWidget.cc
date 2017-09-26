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

#include "ignition/gui/Helpers.hh"
#include "ignition/gui/DoubleWidget.hh"

namespace ignition
{
  namespace gui
  {
    class PropertyWidget;

    /// \brief Private data for the DoubleWidget class.
    class DoubleWidgetPrivate
    {
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
DoubleWidget::DoubleWidget(const std::string &_key, const unsigned int _level)
    : dataPtr(new DoubleWidgetPrivate())
{
  this->level = _level;

  // Label
  auto keyLabel = new QLabel(tr(humanReadable(_key).c_str()));
  keyLabel->setToolTip(tr(_key.c_str()));

  // SpinBox
  double min = 0;
  double max = 0;
  rangeFromKey(_key, min, max);

  auto valueSpinBox = new QDoubleSpinBox(this);
  valueSpinBox->setRange(min, max);
  valueSpinBox->setSingleStep(0.01);
  valueSpinBox->setDecimals(8);
  valueSpinBox->setAlignment(Qt::AlignRight);
  this->connect(valueSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnValueChanged()));

  // Unit
  // std::string jointType = this->EnumWidgetValue("type");
  std::string unit = unitFromKey(_key);

  auto unitLabel = new QLabel();
  unitLabel->setMaximumWidth(40);
  unitLabel->setText(QString::fromStdString(unit));

  // Layout
  auto widgetLayout = new QHBoxLayout;
  widgetLayout->addWidget(keyLabel);
  widgetLayout->addWidget(valueSpinBox);
  if (unitLabel->text() != "")
    widgetLayout->addWidget(unitLabel);

  // ChildWidget
  this->key = _key;
  this->setLayout(widgetLayout);
  this->setFrameStyle(QFrame::Box);

  this->widgets.push_back(valueSpinBox);
  this->mapWidgetToUnit[valueSpinBox] = unitLabel;
}

/////////////////////////////////////////////////
DoubleWidget::~DoubleWidget()
{
}

/////////////////////////////////////////////////
bool DoubleWidget::SetValue(const QVariant _value)
{
  double value = _value.toDouble();

  if (this->widgets.size() == 1u)
  {
    // Spin value
    auto spin = qobject_cast<QDoubleSpinBox *>(this->widgets[0]);
    spin->setValue(value);

    // Unit label
//    std::string jointType = this->EnumWidgetValue("type");
//    std::string unit = unitFromKey(this->key, jointType);
//    qobject_cast<QLabel *>(
//        this->mapWidgetToUnit[spin])->setText(QString::fromStdString(unit));

    return true;
  }

  ignerr << "Error updating double widget, wrong number of child widgets: ["
         << this->widgets.size() << std::endl;
  return false;
}

/////////////////////////////////////////////////
QVariant DoubleWidget::Value() const
{
  double value = 0.0;
  if (this->widgets.size() == 1u)
  {
    value = qobject_cast<QDoubleSpinBox *>(this->widgets[0])->value();
  }
  else
  {
    ignerr << "Error getting value from double widget, wrong number of child "
           << "widgets: [" << this->widgets.size() << std::endl;
  }
  return value;
}
