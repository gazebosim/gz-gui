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
StringWidget::StringWidget(const std::string &_key, const unsigned int _level,
    const std::string &_type) : dataPtr(new StringWidgetPrivate())
{
  this->level = _level;

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
  this->setFrameStyle(QFrame::Box);

  // Internal widgets
  this->widgets.push_back(valueEdit);
}

/////////////////////////////////////////////////
StringWidget::~StringWidget()
{
}

/////////////////////////////////////////////////
bool StringWidget::SetValue(const QVariant _value)
{
  auto value = _value.value<std::string>();

  if (this->widgets.size() == 1u)
  {
    if (qobject_cast<QLineEdit *>(this->widgets[0]))
    {
      qobject_cast<QLineEdit *>(this->widgets[0])
          ->setText(tr(value.c_str()));
      return true;
    }
    else if (qobject_cast<QPlainTextEdit *>(this->widgets[0]))
    {
      qobject_cast<QPlainTextEdit *>(this->widgets[0])
          ->setPlainText(tr(value.c_str()));
      return true;
    }
  }

  ignerr << "Error updating bool widget, wrong number of child widgets: ["
         << this->widgets.size() << std::endl;
  return false;
}

/////////////////////////////////////////////////
QVariant StringWidget::Value() const
{
  std::string value;
  if (this->widgets.size() == 1u)
  {
    if (qobject_cast<QLineEdit *>(this->widgets[0]))
    {
      value =
          qobject_cast<QLineEdit *>(this->widgets[0])->text().toStdString();
    }
    else if (qobject_cast<QPlainTextEdit *>(this->widgets[0]))
    {
      value = qobject_cast<QPlainTextEdit *>(this->widgets[0])
          ->toPlainText().toStdString();
    }
  }
  else
  {
    ignerr << "Error getting value from bool widget, wrong number of child "
           << "widgets: [" << this->widgets.size() << std::endl;
  }

  QVariant v;
  v.setValue(value);

  return v;
}
