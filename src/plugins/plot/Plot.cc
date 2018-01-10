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
  /// \brief Splitter to hold all the canvases.
  public: QSplitter *canvasSplitter;

  /// \brief Mutex to protect the canvas updates
  public: std::mutex mutex;

  /// \brief Flag to indicate whether the plots should be restarted.
  public: bool restart = false;
};

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

// A special list widget that allows dragging of items from it to a plot.
class DragableListWidget : public QListWidget
{
  public: explicit DragableListWidget(QWidget *_parent)
          : QListWidget(_parent)
          {
          }

  protected: virtual void startDrag(Qt::DropActions /*_supportedActions*/)
             {
               QListWidgetItem *currItem = this->currentItem();
               QMimeData *currMimeData = new QMimeData;
               QByteArray ba;
               ba = currItem->text().toLatin1().data();
               currMimeData->setData("application/x-item", ba);
               QDrag *drag = new QDrag(this);
               drag->setMimeData(currMimeData);
               drag->exec(Qt::LinkAction);
             }

  protected: virtual Qt::DropActions supportedDropActions() const
             {
               return Qt::LinkAction;
             }
};

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
  this->dataPtr->canvasSplitter = new QSplitter(Qt::Vertical);
  this->AddCanvas();

  // add button
  QPushButton *addCanvasButton = new QPushButton("+");
  addCanvasButton->setDefault(false);
  addCanvasButton->setAutoDefault(false);
  addCanvasButton->setToolTip("Add a new canvas");
  QGraphicsDropShadowEffect *addCanvasShadow = new QGraphicsDropShadowEffect();
  addCanvasShadow->setBlurRadius(8);
  addCanvasShadow->setOffset(0, 0);
  addCanvasButton->setGraphicsEffect(addCanvasShadow);
  this->connect(addCanvasButton, SIGNAL(clicked()), this, SLOT(OnAddCanvas()));

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

  QHBoxLayout *addButtonLayout = new QHBoxLayout;
  addButtonLayout->addWidget(exportPlotButton);
  addButtonLayout->addStretch();
  addButtonLayout->addWidget(addCanvasButton);
  addButtonLayout->setAlignment(Qt::AlignRight | Qt::AlignBottom);
  addButtonLayout->setContentsMargins(0, 0, 0, 0);
  addCanvasButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  // Plot layout
  QVBoxLayout *plotLayout = new QVBoxLayout;
  plotLayout->addWidget(this->dataPtr->canvasSplitter);
  plotLayout->addLayout(addButtonLayout);
  plotLayout->setStretchFactor(this->dataPtr->canvasSplitter, 1);
  plotLayout->setStretchFactor(addButtonLayout, 0);

  this->setLayout(plotLayout);

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
Canvas *Plot::AddCanvas()
{
  Canvas *canvas = new Canvas(this);
  this->connect(canvas, SIGNAL(CanvasDeleted()), this, SLOT(OnRemoveCanvas()));

  this->dataPtr->canvasSplitter->addWidget(canvas);

  this->UpdateCanvas();

  return canvas;
}

/////////////////////////////////////////////////
void Plot::RemoveCanvas(Canvas *_canvas)
{
  int idx = this->dataPtr->canvasSplitter->indexOf(_canvas);
  if (idx < 0)
    return;

  _canvas->hide();
  _canvas->setParent(nullptr);
  _canvas->deleteLater();
}

/////////////////////////////////////////////////
void Plot::Clear()
{
  while (this->CanvasCount() > 0u)
  {
    Canvas *canvas =
        qobject_cast<Canvas *>(this->dataPtr->canvasSplitter->widget(0));
    this->RemoveCanvas(canvas);
  }
}

/////////////////////////////////////////////////
unsigned int Plot::CanvasCount() const
{
  return static_cast<unsigned int>(this->dataPtr->canvasSplitter->count());
}

/////////////////////////////////////////////////
void Plot::OnAddCanvas()
{
  this->AddCanvas();
}

/////////////////////////////////////////////////
void Plot::OnRemoveCanvas()
{
  Canvas *canvas = qobject_cast<Canvas *>(QObject::sender());
  if (!canvas)
    return;

  this->RemoveCanvas(canvas);

  // add an empty canvas if the plot window is now empty
  if (this->dataPtr->canvasSplitter->count() == 0)
    this->AddCanvas();
  else
  {
    this->UpdateCanvas();
  }
}

/////////////////////////////////////////////////
void Plot::UpdateCanvas()
{
  // disable Delete Canvas option in settings if there is only one
  // canvas in the window
  Canvas *plotCanvas =
      qobject_cast<Canvas *>(this->dataPtr->canvasSplitter->widget(0));
  if (plotCanvas)
  {
    plotCanvas->SetDeleteCanvasEnabled(
        this->dataPtr->canvasSplitter->count() != 1);
  }
}

/////////////////////////////////////////////////
void Plot::Update()
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  if (this->dataPtr->restart)
  {
    for (int i = 0; i < this->dataPtr->canvasSplitter->count(); ++i)
    {
      Canvas *canvas =
          qobject_cast<Canvas *>(this->dataPtr->canvasSplitter->widget(i));
      if (!canvas)
        continue;
      canvas->Restart();
    }
    this->dataPtr->restart = false;
  }

  for (int i = 0; i < this->dataPtr->canvasSplitter->count(); ++i)
  {
    Canvas *canvas =
        qobject_cast<Canvas *>(this->dataPtr->canvasSplitter->widget(i));
    if (!canvas)
      continue;
    canvas->Update();
  }
}

/////////////////////////////////////////////////
void Plot::Restart()
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->restart = true;
}

/////////////////////////////////////////////////
void Plot::OnExport()
{
  // Get the plots that have data.
  std::list<Canvas*> plots;
  for (int i = 0; i < this->dataPtr->canvasSplitter->count(); ++i)
  {
    bool hasData = false;
    Canvas *canvas =
        qobject_cast<Canvas *>(this->dataPtr->canvasSplitter->widget(i));

    if (!canvas)
      continue;

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

    if (hasData)
      plots.push_back(canvas);
  }

  // Display an error message if no plots have data.
  if (plots.empty())
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
    ExportDialog *dialog = new ExportDialog(this, plots);
    dialog->setModal(true);
    dialog->show();
  }
}

/////////////////////////////////////////////////
std::list<Canvas *> Plot::Plots()
{
  std::list<Canvas *> plots;

  for (int i = 0; i < this->dataPtr->canvasSplitter->count(); ++i)
  {
    Canvas *canvas =
        qobject_cast<Canvas *>(this->dataPtr->canvasSplitter->widget(i));

    if (!canvas)
      continue;
    plots.push_back(canvas);
  }

  return plots;
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::plot::Plot,
                                  ignition::gui::Plugin)
