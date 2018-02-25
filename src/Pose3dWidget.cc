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
Pose3dWidget::Pose3dWidget() : dataPtr(new Pose3dWidgetPrivate())
{
  // Labels
  std::vector<std::string> elements;
  std::vector<std::string> names;

  elements.push_back("x");
  elements.push_back("y");
  elements.push_back("z");
  elements.push_back("roll");
  elements.push_back("pitch");
  elements.push_back("yaw");

  names.push_back("/position/x");
  names.push_back("/position/y");
  names.push_back("/position/z");
  names.push_back("/orientation/roll");
  names.push_back("/orientation/pitch");
  names.push_back("/orientation/yaw");

  // Layout
  auto widgetLayout = new QGridLayout;
  widgetLayout->setColumnStretch(3, 1);

  // Internal widgets
  double min = 0;
  double max = 0;
  rangeFromKey("", min, max);

  this->setLayout(widgetLayout);

  for (unsigned int i = 0; i < elements.size(); ++i)
  {
    auto spin = new QDoubleSpinBox(this);
    this->connect(spin, SIGNAL(editingFinished()), this,
        SLOT(OnValueChanged()));

    spin->setRange(min, max);
    spin->setSingleStep(0.01);
    spin->setDecimals(6);
    spin->setAlignment(Qt::AlignRight);
    spin->setMaximumWidth(100);
    spin->setObjectName(names[i].c_str());

    spin->installEventFilter(this);

    auto label = new QLabel(humanReadable(elements[i]).c_str());
    label->setToolTip(tr(elements[i].c_str()));

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
  if (!_value.canConvert<math::Pose3d>())
  {
    ignerr << "Wrong variant type, expected [ignition::math::Pose3d]"
           << std::endl;
    return false;
  }

  auto value = _value.value<math::Pose3d>();

  auto spins = this->findChildren<QDoubleSpinBox *>();

  spins[0]->setValue(value.Pos().X());
  spins[1]->setValue(value.Pos().Y());
  spins[2]->setValue(value.Pos().Z());

  auto rot = value.Rot().Euler();
  spins[3]->setValue(rot.X());
  spins[4]->setValue(rot.Y());
  spins[5]->setValue(rot.Z());

  return true;
}

/////////////////////////////////////////////////
QVariant Pose3dWidget::Value() const
{
  math::Pose3d value;

  auto spins = this->findChildren<QDoubleSpinBox *>();

  value.Pos().X(spins[0]->value());
  value.Pos().Y(spins[1]->value());
  value.Pos().Z(spins[2]->value());

  math::Vector3d rot;
  rot.X(spins[3]->value());
  rot.Y(spins[4]->value());
  rot.Z(spins[5]->value());
  value.Rot().Euler(rot);

  return QVariant::fromValue(value);
}
