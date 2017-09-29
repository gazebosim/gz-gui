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
#include "ignition/gui/QtMetatypes.hh"

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
      public: QStackedWidget *dimensionWidget;

      /// \brief A spin box for the length of the geometry.
      public: QWidget *lengthSpinBox;

      /// \brief A label for the length widget.
      public: QLabel *lengthLabel;

      /// \brief A label for the unit of the length widget.
      public: QLabel *lengthUnitLabel;

      /// \brief A line edit for the mesh filename.
      public: QLineEdit *filenameLineEdit;

      /// \brief A label for the mesh filename widget.
      public: QLabel *filenameLabel;

      /// \brief A button for selecting the mesh file.
      public: QPushButton *filenameButton;
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
GeometryWidget::GeometryWidget()
    : dataPtr(new GeometryWidgetPrivate())
{
  // Geometry ComboBox
  auto label = new QLabel(tr("Type"));
  label->setToolTip(tr("type"));
  auto comboBox = new QComboBox(this);
  comboBox->addItem(tr("box"));
  comboBox->addItem(tr("cylinder"));
  comboBox->addItem(tr("sphere"));
  comboBox->addItem(tr("mesh"));
  comboBox->addItem(tr("polyline"));
  this->connect(comboBox, SIGNAL(currentIndexChanged(const QString)),
      this, SLOT(OnTypeChanged(const QString)));

  // Size XYZ
  double min = 0;
  double max = 0;
  rangeFromKey("length", min, max);

  auto sizeXSpinBox = new QDoubleSpinBox(this);
  sizeXSpinBox->setObjectName("size");
  sizeXSpinBox->setRange(min, max);
  sizeXSpinBox->setSingleStep(0.01);
  sizeXSpinBox->setDecimals(6);
  sizeXSpinBox->setValue(1.000);
  sizeXSpinBox->setAlignment(Qt::AlignRight);
  sizeXSpinBox->setMaximumWidth(100);
  this->connect(sizeXSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnValueChanged()));

  auto sizeYSpinBox = new QDoubleSpinBox(this);
  sizeYSpinBox->setObjectName("size");
  sizeYSpinBox->setRange(min, max);
  sizeYSpinBox->setSingleStep(0.01);
  sizeYSpinBox->setDecimals(6);
  sizeYSpinBox->setValue(1.000);
  sizeYSpinBox->setAlignment(Qt::AlignRight);
  sizeYSpinBox->setMaximumWidth(100);
  this->connect(sizeYSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnValueChanged()));

  auto sizeZSpinBox = new QDoubleSpinBox(this);
  sizeZSpinBox->setObjectName("size");
  sizeZSpinBox->setRange(min, max);
  sizeZSpinBox->setSingleStep(0.01);
  sizeZSpinBox->setDecimals(6);
  sizeZSpinBox->setValue(1.000);
  sizeZSpinBox->setAlignment(Qt::AlignRight);
  sizeZSpinBox->setMaximumWidth(100);
  this->connect(sizeZSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnValueChanged()));

  auto sizeXLabel = new QLabel(tr("X"));
  auto sizeYLabel = new QLabel(tr("Y"));
  auto sizeZLabel = new QLabel(tr("Z"));
  sizeXLabel->setStyleSheet("QLabel{color: " + kRedColor + ";}");
  sizeYLabel->setStyleSheet("QLabel{color: " + kGreenColor + ";}");
  sizeZLabel->setStyleSheet("QLabel{color: " + kBlueColor + ";}");
  sizeXLabel->setToolTip(tr("x"));
  sizeYLabel->setToolTip(tr("y"));
  sizeZLabel->setToolTip(tr("z"));

  auto unit = unitFromKey("length");
  auto sizeXUnitLabel = new QLabel(QString::fromStdString(unit));
  auto sizeYUnitLabel = new QLabel(QString::fromStdString(unit));
  auto sizeZUnitLabel = new QLabel(QString::fromStdString(unit));

  auto sizeLayout = new QHBoxLayout;
  sizeLayout->addWidget(sizeXLabel);
  sizeLayout->addWidget(sizeXSpinBox);
  sizeLayout->addWidget(sizeXUnitLabel);
  sizeLayout->addWidget(sizeYLabel);
  sizeLayout->addWidget(sizeYSpinBox);
  sizeLayout->addWidget(sizeYUnitLabel);
  sizeLayout->addWidget(sizeZLabel);
  sizeLayout->addWidget(sizeZSpinBox);
  sizeLayout->addWidget(sizeZUnitLabel);

  sizeLayout->setAlignment(sizeXLabel, Qt::AlignRight);
  sizeLayout->setAlignment(sizeYLabel, Qt::AlignRight);
  sizeLayout->setAlignment(sizeZLabel, Qt::AlignRight);

  // Uri
  auto filenameLabel = new QLabel(tr("Uri"));
  filenameLabel->setToolTip(tr("uri"));
  auto filenameLineEdit = new QLineEdit(this);
  this->connect(filenameLineEdit, SIGNAL(editingFinished()), this,
      SLOT(OnGeometryValueChanged()));
  auto filenameButton = new QPushButton(tr("..."));
  filenameButton->setMaximumWidth(30);

  auto filenameLayout = new QHBoxLayout;
  filenameLayout->addWidget(filenameLabel);
  filenameLayout->addWidget(filenameLineEdit);
  filenameLayout->addWidget(filenameButton);

  auto sizeFilenameLayout = new QVBoxLayout;
  sizeFilenameLayout->addLayout(sizeLayout);
  sizeFilenameLayout->addLayout(filenameLayout);

  auto sizeWidget = new QWidget(this);
  sizeWidget->setLayout(sizeFilenameLayout);

  // Radius / Length
  auto radiusLabel = new QLabel(tr("Radius"));
  auto lengthLabel = new QLabel(tr("Length"));
  auto radiusUnitLabel = new QLabel(QString::fromStdString(unit));
  auto lengthUnitLabel = new QLabel(QString::fromStdString(unit));
  radiusLabel->setToolTip(tr("radius"));
  lengthLabel->setToolTip(tr("length"));

  auto radiusSpinBox = new QDoubleSpinBox(this);
  radiusSpinBox->setObjectName("radius");
  radiusSpinBox->setRange(min, max);
  radiusSpinBox->setSingleStep(0.01);
  radiusSpinBox->setDecimals(6);
  radiusSpinBox->setValue(0.500);
  radiusSpinBox->setAlignment(Qt::AlignRight);
  radiusSpinBox->setMaximumWidth(100);
  this->connect(radiusSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnGeometryValueChanged()));

  auto lengthSpinBox = new QDoubleSpinBox(this);
  lengthSpinBox->setObjectName("length");
  lengthSpinBox->setRange(min, max);
  lengthSpinBox->setSingleStep(0.01);
  lengthSpinBox->setDecimals(6);
  lengthSpinBox->setValue(1.000);
  lengthSpinBox->setAlignment(Qt::AlignRight);
  lengthSpinBox->setMaximumWidth(100);
  this->connect(lengthSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnGeometryValueChanged()));

  auto rLLayout = new QHBoxLayout;
  rLLayout->addWidget(radiusLabel);
  rLLayout->addWidget(radiusSpinBox);
  rLLayout->addWidget(radiusUnitLabel);
  rLLayout->addWidget(lengthLabel);
  rLLayout->addWidget(lengthSpinBox);
  rLLayout->addWidget(lengthUnitLabel);

  rLLayout->setAlignment(radiusLabel, Qt::AlignRight);
  rLLayout->setAlignment(lengthLabel, Qt::AlignRight);

  auto rLWidget = new QWidget;
  rLWidget->setLayout(rLLayout);

  // Dimensions
  auto dimensionWidget = new QStackedWidget(this);
  dimensionWidget->insertWidget(0, sizeWidget);

  dimensionWidget->insertWidget(1, rLWidget);
  dimensionWidget->setCurrentIndex(0);
  dimensionWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Layout
  auto widgetLayout = new QGridLayout;
  widgetLayout->addWidget(label, 0, 1);
  widgetLayout->addWidget(comboBox, 0, 2, 1, 2);
  widgetLayout->addWidget(dimensionWidget, 2, 1, 1, 3);

  // ChildWidget
  this->setFrameStyle(QFrame::Box);
  this->dataPtr->dimensionWidget = dimensionWidget;
  this->dataPtr->lengthSpinBox = lengthSpinBox;
  this->dataPtr->lengthLabel = lengthLabel;
  this->dataPtr->lengthUnitLabel = lengthUnitLabel;
  this->dataPtr->filenameLabel = filenameLabel;
  this->dataPtr->filenameLineEdit = filenameLineEdit;
  this->dataPtr->filenameButton = filenameButton;

  filenameLabel->setVisible(false);
  filenameLineEdit->setVisible(false);
  filenameButton->setVisible(false);

  this->connect(filenameButton, SIGNAL(clicked()), this, SLOT(OnSelectFile()));

  this->connect(this, SIGNAL(GeometryChanged()),
      this, SLOT(OnGeometryChanged()));

  this->connect(sizeXSpinBox, SIGNAL(valueChanged(double)),
      this, SLOT(OnValueChanged()));

  this->connect(sizeYSpinBox, SIGNAL(valueChanged(double)),
      this, SLOT(OnValueChanged()));

  this->connect(sizeZSpinBox, SIGNAL(valueChanged(double)),
      this, SLOT(OnValueChanged()));

  this->connect(radiusSpinBox, SIGNAL(valueChanged(double)),
      this, SLOT(OnValueChanged()));

  this->connect(lengthSpinBox, SIGNAL(valueChanged(double)),
      this, SLOT(OnValueChanged()));

  this->setLayout(widgetLayout);
}

/////////////////////////////////////////////////
GeometryWidget::~GeometryWidget()
{
}

/////////////////////////////////////////////////
bool GeometryWidget::SetValue(const QVariant _value)
{
  auto value = _value.value<msgs::Geometry>();

  auto combo = this->findChild<QComboBox *>();
  auto sizeSpins = this->findChildren<QDoubleSpinBox *>("size");
  auto radiusSpin = this->findChild<QDoubleSpinBox *>("radius");
  auto lengthSpin = this->findChild<QDoubleSpinBox *>("length");
  auto edit = this->findChild<QLineEdit *>();

  auto type = msgs::ConvertGeometryType(value.type());
  int index = combo->findText(tr(type.c_str()));

  if (index < 0)
  {
    ignerr << "Error updating Geometry widget: '" << type <<
      "' not found" << std::endl;
    return false;
  }

  combo->setCurrentIndex(index);

  if (type == "box")
  {
    sizeSpins[0]->setValue(value.box().size().x());
    sizeSpins[1]->setValue(value.box().size().y());
    sizeSpins[2]->setValue(value.box().size().z());
  }
  else if (type == "cylinder")
  {
    radiusSpin->setValue(value.cylinder().radius());
    lengthSpin->setValue(value.cylinder().length());
  }
  else if (type == "sphere")
  {
    radiusSpin->setValue(value.sphere().radius());
  }
  else if (type == "mesh")
  {
    sizeSpins[0]->setValue(value.mesh().scale().x());
    sizeSpins[1]->setValue(value.mesh().scale().y());
    sizeSpins[2]->setValue(value.mesh().scale().z());
    edit->setText(tr(value.mesh().filename().c_str()));
  }
  else if (type == "polyline")
  {
    // do nothing
  }
  else
  {
    ignwarn << "Geometry type [" << type << "] not supported" << std::endl;
  }

  return false;
}

/////////////////////////////////////////////////
QVariant GeometryWidget::Value() const
{
  msgs::Geometry value;

  auto combo = this->findChild<QComboBox *>();
  auto sizeSpins = this->findChildren<QDoubleSpinBox *>("size");
  auto radiusSpin = this->findChild<QDoubleSpinBox *>("radius");
  auto lengthSpin = this->findChild<QDoubleSpinBox *>("length");
  auto edit = this->findChild<QLineEdit *>();

  auto type = combo->currentText().toStdString();

  if (type == "box")
  {
    value.set_type(msgs::Geometry::BOX);
    auto size = value.mutable_box()->mutable_size();
    size->set_x(sizeSpins[0]->value());
    size->set_y(sizeSpins[1]->value());
    size->set_z(sizeSpins[2]->value());
  }
  else if (type == "cylinder")
  {
    value.set_type(msgs::Geometry::CYLINDER);
    value.mutable_cylinder()->set_radius(radiusSpin->value());
    value.mutable_cylinder()->set_length(lengthSpin->value());
  }
  else if (type == "sphere")
  {
    value.set_type(msgs::Geometry::SPHERE);
    value.mutable_sphere()->set_radius(radiusSpin->value());
  }
  else if (type == "polyline")
  {
    // do nothing
  }
  else if (type == "mesh")
  {
    value.set_type(msgs::Geometry::MESH);
    auto scale = value.mutable_mesh()->mutable_scale();
    scale->set_x(sizeSpins[0]->value());
    scale->set_y(sizeSpins[1]->value());
    scale->set_z(sizeSpins[2]->value());
    value.mutable_mesh()->set_filename(edit->text().toStdString());
  }
  else
  {
    ignerr << "Error getting geometry dimensions for type: '" << type << "'"
           << std::endl;
  }

  QVariant v;
  v.setValue(value);

  return v;
}

/////////////////////////////////////////////////
void GeometryWidget::OnTypeChanged(const QString &_text)
{
  auto textStr = _text.toStdString();
  bool isMesh = (textStr == "mesh");
  if (textStr == "box" || isMesh)
  {
    this->dataPtr->dimensionWidget->show();
    this->dataPtr->dimensionWidget->setCurrentIndex(0);
  }
  else if (textStr == "cylinder")
  {
    this->dataPtr->dimensionWidget->show();
    this->dataPtr->dimensionWidget->setCurrentIndex(1);
    this->dataPtr->lengthSpinBox->show();
    this->dataPtr->lengthLabel->show();
    this->dataPtr->lengthUnitLabel->show();
  }
  else if (textStr == "sphere")
  {
    this->dataPtr->dimensionWidget->show();
    this->dataPtr->dimensionWidget->setCurrentIndex(1);
    this->dataPtr->lengthSpinBox->hide();
    this->dataPtr->lengthLabel->hide();
    this->dataPtr->lengthUnitLabel->hide();
  }
  else if (textStr == "polyline")
  {
    this->dataPtr->dimensionWidget->hide();
  }

  this->dataPtr->filenameLabel->setVisible(isMesh);
  this->dataPtr->filenameLineEdit->setVisible(isMesh);
  this->dataPtr->filenameButton->setVisible(isMesh);
  this->OnValueChanged();
}

/////////////////////////////////////////////////
void GeometryWidget::OnSelectFile()
{
  QFileDialog fd(this, tr("Select mesh file"), QDir::homePath(),
    tr("Mesh files (*.dae *.stl)"));
  fd.setFilter(QDir::AllDirs | QDir::Hidden);
  fd.setFileMode(QFileDialog::ExistingFile);
  fd.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);
  if (fd.exec())
  {
    if (!fd.selectedFiles().isEmpty())
    {
      auto file = fd.selectedFiles().at(0);
      if (!file.isEmpty())
      {
        this->dataPtr->filenameLineEdit->setText(file);
      }
    }
  }
}

