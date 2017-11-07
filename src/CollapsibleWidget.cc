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

#include <string>

#include "ignition/gui/CollapsibleWidget.hh"
#include "ignition/gui/Helpers.hh"

using namespace ignition;
using namespace gui;

namespace ignition
{
  namespace gui
  {
    class CollapsibleWidgetPrivate
    {
      /// \brief Whether the widget is collapsed or expanded.
      public: bool expanded = true;
    };
  }
}

/////////////////////////////////////////////////
CollapsibleWidget::CollapsibleWidget(const std::string &_key)
    : dataPtr(new CollapsibleWidgetPrivate)
{
  // Button label
  auto buttonLabel = new QLabel(tr(humanReadable(_key).c_str()));
  buttonLabel->setToolTip(tr(_key.c_str()));

  // Button icon initialized to ▲
  auto buttonIcon = new QLabel(QString::fromUtf8("\u25b2"));
  buttonIcon->setObjectName("buttonIcon");

  // Button layout
  auto buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(buttonLabel);
  buttonLayout->addWidget(buttonIcon);
  buttonLayout->setAlignment(buttonIcon, Qt::AlignRight);

  // Button
  auto button = new QPushButton();
  button->setLayout(buttonLayout);
  button->setCheckable(true);
  button->setObjectName("collapsibleButton");

  this->connect(button, SIGNAL(toggled(bool)), this, SLOT(Toggle(bool)));

  // Layout
  auto mainLayout = new QVBoxLayout;
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  mainLayout->addWidget(button);
  this->setLayout(mainLayout);
}

/////////////////////////////////////////////////
CollapsibleWidget::~CollapsibleWidget()
{
}

/////////////////////////////////////////////////
void CollapsibleWidget::Toggle(const bool _checked)
{
  // Toggle all items below the button in the main layout
  for (auto i = 1; i < this->layout()->count(); ++i)
  {
    this->layout()->itemAt(i)->widget()->setVisible(_checked);
  }

  auto icon = this->findChild<QLabel *>("buttonIcon");
  // Change to ▼
  if (_checked)
    icon->setText(QString::fromUtf8("\u25bc"));
  // Change to ▲
  else
    icon->setText(QString::fromUtf8("\u25b2"));

  this->dataPtr->expanded = _checked;
}

/////////////////////////////////////////////////
bool CollapsibleWidget::SetValue(const QVariant _value)
{
  // Set value of first property
  auto prop = this->findChild<PropertyWidget *>();

  if (!prop)
    return false;

  return prop->SetValue(_value);
}

/////////////////////////////////////////////////
QVariant CollapsibleWidget::Value() const
{
  // Get value of first property
  auto prop = this->findChild<PropertyWidget *>();

  if (!prop)
    return QVariant();

  return prop->Value();
}

/////////////////////////////////////////////////
bool CollapsibleWidget::IsExpanded() const
{
  return this->dataPtr->expanded;
}

