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

#include "ignition/gui/StringWidget.hh"

namespace ignition
{
  namespace gui
  {
    class PropertyWidget;

    /// \brief Private data for the StringWidget class.
    class StringWidgetPrivate
    {
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
StringWidget::StringWidget(const std::string &_key, const std::string &_type)
    : dataPtr(new StringWidgetPrivate())
{
  // Label
  auto keyLabel = new QLabel(tr(humanReadable(_key).c_str()));
  keyLabel->setToolTip(tr(_key.c_str()));

  // Line or Text Edit based on key
  QWidget *valueEdit;
  if (_type == "plain")
  {
    valueEdit = new QPlainTextEdit(this);
    valueEdit->setMinimumHeight(50);
    // QPlainTextEdit's don't have editingFinished signals
  }
  else
  {
    valueEdit = new QLineEdit(this);
    this->connect(valueEdit, SIGNAL(editingFinished()), this,
        SLOT(OnValueChanged()));
  }

  // Layout
  auto widgetLayout = new QHBoxLayout;
  widgetLayout->addWidget(keyLabel);
  widgetLayout->addWidget(valueEdit);

  this->setLayout(widgetLayout);
}

/////////////////////////////////////////////////
StringWidget::~StringWidget()
{
}

/////////////////////////////////////////////////
bool StringWidget::SetValue(const QVariant _value)
{
  auto value = _value.value<std::string>();

  if (auto edit = this->findChild<QLineEdit *>())
  {
    edit->setText(tr(value.c_str()));
    return true;
  }

  if (auto plain = this->findChild<QPlainTextEdit *>())
  {
    plain->setPlainText(tr(value.c_str()));
    return true;
  }

  ignerr << "Error updating widget" << std::endl;
  return false;
}

/////////////////////////////////////////////////
QVariant StringWidget::Value() const
{
  std::string value;

  if (auto edit = this->findChild<QLineEdit *>())
  {
    value = edit->text().toStdString();
  }
  else if (auto plain = this->findChild<QPlainTextEdit *>())
  {
    value = plain->toPlainText().toStdString();
  }

  QVariant v;
  v.setValue(value);

  return v;
}
