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
#include "ignition/gui/BoolWidget.hh"

namespace ignition
{
  namespace gui
  {
    /// \brief Private data for the BoolWidget class.
    class BoolWidgetPrivate
    {
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
BoolWidget::BoolWidget(const std::string &_key, const unsigned int _level)
    : dataPtr(new BoolWidgetPrivate())
{
  this->level = _level;

  // Label
  auto keyLabel = new QLabel(tr(humanReadable(_key).c_str()));
  keyLabel->setToolTip(tr(_key.c_str()));

  // Buttons
  auto valueTrueRadioButton = new QRadioButton(this);
  valueTrueRadioButton->setText(tr("True"));
  this->connect(valueTrueRadioButton, SIGNAL(toggled(bool)), this,
      SLOT(OnValueChanged()));

  auto valueFalseRadioButton = new QRadioButton(this);
  valueFalseRadioButton->setText(tr("False"));
  this->connect(valueFalseRadioButton, SIGNAL(toggled(bool)), this,
      SLOT(OnValueChanged()));

  auto boolButtonGroup = new QButtonGroup;
  boolButtonGroup->addButton(valueTrueRadioButton);
  boolButtonGroup->addButton(valueFalseRadioButton);
  boolButtonGroup->setExclusive(true);

  auto buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(valueTrueRadioButton);
  buttonLayout->addWidget(valueFalseRadioButton);

  // Layout
  auto widgetLayout = new QHBoxLayout;
  widgetLayout->addWidget(keyLabel);
  widgetLayout->addLayout(buttonLayout);

  // ChildWidget
  this->setLayout(widgetLayout);
  this->setFrameStyle(QFrame::Box);
}

/////////////////////////////////////////////////
BoolWidget::~BoolWidget()
{
}

/////////////////////////////////////////////////
bool BoolWidget::SetValue(const QVariant _value)
{
  bool value = _value.toBool();

  auto radios = this->findChildren<QRadioButton *>();
  radios[0]->setChecked(value);
  radios[1]->setChecked(!value);

  return true;
}

/////////////////////////////////////////////////
QVariant BoolWidget::Value() const
{
  auto radios = this->findChildren<QRadioButton *>();
  return radios[0]->isChecked();
}
