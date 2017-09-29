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
#include <ignition/math/Helpers.hh>

#include "ignition/gui/Helpers.hh"
#include "ignition/gui/NumberWidget.hh"

namespace ignition
{
  namespace gui
  {
    /// \brief Private data for the NumberWidget class.
    class NumberWidgetPrivate
    {
      public: NumberWidget::NumberType type;
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
NumberWidget::NumberWidget(const std::string &_key, const NumberType _type)
    : dataPtr(new NumberWidgetPrivate())
{
  this->dataPtr->type = _type;

  auto widgetLayout = new QHBoxLayout;

  // Label
  auto keyLabel = new QLabel(tr(humanReadable(_key).c_str()));
  keyLabel->setToolTip(tr(_key.c_str()));
  widgetLayout->addWidget(keyLabel);

  // Range
  double min = 0;
  double max = 0;
  rangeFromKey(_key, min, max);

  // Make sure it fits QSpinBox
  if (this->dataPtr->type == UINT)
  {
    min = std::max((unsigned int)min, 0u);
    max = math::equal(max, math::MAX_D) ? math::MAX_I32 : (unsigned int)max;
  }
  else if (this->dataPtr->type == INT)
  {
    min = math::equal(min, math::MIN_D) ? math::MIN_I32 : (int)max;
    max = math::equal(max, math::MAX_D) ? math::MAX_I32 : (int)max;
  }

  // Unit
  // FIXME: handle this
  // std::string jointType = this->EnumWidgetValue("type");
  std::string unit = unitFromKey(_key);

  auto unitLabel = new QLabel();
  unitLabel->setMaximumWidth(40);
  unitLabel->setText(QString::fromStdString(unit));

  // Spin
  if (this->dataPtr->type == DOUBLE)
  {
    auto spin = new QDoubleSpinBox(this);
    spin->setSingleStep(0.01);
    spin->setDecimals(8);
    spin->setRange(min, max);
    spin->setAlignment(Qt::AlignRight);
    this->connect(spin, SIGNAL(editingFinished()), this,
      SLOT(OnValueChanged()));
    widgetLayout->addWidget(spin);

    this->mapWidgetToUnit[spin] = unitLabel;
  }
  else
  {
    auto spin = new QSpinBox(this);
    spin->setRange((int)min, (int)max);
    spin->setAlignment(Qt::AlignRight);
    this->connect(spin, SIGNAL(editingFinished()), this,
      SLOT(OnValueChanged()));
    widgetLayout->addWidget(spin);

    this->mapWidgetToUnit[spin] = unitLabel;
  }

  if (unitLabel->text() != "")
    widgetLayout->addWidget(unitLabel);

  // ChildWidget
  this->key = _key;
  this->setLayout(widgetLayout);
  this->setFrameStyle(QFrame::Box);
}

/////////////////////////////////////////////////
NumberWidget::~NumberWidget()
{
}

/////////////////////////////////////////////////
bool NumberWidget::SetValue(const QVariant _value)
{
  if (this->dataPtr->type == DOUBLE)
  {
    double value = _value.toDouble();

    auto spin = this->findChild<QDoubleSpinBox *>();
    if (!spin)
    {
      ignerr << "This should never happen." << std::endl;
      return false;
    }
    spin->setValue(value);
  }
  else
  {
    auto spin = this->findChild<QSpinBox *>();
    if (!spin)
    {
      ignerr << "This should never happen." << std::endl;
      return false;
    }

    if (this->dataPtr->type == INT)
      spin->setValue(_value.toInt());
    else
      spin->setValue(_value.toUInt());
  }

    // Unit label
//    std::string jointType = this->EnumWidgetValue("type");
//    std::string unit = unitFromKey(this->key, jointType);
//    qobject_cast<QLabel *>(
//        this->mapWidgetToUnit[spin])->setText(QString::fromStdString(unit));

  return true;
}

/////////////////////////////////////////////////
QVariant NumberWidget::Value() const
{
  {
    auto spin = this->findChild<QDoubleSpinBox *>();
    if (spin)
      return spin->value();
  }

  {
    auto spin = this->findChild<QSpinBox *>();
    if (spin)
      return QVariant(spin->value());
  }

  ignerr << "This should never happen." << std::endl;
  return QVariant();
}
