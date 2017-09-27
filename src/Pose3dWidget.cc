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
#include <ignition/math/Pose3.hh>

#include "ignition/gui/Helpers.hh"
#include "ignition/gui/QtMetatypes.hh"

#include "ignition/gui/Pose3dWidget.hh"

namespace ignition
{
  namespace gui
  {
    class PropertyWidget;

    /// \brief Private data for the Pose3dWidget class.
    class Pose3dWidgetPrivate
    {
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
Pose3dWidget::Pose3dWidget(const std::string &/*_key*/, const unsigned int _level)
    : dataPtr(new Pose3dWidgetPrivate())
{
  this->level = _level;

  // Labels
  std::vector<std::string> elements;
  elements.push_back("x");
  elements.push_back("y");
  elements.push_back("z");
  elements.push_back("roll");
  elements.push_back("pitch");
  elements.push_back("yaw");

  // Layout
  auto widgetLayout = new QGridLayout;
  widgetLayout->setColumnStretch(3, 1);

  // Internal widgets
  double min = 0;
  double max = 0;
  rangeFromKey("", min, max);

  this->setLayout(widgetLayout);
  this->setFrameStyle(QFrame::Box);

  for (unsigned int i = 0; i < elements.size(); ++i)
  {
    auto spin = new QDoubleSpinBox(this);
    this->connect(spin, SIGNAL(editingFinished()), this,
        SLOT(OnValueChanged()));
    this->widgets.push_back(spin);

    spin->setRange(min, max);
    spin->setSingleStep(0.01);
    spin->setDecimals(6);
    spin->setAlignment(Qt::AlignRight);
    spin->setMaximumWidth(100);

    auto label = new QLabel(humanReadable(elements[i]).c_str());
    label->setToolTip(tr(elements[i].c_str()));
    if (i == 0)
      label->setStyleSheet("QLabel{color: " + kRedColor + ";}");
    else if (i == 1)
      label->setStyleSheet("QLabel{color: " + kGreenColor + ";}");
    else if (i == 2)
      label->setStyleSheet("QLabel{color:" + kBlueColor + ";}");

    auto unitLabel = new QLabel();
    unitLabel->setMaximumWidth(40);
    unitLabel->setMinimumWidth(40);
    if (i < 3)
      unitLabel->setText(QString::fromStdString(unitFromKey("pos")));
    else
      unitLabel->setText(QString::fromStdString(unitFromKey("rot")));

    widgetLayout->addWidget(label, i%3, std::floor(i/3)*3+1);
    widgetLayout->addWidget(spin, i%3, std::floor(i/3)*3+2);
    widgetLayout->addWidget(unitLabel, i%3, std::floor(i/3)*3+3);

    widgetLayout->setAlignment(label, Qt::AlignLeft);
    widgetLayout->setAlignment(spin, Qt::AlignLeft);
    widgetLayout->setAlignment(unitLabel, Qt::AlignLeft);
  }
}

/////////////////////////////////////////////////
Pose3dWidget::~Pose3dWidget()
{
}

/////////////////////////////////////////////////
bool Pose3dWidget::SetValue(const QVariant _value)
{
  auto value = _value.value<math::Pose3d>();

  if (this->widgets.size() == 6u)
  {
    qobject_cast<QDoubleSpinBox *>(this->widgets[0])->setValue(
        value.Pos().X());
    qobject_cast<QDoubleSpinBox *>(this->widgets[1])->setValue(
        value.Pos().Y());
    qobject_cast<QDoubleSpinBox *>(this->widgets[2])->setValue(
        value.Pos().Z());

    auto rot = value.Rot().Euler();
    qobject_cast<QDoubleSpinBox *>(this->widgets[3])->setValue(rot.X());
    qobject_cast<QDoubleSpinBox *>(this->widgets[4])->setValue(rot.Y());
    qobject_cast<QDoubleSpinBox *>(this->widgets[5])->setValue(rot.Z());
    return true;
  }

  ignerr << "Error updating pose widget, wrong number of child widgets: ["
         << this->widgets.size() << std::endl;
  return false;
}

/////////////////////////////////////////////////
QVariant Pose3dWidget::Value() const
{
  math::Pose3d value;
  if (this->widgets.size() == 6u)
  {
    value.Pos().X(qobject_cast<QDoubleSpinBox *>(this->widgets[0])->value());
    value.Pos().Y(qobject_cast<QDoubleSpinBox *>(this->widgets[1])->value());
    value.Pos().Z(qobject_cast<QDoubleSpinBox *>(this->widgets[2])->value());

    math::Vector3d rot;
    rot.X(qobject_cast<QDoubleSpinBox *>(this->widgets[3])->value());
    rot.Y(qobject_cast<QDoubleSpinBox *>(this->widgets[4])->value());
    rot.Z(qobject_cast<QDoubleSpinBox *>(this->widgets[5])->value());
    value.Rot().Euler(rot);
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
