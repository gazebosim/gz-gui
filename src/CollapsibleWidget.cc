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

#include "ignition/gui/Helpers.hh"
#include "ignition/gui/CollapsibleWidget.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
CollapsibleWidget::CollapsibleWidget(const std::string &_name,
    PropertyWidget *_childWidget, const int _level)
{
  // this->level = _level;

  // Button label
  auto buttonLabel = new QLabel(tr(humanReadable(_name).c_str()));
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
  buttonLayout->addWidget(buttonLabel);
  buttonLayout->addWidget(buttonIcon);
  buttonLayout->setAlignment(buttonIcon, Qt::AlignRight);

  // Button frame
  auto buttonFrame = new QFrame();
  buttonFrame->setFrameStyle(QFrame::Box);
  buttonFrame->setLayout(buttonLayout);

  // Set color for top level button
  // FIXME: color stuff
  if (_level == 0)
  {
    buttonFrame->setStyleSheet(
        "QWidget\
        {\
          background-color: " + kBgColors[0] +
        "}");
  }

  // Child widgets are contained in a group box which can be collapsed
  this->setStyleSheet(
      "QGroupBox {\
        border : 0;\
        margin : 0;\
        padding : 0;\
      }");

  this->connect(buttonIcon, SIGNAL(toggled(bool)), this, SLOT(Toggle(bool)));

  // Set the child widget
  this->childWidget = _childWidget;
  _childWidget->setParent(this);
  _childWidget->setContentsMargins(0, 0, 0, 0);

  // Set color for children
  // FIXME: color stuff
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
  this->setLayout(configCollapsibleLayout);

  // Start collapsed
  this->Toggle(false);
}

/////////////////////////////////////////////////
void CollapsibleWidget::Toggle(bool _checked)
{
  if (!this->childWidget)
    return;

  this->childWidget->setVisible(_checked);
}

