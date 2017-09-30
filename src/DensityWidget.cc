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
#include <ignition/common/MaterialDensity.hh>

#include "ignition/gui/Helpers.hh"
#include "ignition/gui/DensityWidget.hh"

namespace ignition
{
  namespace gui
  {
    class PropertyWidget;

    /// \brief Private data for the DensityWidget class.
    class DensityWidgetPrivate
    {
      /// \brief A combo box for density according to material.
      public: QComboBox *comboBox;

      /// \brief A spin box for density value.
      public: QDoubleSpinBox *spinBox;
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
DensityWidget::DensityWidget(const std::string &/*_key*/)
    : dataPtr(new DensityWidgetPrivate())
{
  // Label
  auto densityLabel = new QLabel(tr("Density"));
  densityLabel->setToolTip(tr("density"));

  // Combo
  auto comboBox = new QComboBox;
  size_t minLen = 0;

// FIXME
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

  auto unit = unitFromKey("density");
  auto unitLabel = new QLabel(QString::fromStdString(unit));

  auto widgetLayout = new QHBoxLayout;
  widgetLayout->addWidget(densityLabel);
  widgetLayout->addStretch();
  widgetLayout->addWidget(comboBox);
  widgetLayout->addWidget(spinBox);
  widgetLayout->addWidget(unitLabel);

  this->setLayout(widgetLayout);

  this->dataPtr->comboBox = comboBox;
  this->dataPtr->spinBox = spinBox;

  this->connect(comboBox, SIGNAL(currentIndexChanged(const QString &)),
      this, SLOT(OnComboBoxChanged()));

  this->connect(spinBox, SIGNAL(valueChanged(const QString &)),
      this, SLOT(OnSpinBoxChanged()));

  this->connect(this, SIGNAL(DensityValueChanged(const double)),
      this, SLOT(OnValueChanged()));
}

/////////////////////////////////////////////////
DensityWidget::~DensityWidget()
{
}

/////////////////////////////////////////////////
bool DensityWidget::SetValue(const QVariant _value)
{
  bool comboSigState = this->dataPtr->comboBox->blockSignals(true);
  bool spinSigState = this->dataPtr->spinBox->blockSignals(true);

  auto density = _value.toDouble();

  common::MaterialDensity::Type type;
  double matDensity;

  // Get the material closest to _density
  std::tie(type, matDensity) = common::MaterialDensity::Nearest(density, 1.0);

  if (matDensity >= 0)
  {
// FIXME
//    this->dataPtr->comboBox->setCurrentIndex(
//        this->dataPtr->comboBox->findText(tr(
//            common::EnumIface<common::MaterialDensity::Type>::Str(
//            type).c_str())));
  }
  else
  {
    this->dataPtr->comboBox->setCurrentIndex(
        this->dataPtr->comboBox->count()-1);
  }

  this->dataPtr->spinBox->setValue(density);

  this->dataPtr->comboBox->blockSignals(comboSigState);
  this->dataPtr->spinBox->blockSignals(spinSigState);

  return true;
}

/////////////////////////////////////////////////
QVariant DensityWidget::Value() const
{
  return this->dataPtr->spinBox->value();
}

/////////////////////////////////////////////////
void DensityWidget::OnComboBoxChanged()
{
  auto variant = this->dataPtr->comboBox->itemData(
      this->dataPtr->comboBox->currentIndex());
  this->SetValue(variant.toDouble());
  this->OnValueChanged();
}

/////////////////////////////////////////////////
void DensityWidget::OnSpinBoxChanged()
{
  this->SetValue(this->dataPtr->spinBox->value());
  this->OnValueChanged();
}
