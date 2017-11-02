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

#include <ignition/msgs/geometry.pb.h>
#include <ignition/msgs/Utility.hh>

#include <ignition/common/Console.hh>

#include "ignition/gui/Helpers.hh"
#include "ignition/gui/NumberWidget.hh"
#include "ignition/gui/QtMetatypes.hh"
#include "ignition/gui/StringWidget.hh"
#include "ignition/gui/Vector3dWidget.hh"

#include "ignition/gui/GeometryWidget.hh"

namespace ignition
{
  namespace gui
  {
    class PropertyWidget;

    /// \brief Private data for the GeometryWidget class.
    class GeometryWidgetPrivate
    {
      /// \brief A stacked widget containing widgets for configuring
      /// geometry dimensions.
      public: QStackedWidget *typeStack;
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
GeometryWidget::GeometryWidget()
    : dataPtr(new GeometryWidgetPrivate())
{
  // Label
  auto label = new QLabel(tr("Type"));
  label->setToolTip(tr("type"));

  // Type combo
  msgs::Geometry msg;
  auto enumDescriptor = msg.Type_descriptor();

  auto comboBox = new QComboBox(this);
  for (int type = 0; type < enumDescriptor->value_count(); ++type)
  {
    auto value = enumDescriptor->value(type);
    comboBox->addItem(QString::fromStdString(value->name()));
  }
  this->connect(comboBox, SIGNAL(currentIndexChanged(const int)),
      this, SLOT(OnTypeChanged(const int)));

  // Box widget
  auto boxWidget = new Vector3dWidget("length");
  boxWidget->setObjectName("boxWidget");
  this->connect(boxWidget, SIGNAL(ValueChanged(const QVariant)), this,
      SLOT(OnValueChanged()));

  // Cylinder widget
  auto cylinderRWidget = new NumberWidget("radius");
  cylinderRWidget->setObjectName("cylinderRWidget");
  this->connect(cylinderRWidget, SIGNAL(ValueChanged(const QVariant)), this,
      SLOT(OnValueChanged()));

  auto cylinderLWidget = new NumberWidget("length");
  cylinderLWidget->setObjectName("cylinderLWidget");
  this->connect(cylinderRWidget, SIGNAL(ValueChanged(const QVariant)), this,
      SLOT(OnValueChanged()));

  auto cylinderLayout = new QHBoxLayout;
  cylinderLayout->addWidget(cylinderRWidget);
  cylinderLayout->addWidget(cylinderLWidget);

  auto cylinderWidget = new QWidget;
  cylinderWidget->setLayout(cylinderLayout);

  // Sphere widget
  auto sphereWidget = new NumberWidget("radius");
  sphereWidget->setObjectName("sphereWidget");
  this->connect(sphereWidget, SIGNAL(ValueChanged(const QVariant)), this,
      SLOT(OnValueChanged()));

  // Mesh widget
  auto meshScaleWidget = new Vector3dWidget("size");
  meshScaleWidget->setObjectName("meshScaleWidget");
  this->connect(meshScaleWidget, SIGNAL(ValueChanged(const QVariant)), this,
      SLOT(OnValueChanged()));

  auto meshUriWidget = new StringWidget("uri");
  meshUriWidget->setObjectName("meshUriWidget");
  this->connect(meshUriWidget, SIGNAL(ValueChanged(const QVariant)), this,
      SLOT(OnValueChanged()));

  auto filenameButton = new QPushButton(tr("..."));
  filenameButton->setMaximumWidth(30);
  this->connect(filenameButton, SIGNAL(clicked()), this, SLOT(OnSelectFile()));

  auto meshLayout = new QGridLayout;
  meshLayout->addWidget(meshScaleWidget, 0, 0, 1, 2);
  meshLayout->addWidget(meshUriWidget, 1, 0, 1, 1);
  meshLayout->addWidget(filenameButton, 1, 1, 1, 1);

  auto meshWidget = new QWidget;
  meshWidget->setLayout(meshLayout);

  // Type - specific (make sure order matches combo)
  this->dataPtr->typeStack = new QStackedWidget();
  this->dataPtr->typeStack->setSizePolicy(QSizePolicy::Minimum,
                                          QSizePolicy::Minimum);

  this->dataPtr->typeStack->insertWidget(0, boxWidget);
  this->dataPtr->typeStack->insertWidget(1, cylinderWidget);
  this->dataPtr->typeStack->insertWidget(2, sphereWidget);
  this->dataPtr->typeStack->insertWidget(3, new QWidget());
  this->dataPtr->typeStack->insertWidget(4, new QWidget());
  this->dataPtr->typeStack->insertWidget(5, new QWidget());
  this->dataPtr->typeStack->insertWidget(6, meshWidget);
  this->dataPtr->typeStack->insertWidget(7, new QWidget());

  // Layout
  auto mainLayout = new QGridLayout;
  mainLayout->addWidget(label, 0, 1);
  mainLayout->addWidget(comboBox, 0, 2, 1, 2);
  mainLayout->addWidget(this->dataPtr->typeStack, 2, 1, 1, 3);
  this->setLayout(mainLayout);
}

/////////////////////////////////////////////////
GeometryWidget::~GeometryWidget()
{
}

/////////////////////////////////////////////////
bool GeometryWidget::SetValue(const QVariant _value)
{
  if (!_value.canConvert<msgs::Geometry>())
  {
    ignerr << "Wrong variant type, expected [ignition::msgs::Geometry]"
           << std::endl;
    return false;
  }

  auto value = _value.value<msgs::Geometry>();

  auto type = msgs::ConvertGeometryType(value.type());

  auto combo = this->findChild<QComboBox *>();
  int index = combo->findText(QString(type.c_str()).toUpper());

  if (index < 0)
  {
    ignwarn << "Type [" << type << "] not supported yet. See issue #10."
            << std::endl;
    return false;
  }

  // Update combo and type stack
  combo->setCurrentIndex(index);
  this->dataPtr->typeStack->setCurrentIndex(index);

  // Update type stack
  auto widget = this->dataPtr->typeStack->widget(index);

  if (type == "box")
  {
    auto size = qobject_cast<Vector3dWidget *>(widget);
    size->SetValue(QVariant::fromValue(msgs::Convert(value.box().size())));
  }
  else if (type == "cylinder")
  {
    auto numbers = widget->findChildren<NumberWidget *>();
    numbers[0]->SetValue(value.cylinder().radius());
    numbers[1]->SetValue(value.cylinder().length());
  }
  else if (type == "sphere")
  {
    auto radius = qobject_cast<NumberWidget *>(widget);
    radius->SetValue(value.cylinder().radius());
  }
  else if (type == "mesh")
  {
    auto scale = widget->findChild<Vector3dWidget *>();
    scale->SetValue(QVariant::fromValue(msgs::Convert(value.mesh().scale())));

    auto uri = widget->findChild<StringWidget *>();
    uri->SetValue(QVariant::fromValue(
        std::string(value.mesh().filename().c_str())));
  }
  else
  {
    ignwarn << "Type [" << type << "] not supported yet. See issue #10."
            << std::endl;
    return false;
  }

  return true;
}

/////////////////////////////////////////////////
QVariant GeometryWidget::Value() const
{
  msgs::Geometry value;

  auto combo = this->findChild<QComboBox *>();

  // Type
  auto type = combo->currentText().toLower().toStdString();
  value.set_type(msgs::ConvertGeometryType(type));

  // From type stack
  auto i = combo->currentIndex();

  // Unsupported types
  if (i > 7)
    i = 7;

  auto widget = this->dataPtr->typeStack->widget(i);
  if (type == "box")
  {
    auto sizeWidget = qobject_cast<Vector3dWidget *>(widget);
    auto size = sizeWidget->Value().value<math::Vector3d>();

    msgs::Set(value.mutable_box()->mutable_size(), size);
  }
  else if (type == "cylinder")
  {
    auto numberWidgets = widget->findChildren<NumberWidget *>();

    value.mutable_cylinder()->set_radius(numberWidgets[0]->Value().toDouble());
    value.mutable_cylinder()->set_length(numberWidgets[1]->Value().toDouble());
  }
  else if (type == "sphere")
  {
    auto numberWidget = qobject_cast<NumberWidget *>(widget);
    value.mutable_sphere()->set_radius(numberWidget->Value().toDouble());
  }
  else if (type == "mesh")
  {
    auto scaleWidget = widget->findChild<Vector3dWidget *>();

    msgs::Set(value.mutable_mesh()->mutable_scale(),
              scaleWidget->Value().value<math::Vector3d>());

    auto uriWidget = widget->findChild<StringWidget *>();
    value.mutable_mesh()->set_filename(uriWidget->Value().value<std::string>());
  }
  else
  {
    ignwarn << "Type [" << type << "] not supported yet. See issue #10."
            << std::endl;
  }

  return QVariant::fromValue(value);
}

/////////////////////////////////////////////////
void GeometryWidget::OnTypeChanged(const int _index)
{
  int i = _index;

  // Unsupported types
  if (i > 7)
    i = 7;

  this->dataPtr->typeStack->setCurrentIndex(i);
  this->OnValueChanged();
}

/////////////////////////////////////////////////
void GeometryWidget::OnSelectFile()
{
  QFileDialog fd(this, tr("Select mesh file"), QDir::homePath(),
    tr("Mesh files (*.dae *.stl *.obj)"));
  fd.setFilter(QDir::AllDirs | QDir::Hidden);
  fd.setFileMode(QFileDialog::ExistingFile);
  fd.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  if (fd.exec())
  {
    if (fd.selectedFiles().isEmpty())
      return;

    auto file = fd.selectedFiles().at(0);
    if (file.isEmpty())
      return;

    auto uri = this->findChild<StringWidget *>("meshUriWidget");
    uri->SetValue(QVariant::fromValue(file.toStdString()));
    this->OnValueChanged();
  }
}

