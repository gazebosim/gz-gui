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
#include <ignition/math/Color.hh>

#include "ignition/gui/Conversions.hh"
#include "ignition/gui/Helpers.hh"
#include "ignition/gui/QtMetatypes.hh"
#include "ignition/gui/ColorWidget.hh"

namespace ignition
{
  namespace gui
  {
    class PropertyWidget;

    /// \brief Private data for the ColorWidget class.
    class ColorWidgetPrivate
    {
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
ColorWidget::ColorWidget() : dataPtr(new ColorWidgetPrivate())
{
  // Labels
  auto colorRLabel = new QLabel(tr("R"));
  auto colorGLabel = new QLabel(tr("G"));
  auto colorBLabel = new QLabel(tr("B"));
  auto colorALabel = new QLabel(tr("A"));
  colorRLabel->setToolTip(tr("r"));
  colorGLabel->setToolTip(tr("g"));
  colorBLabel->setToolTip(tr("b"));
  colorALabel->setToolTip(tr("a"));

  // SpinBoxes
  double min = 0.0;
  double max = 1.0;

  auto colorRSpinBox = new QDoubleSpinBox(this);
  colorRSpinBox->setRange(min, max);
  colorRSpinBox->setSingleStep(0.1);
  colorRSpinBox->setDecimals(3);
  colorRSpinBox->setAlignment(Qt::AlignRight);
  colorRSpinBox->setMaximumWidth(100);
  this->connect(colorRSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnValueChanged()));

  auto colorGSpinBox = new QDoubleSpinBox(this);
  colorGSpinBox->setRange(min, max);
  colorGSpinBox->setSingleStep(0.1);
  colorGSpinBox->setDecimals(3);
  colorGSpinBox->setAlignment(Qt::AlignRight);
  colorGSpinBox->setMaximumWidth(100);
  this->connect(colorGSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnValueChanged()));

  auto colorBSpinBox = new QDoubleSpinBox(this);
  colorBSpinBox->setRange(min, max);
  colorBSpinBox->setSingleStep(0.1);
  colorBSpinBox->setDecimals(3);
  colorBSpinBox->setAlignment(Qt::AlignRight);
  colorBSpinBox->setMaximumWidth(100);
  this->connect(colorBSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnValueChanged()));

  auto colorASpinBox = new QDoubleSpinBox(this);
  colorASpinBox->setRange(min, max);
  colorASpinBox->setValue(max);
  colorASpinBox->setSingleStep(0.1);
  colorASpinBox->setDecimals(3);
  colorASpinBox->setAlignment(Qt::AlignRight);
  colorASpinBox->setMaximumWidth(100);
  this->connect(colorASpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnValueChanged()));

  auto customColorButton = new QPushButton(tr("..."), this);
  customColorButton->setMaximumWidth(30);
  this->connect(customColorButton, SIGNAL(clicked()), this,
      SLOT(OnCustomColorDialog()));

  // Layout
  auto widgetLayout = new QHBoxLayout;
  widgetLayout->addWidget(colorRLabel);
  widgetLayout->addWidget(colorRSpinBox);
  widgetLayout->addWidget(colorGLabel);
  widgetLayout->addWidget(colorGSpinBox);
  widgetLayout->addWidget(colorBLabel);
  widgetLayout->addWidget(colorBSpinBox);
  widgetLayout->addWidget(colorALabel);
  widgetLayout->addWidget(colorASpinBox);
  widgetLayout->addWidget(customColorButton);

  widgetLayout->setAlignment(colorRLabel, Qt::AlignRight);
  widgetLayout->setAlignment(colorGLabel, Qt::AlignRight);
  widgetLayout->setAlignment(colorBLabel, Qt::AlignRight);
  widgetLayout->setAlignment(colorALabel, Qt::AlignRight);

  this->setLayout(widgetLayout);
}

/////////////////////////////////////////////////
ColorWidget::~ColorWidget()
{
}

/////////////////////////////////////////////////
bool ColorWidget::SetValue(const QVariant _value)
{
  auto value = _value.value<math::Color>();

  auto spins = this->findChildren<QDoubleSpinBox *>();

  spins[0]->setValue(value.R());
  spins[1]->setValue(value.G());
  spins[2]->setValue(value.B());
  spins[3]->setValue(value.A());
  return true;
}

/////////////////////////////////////////////////
QVariant ColorWidget::Value() const
{
  math::Color value;

  auto spins = this->findChildren<QDoubleSpinBox *>();

  value.R(spins[0]->value());
  value.G(spins[1]->value());
  value.B(spins[2]->value());
  value.A(spins[3]->value());

  QVariant v;
  v.setValue(value);

  return v;
}

/////////////////////////////////////////////////
void ColorWidget::OnCustomColorDialog()
{
  // Current color
  auto color = convert(this->Value().value<math::Color>());

  auto dialog = this->findChild<QColorDialog *>();
  if (!dialog)
  {
    // Opening for the first time
    dialog = new QColorDialog(color, this);
    dialog->setOption(QColorDialog::ShowAlphaChannel);
    dialog->setOption(QColorDialog::NoButtons);
    this->connect(dialog, &QColorDialog::currentColorChanged,
      [this](const QColor _value)
      {
        QVariant v;
        v.setValue(convert(_value));

        this->SetValue(v);

        this->ValueChanged(v);
      });
  }
  else
  {
    dialog->blockSignals(true);
    dialog->setCurrentColor(color);
    dialog->blockSignals(false);
  }

  dialog->open();
}

