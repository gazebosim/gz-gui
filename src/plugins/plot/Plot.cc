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

#include <mutex>
#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>

#include "ignition/gui/EditableLabel.hh"
#include "ignition/gui/plugins/plot/Canvas.hh"
#include "ignition/gui/plugins/plot/Curve.hh"
#include "ignition/gui/plugins/plot/ExportDialog.hh"
#include "ignition/gui/plugins/plot/IncrementalPlot.hh"
#include "ignition/gui/plugins/plot/Plot.hh"
#include "ignition/gui/VariablePill.hh"
#include "ignition/gui/VariablePillContainer.hh"

class ignition::gui::plugins::plot::PlotPrivate
{
  /// \brief Mutex to protect the canvas updates
  public: std::mutex mutex;
};

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

/////////////////////////////////////////////////
Plot::Plot()
  : Plugin(), dataPtr(new PlotPrivate)
{
}

/////////////////////////////////////////////////
Plot::~Plot()
{
}

/////////////////////////////////////////////////
void Plot::LoadConfig(const tinyxml2::XMLElement */*_pluginElem*/)
{
  if (this->title.empty())
    this->title = "Plotting Utility";

  // new empty canvas
  auto canvas = new Canvas(this);

  // export button
  QPushButton *exportPlotButton = new QPushButton("Export");
  exportPlotButton->setIcon(QIcon(":/images/file_upload.svg"));
  exportPlotButton->setDefault(false);
  exportPlotButton->setAutoDefault(false);
  exportPlotButton->setToolTip("Export plot data");
  QGraphicsDropShadowEffect *exportPlotShadow = new QGraphicsDropShadowEffect();
  exportPlotShadow->setBlurRadius(8);
  exportPlotShadow->setOffset(0, 0);
  exportPlotButton->setGraphicsEffect(exportPlotShadow);
  this->connect(exportPlotButton, SIGNAL(clicked()), this, SLOT(OnExport()));

  // Plot layout
  auto mainLayout = new QVBoxLayout;
  mainLayout->addWidget(canvas);
  mainLayout->addWidget(exportPlotButton);
  this->setLayout(mainLayout);

  QTimer *displayTimer = new QTimer(this);
  this->connect(displayTimer, SIGNAL(timeout()), this, SLOT(Update()));
  displayTimer->start(30);

  this->setMinimumSize(640, 480);
}

/////////////////////////////////////////////////
void Plot::ShowContextMenu(const QPoint &/*_pos*/)
{
  // Do nothing so the panner works
}

/////////////////////////////////////////////////
void Plot::Update()
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  auto canvas = this->findChild<Canvas *>();
  canvas->Update();
}

/////////////////////////////////////////////////
void Plot::OnExport()
{
  // Get the plots that have data.
  bool hasData = false;
  auto canvas = this->findChild<Canvas *>();

  for (const auto &plot : canvas->Plots())
  {
    for (const auto &curve : plot->Curves())
    {
      auto c = curve.lock();
      if (!c)
        continue;

      hasData = hasData || c->Size() > 0;
    }
  }

  // Display an error message if no plots have data.
  if (!hasData)
  {
    QMessageBox msgBox(
        QMessageBox::Information,
        QString("Unable to export"),
        QString(
          "No data to export.\nAdd variables with data to a graph first."),
        QMessageBox::Close,
        this,
        Qt::Window | Qt::WindowTitleHint |
        Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);
    msgBox.exec();
  }
  else
  {
    std::list<Canvas *> canvases;
    canvases.push_back(canvas);
    auto dialog = new ExportDialog(this, canvases);
    dialog->setModal(true);
    dialog->show();
  }
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::plot::Plot,
                                  ignition::gui::Plugin)
