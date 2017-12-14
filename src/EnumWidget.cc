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
#include "ignition/gui/QtMetatypes.hh"

#include "ignition/gui/EnumWidget.hh"

namespace ignition
{
  namespace gui
  {
    class PropertyWidget;

    /// \brief Private data for the EnumWidget class.
    class EnumWidgetPrivate
    {
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
EnumWidget::EnumWidget(const std::string &_key,
                       const std::vector<std::string> &_values)
    : dataPtr(new EnumWidgetPrivate())
{
  // Label
  auto label = new QLabel(humanReadable(_key).c_str());
  label->setToolTip(tr(_key.c_str()));

  // ComboBox
  auto comboBox = new QComboBox();

  for (unsigned int i = 0; i < _values.size(); ++i)
    comboBox->addItem(tr(_values[i].c_str()));

  // Layout
  auto widgetLayout = new QHBoxLayout;
  widgetLayout->addWidget(label);
  widgetLayout->addWidget(comboBox);

  this->setLayout(widgetLayout);

  this->connect(comboBox, SIGNAL(currentIndexChanged(const QString &)),
      this, SLOT(OnValueChanged()));
}

/////////////////////////////////////////////////
EnumWidget::~EnumWidget()
{
}

/////////////////////////////////////////////////
bool EnumWidget::SetValue(const QVariant _value)
{
  if (!_value.canConvert<std::string>())
  {
    ignerr << "Wrong variant type, expected [std::string]" << std::endl;
    return false;
  }

  auto value = _value.value<std::string>();

  auto combo = this->findChild<QComboBox *>();

  int index = combo->findText(QString::fromStdString(value));

  if (index < 0)
  {
    ignerr << "Error updating Enum widget: '" << value <<
      "' not found" << std::endl;
    return false;
  }

  combo->setCurrentIndex(index);

  return true;
}

/////////////////////////////////////////////////
QVariant EnumWidget::Value() const
{
  auto combo = this->findChild<QComboBox *>();
  auto value = combo->currentText().toStdString();

  return QVariant::fromValue(value);
}

/////////////////////////////////////////////////
bool EnumWidget::Clear()
{
  auto combo = this->findChild<QComboBox *>();

  combo->blockSignals(true);
  combo->clear();
  combo->blockSignals(false);

  return true;
}

/////////////////////////////////////////////////
bool EnumWidget::AddItem(const std::string &_itemText)
{
  auto combo = this->findChild<QComboBox *>();

  combo->blockSignals(true);
  combo->addItem(QString::fromStdString(_itemText));
  combo->blockSignals(false);

  return true;
}

/////////////////////////////////////////////////
bool EnumWidget::RemoveItem(const std::string &_itemText)
{
  auto combo = this->findChild<QComboBox *>();

  // Remove item if exists, otherwise return false
  int index = combo->findText(QString::fromStdString(_itemText));
  if (index < 0)
    return false;

  combo->blockSignals(true);
  combo->removeItem(index);
  combo->blockSignals(false);

  return true;
}

/////////////////////////////////////////////////
QComboBox *EnumWidget::Combo() const
{
  return this->findChild<QComboBox *>();
}

