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

#include <ignition/msgs/geometry.pb.h>
#include <ignition/msgs/Utility.hh>

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
      public: QWidget *lengthLabel;

      /// \brief A label for the unit of the length widget.
      public: QWidget *lengthUnitLabel;

      /// \brief A line edit for the mesh filename.
      public: QWidget *filenameLineEdit;

      /// \brief A label for the mesh filename widget.
      public: QWidget *filenameLabel;

      /// \brief A button for selecting the mesh file.
      public: QWidget *filenameButton;
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
GeometryWidget::GeometryWidget(const std::string &/*_key*/,
    const unsigned int _level)
    : dataPtr(new GeometryWidgetPrivate())
{
  this->level = _level;

  // Geometry ComboBox
  auto geometryLabel = new QLabel(tr("Type"));
  geometryLabel->setToolTip(tr("type"));
  auto geometryComboBox = new QComboBox(this);
  geometryComboBox->addItem(tr("box"));
  geometryComboBox->addItem(tr("cylinder"));
  geometryComboBox->addItem(tr("sphere"));
  geometryComboBox->addItem(tr("mesh"));
  geometryComboBox->addItem(tr("polyline"));
  this->connect(geometryComboBox, SIGNAL(currentIndexChanged(const QString)),
      this, SLOT(OnTypeChanged(const QString)));

  // Size XYZ
  double min = 0;
  double max = 0;
  rangeFromKey("length", min, max);

  auto geomSizeXSpinBox = new QDoubleSpinBox(this);
  geomSizeXSpinBox->setRange(min, max);
  geomSizeXSpinBox->setSingleStep(0.01);
  geomSizeXSpinBox->setDecimals(6);
  geomSizeXSpinBox->setValue(1.000);
  geomSizeXSpinBox->setAlignment(Qt::AlignRight);
  geomSizeXSpinBox->setMaximumWidth(100);
  this->connect(geomSizeXSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnValueChanged()));

  auto geomSizeYSpinBox = new QDoubleSpinBox(this);
  geomSizeYSpinBox->setRange(min, max);
  geomSizeYSpinBox->setSingleStep(0.01);
  geomSizeYSpinBox->setDecimals(6);
  geomSizeYSpinBox->setValue(1.000);
  geomSizeYSpinBox->setAlignment(Qt::AlignRight);
  geomSizeYSpinBox->setMaximumWidth(100);
  this->connect(geomSizeYSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnValueChanged()));

  auto geomSizeZSpinBox = new QDoubleSpinBox(this);
  geomSizeZSpinBox->setRange(min, max);
  geomSizeZSpinBox->setSingleStep(0.01);
  geomSizeZSpinBox->setDecimals(6);
  geomSizeZSpinBox->setValue(1.000);
  geomSizeZSpinBox->setAlignment(Qt::AlignRight);
  geomSizeZSpinBox->setMaximumWidth(100);
  this->connect(geomSizeZSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnValueChanged()));

  auto geomSizeXLabel = new QLabel(tr("X"));
  auto geomSizeYLabel = new QLabel(tr("Y"));
  auto geomSizeZLabel = new QLabel(tr("Z"));
  geomSizeXLabel->setStyleSheet("QLabel{color: " + kRedColor + ";}");
  geomSizeYLabel->setStyleSheet("QLabel{color: " + kGreenColor + ";}");
  geomSizeZLabel->setStyleSheet("QLabel{color: " + kBlueColor + ";}");
  geomSizeXLabel->setToolTip(tr("x"));
  geomSizeYLabel->setToolTip(tr("y"));
  geomSizeZLabel->setToolTip(tr("z"));

  auto unit = unitFromKey("length");
  auto geomSizeXUnitLabel = new QLabel(QString::fromStdString(unit));
  auto geomSizeYUnitLabel = new QLabel(QString::fromStdString(unit));
  auto geomSizeZUnitLabel = new QLabel(QString::fromStdString(unit));

  auto geomSizeLayout = new QHBoxLayout;
  geomSizeLayout->addWidget(geomSizeXLabel);
  geomSizeLayout->addWidget(geomSizeXSpinBox);
  geomSizeLayout->addWidget(geomSizeXUnitLabel);
  geomSizeLayout->addWidget(geomSizeYLabel);
  geomSizeLayout->addWidget(geomSizeYSpinBox);
  geomSizeLayout->addWidget(geomSizeYUnitLabel);
  geomSizeLayout->addWidget(geomSizeZLabel);
  geomSizeLayout->addWidget(geomSizeZSpinBox);
  geomSizeLayout->addWidget(geomSizeZUnitLabel);

  geomSizeLayout->setAlignment(geomSizeXLabel, Qt::AlignRight);
  geomSizeLayout->setAlignment(geomSizeYLabel, Qt::AlignRight);
  geomSizeLayout->setAlignment(geomSizeZLabel, Qt::AlignRight);

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

  auto geomSizeFilenameLayout = new QVBoxLayout;
  geomSizeFilenameLayout->addLayout(geomSizeLayout);
  geomSizeFilenameLayout->addLayout(filenameLayout);

  auto geomSizeWidget = new QWidget(this);
  geomSizeWidget->setLayout(geomSizeFilenameLayout);

  // Radius / Length
  auto geomRadiusLabel = new QLabel(tr("Radius"));
  auto lengthLabel = new QLabel(tr("Length"));
  auto geomRadiusUnitLabel = new QLabel(QString::fromStdString(unit));
  auto lengthUnitLabel = new QLabel(QString::fromStdString(unit));
  geomRadiusLabel->setToolTip(tr("radius"));
  lengthLabel->setToolTip(tr("length"));

  auto geomRadiusSpinBox = new QDoubleSpinBox(this);
  geomRadiusSpinBox->setRange(min, max);
  geomRadiusSpinBox->setSingleStep(0.01);
  geomRadiusSpinBox->setDecimals(6);
  geomRadiusSpinBox->setValue(0.500);
  geomRadiusSpinBox->setAlignment(Qt::AlignRight);
  geomRadiusSpinBox->setMaximumWidth(100);
  this->connect(geomRadiusSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnGeometryValueChanged()));

  auto lengthSpinBox = new QDoubleSpinBox(this);
  lengthSpinBox->setRange(min, max);
  lengthSpinBox->setSingleStep(0.01);
  lengthSpinBox->setDecimals(6);
  lengthSpinBox->setValue(1.000);
  lengthSpinBox->setAlignment(Qt::AlignRight);
  lengthSpinBox->setMaximumWidth(100);
  this->connect(lengthSpinBox, SIGNAL(editingFinished()), this,
      SLOT(OnGeometryValueChanged()));

  auto geomRLLayout = new QHBoxLayout;
  geomRLLayout->addWidget(geomRadiusLabel);
  geomRLLayout->addWidget(geomRadiusSpinBox);
  geomRLLayout->addWidget(geomRadiusUnitLabel);
  geomRLLayout->addWidget(lengthLabel);
  geomRLLayout->addWidget(lengthSpinBox);
  geomRLLayout->addWidget(lengthUnitLabel);

  geomRLLayout->setAlignment(geomRadiusLabel, Qt::AlignRight);
  geomRLLayout->setAlignment(lengthLabel, Qt::AlignRight);

  auto geomRLWidget = new QWidget;
  geomRLWidget->setLayout(geomRLLayout);

  // Dimensions
  auto dimensionWidget = new QStackedWidget(this);
  dimensionWidget->insertWidget(0, geomSizeWidget);

  dimensionWidget->insertWidget(1, geomRLWidget);
  dimensionWidget->setCurrentIndex(0);
  dimensionWidget->setSizePolicy(
      QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Layout
  auto widgetLayout = new QGridLayout;
  widgetLayout->addWidget(geometryLabel, 0, 1);
  widgetLayout->addWidget(geometryComboBox, 0, 2, 1, 2);
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

  this->connect(geomSizeXSpinBox, SIGNAL(valueChanged(double)),
      this, SLOT(OnValueChanged()));

  this->connect(geomSizeYSpinBox, SIGNAL(valueChanged(double)),
      this, SLOT(OnValueChanged()));

  this->connect(geomSizeZSpinBox, SIGNAL(valueChanged(double)),
      this, SLOT(OnValueChanged()));

  this->connect(geomRadiusSpinBox, SIGNAL(valueChanged(double)),
      this, SLOT(OnValueChanged()));

  this->connect(lengthSpinBox, SIGNAL(valueChanged(double)),
      this, SLOT(OnValueChanged()));

  this->setLayout(widgetLayout);
  this->widgets.push_back(geometryComboBox);
  this->widgets.push_back(geomSizeXSpinBox);
  this->widgets.push_back(geomSizeYSpinBox);
  this->widgets.push_back(geomSizeZSpinBox);
  this->widgets.push_back(geomRadiusSpinBox);
  this->widgets.push_back(lengthSpinBox);
  this->widgets.push_back(filenameLineEdit);
  this->widgets.push_back(filenameButton);
}

/////////////////////////////////////////////////
GeometryWidget::~GeometryWidget()
{
}

/////////////////////////////////////////////////
bool GeometryWidget::SetValue(const QVariant _value)
{
  auto value = _value.value<msgs::Geometry>();

  if (this->widgets.size() != 8u)
  {
    ignerr << "Error updating Geometry widget " << std::endl;
    return false;
  }

  auto type = msgs::ConvertGeometryType(value.type());

  auto valueComboBox = qobject_cast<QComboBox *>(this->widgets[0]);
  int index = valueComboBox->findText(tr(type.c_str()));

  if (index < 0)
  {
    ignerr << "Error updating Geometry widget: '" << type <<
      "' not found" << std::endl;
    return false;
  }

  qobject_cast<QComboBox *>(this->widgets[0])->setCurrentIndex(index);

  if (type == "box")
  {
    qobject_cast<QDoubleSpinBox *>(this->widgets[1])->setValue(
        value.box().size().x());
    qobject_cast<QDoubleSpinBox *>(this->widgets[2])->setValue(
        value.box().size().y());
    qobject_cast<QDoubleSpinBox *>(this->widgets[3])->setValue(
        value.box().size().z());
  }
  else if (type == "cylinder")
  {
    qobject_cast<QDoubleSpinBox *>(this->widgets[4])->setValue(
        value.cylinder().radius());
    qobject_cast<QDoubleSpinBox *>(this->widgets[5])->setValue(
        value.cylinder().length());
  }
  else if (type == "sphere")
  {
    qobject_cast<QDoubleSpinBox *>(this->widgets[4])->setValue(
        value.sphere().radius());
  }
  else if (type == "mesh")
  {
    qobject_cast<QLineEdit *>(this->widgets[6])->setText(tr(
      value.mesh().filename().c_str()));
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

  if (this->widgets.size() != 8u)
  {
    ignerr << "Error getting value from Geometry widget " << std::endl;
    return QVariant();
  }

  auto valueComboBox = qobject_cast<QComboBox *>(this->widgets[0]);
  auto type = valueComboBox->currentText().toStdString();

  if (type == "box")
  {
    value.set_type(msgs::Geometry::BOX);
    auto size = value.mutable_box()->mutable_size();
    size->set_x(qobject_cast<QDoubleSpinBox *>(this->widgets[1])->value());
    size->set_y(qobject_cast<QDoubleSpinBox *>(this->widgets[2])->value());
    size->set_z(qobject_cast<QDoubleSpinBox *>(this->widgets[3])->value());
  }
  else if (type == "cylinder")
  {
    value.set_type(msgs::Geometry::CYLINDER);
    value.mutable_cylinder()->set_radius(
        qobject_cast<QDoubleSpinBox *>(this->widgets[4])->value());
    value.mutable_cylinder()->set_length(qobject_cast<QDoubleSpinBox *>(
        this->widgets[5])->value());
  }
  else if (type == "sphere")
  {
    value.set_type(msgs::Geometry::SPHERE);
    value.mutable_sphere()->set_radius(
        qobject_cast<QDoubleSpinBox *>(this->widgets[4])->value());
  }
  else if (type == "polyline")
  {
    // do nothing
  }
  else if (type == "mesh")
  {
    value.set_type(msgs::Geometry::MESH);
    auto scale = value.mutable_mesh()->mutable_scale();
    scale->set_x(qobject_cast<QDoubleSpinBox *>(this->widgets[1])->value());
    scale->set_y(qobject_cast<QDoubleSpinBox *>(this->widgets[2])->value());
    scale->set_z(qobject_cast<QDoubleSpinBox *>(this->widgets[3])->value());
    value.mutable_mesh()->set_filename(
        qobject_cast<QLineEdit *>(this->widgets[6])->text().toStdString());
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
igndbg << "OnType " << textStr << std::endl;
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
        dynamic_cast<QLineEdit *>(this->dataPtr->filenameLineEdit)->setText(file);
      }
    }
  }
}

