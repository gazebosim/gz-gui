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

#include <sys/stat.h>
#include <cassert>
#include <map>
#include <tuple>
#include <vector>
#include <mutex>
#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>

#include "ignition/gui/EditableLabel.hh"
#include "ignition/gui/plugins/plot/Curve.hh"
#include "ignition/gui/plugins/plot/IncrementalPlot.hh"
#include "ignition/gui/plugins/plot/Plot.hh"
#include "ignition/gui/plugins/plot/TopicCurveHandler.hh"
#include "ignition/gui/plugins/plot/Types.hh"
#include "ignition/gui/VariablePill.hh"
#include "ignition/gui/VariablePillContainer.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
namespace plot
{
/// \brief Helper data structure to store plot data
class Data
{
  /// \brief Unique id of the plot
  public: unsigned int id;

  /// brief Pointer to the plot
  public: IncrementalPlot *plot = nullptr;

  /// \brief A map of container variable ids to their plot curve ids.
  public: std::map<unsigned int, unsigned int> variableCurves;
};

class PlotPrivate
{
  /// \brief Text label
  public: EditableLabel *title{nullptr};

  /// \brief Splitter that contains all the plots.
  public: QSplitter *plotSplitter{nullptr};

  /// \brief A map of plot id to plot data;
  public: std::map<unsigned int, plot::Data *> plotData;

  /// \brief Initial empty plot. Variables are never added to it, and it is
  /// hidden when other plots are added.
  public: IncrementalPlot *emptyPlot{nullptr};

  /// \brief Container for all the variableCurves on the Y axis.
  public: VariablePillContainer *yVariableContainer{nullptr};

  /// \brief Global plot counter.
  public: static unsigned int globalPlotId;

  /// \brief Handler for updating topic curves
  public: TopicCurveHandler topicCurve;

  /// \brief Mutex to protect the canvas updates
  public: std::mutex mutex;
};
}
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

const unsigned int Plot::EmptyPlot = ignition::math::MAX_UI32;
unsigned int PlotPrivate::globalPlotId = 0;

/////////////////////////////////////////////////
Plot::Plot()
  : Plugin(), dataPtr(new PlotPrivate)
{
}

/////////////////////////////////////////////////
Plot::~Plot()
{
  this->Clear();
}

/////////////////////////////////////////////////
void Plot::LoadConfig(const tinyxml2::XMLElement */*_pluginElem*/)
{
  if (this->title.empty())
    this->title = "Plotting Utility";

  // Plot title
  this->dataPtr->title = new EditableLabel("Plot Name");

  auto titleLayout = new QHBoxLayout;
  titleLayout->addWidget(this->dataPtr->title);
  titleLayout->setAlignment(Qt::AlignHCenter);

  // Settings menu
  auto settingsMenu = new QMenu;
  settingsMenu->setObjectName("material");
  auto clearPlotAct = new QAction("Clear all fields", settingsMenu);
  clearPlotAct->setStatusTip(tr("Clear variables and all plots"));
  this->connect(clearPlotAct, SIGNAL(triggered()), this, SLOT(OnClear()));
  settingsMenu->addAction(clearPlotAct);

  auto showGridAct = new QAction("Show grid", settingsMenu);
  showGridAct->setStatusTip(tr("Show/hide grid lines on plot"));
  showGridAct->setCheckable(true);
  this->connect(showGridAct, SIGNAL(toggled(bool)), this,
      SLOT(OnShowGrid(bool)));
  settingsMenu->addAction(showGridAct);

  auto showHoverLineAct = new QAction("Show hover line", settingsMenu);
  showHoverLineAct->setStatusTip(tr("Show hover line"));
  showHoverLineAct->setCheckable(true);
  this->connect(showHoverLineAct, SIGNAL(toggled(bool)), this,
      SLOT(OnShowHoverLine(bool)));
  settingsMenu->addAction(showHoverLineAct);

  auto exportAct = new QAction("Export", settingsMenu);
  exportAct->setStatusTip(tr("Export plot data"));
  this->connect(exportAct, SIGNAL(triggered()), this,
      SLOT(OnExport()));
  settingsMenu->addAction(exportAct);

  // Settings button
  auto settingsButton = new QToolButton();
  settingsButton->installEventFilter(this);
  settingsButton->setToolTip(tr("Settings"));
  settingsButton->setIcon(QIcon(":/images/settings.svg"));
  settingsButton->setIconSize(QSize(25, 25));
  settingsButton->setFixedSize(QSize(45, 35));
  settingsButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
  settingsButton->setPopupMode(QToolButton::InstantPopup);
  settingsButton->setMenu(settingsMenu);

  // Layout with title + settings
  auto titleSettingsLayout = new QHBoxLayout;
  titleSettingsLayout->addLayout(titleLayout);
  titleSettingsLayout->addWidget(settingsButton);
  titleSettingsLayout->setContentsMargins(0, 0, 0, 0);

  // X variable container
  // \todo: fix hardcoded x axis - issue #18
  auto xVariableContainer = new VariablePillContainer(this);
  xVariableContainer->SetText("x ");
  xVariableContainer->SetMaxSize(1);
  xVariableContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  xVariableContainer->setContentsMargins(0, 0, 0, 0);
  xVariableContainer->AddVariablePill("time");
  xVariableContainer->setEnabled(false);

  // Y variable container
  this->dataPtr->yVariableContainer = new VariablePillContainer(this);
  this->dataPtr->yVariableContainer->SetText("y ");
  this->dataPtr->yVariableContainer->setSizePolicy(
      QSizePolicy::Expanding, QSizePolicy::Fixed);
  this->dataPtr->yVariableContainer->setContentsMargins(0, 0, 0, 0);

  this->connect(this->dataPtr->yVariableContainer,
      SIGNAL(VariableAdded(unsigned int, std::string, unsigned int)),
      this, SLOT(OnAddVariableFromPill(unsigned int, std::string, unsigned int)));
  this->connect(this->dataPtr->yVariableContainer,
      SIGNAL(VariableRemoved(unsigned int, unsigned int)),
      this, SLOT(OnRemoveVariableFromPill(unsigned int, unsigned int)));
  this->connect(this->dataPtr->yVariableContainer,
      SIGNAL(VariableMoved(unsigned int, unsigned int)),
      this, SLOT(OnMoveVariableFromPill(unsigned int, unsigned int)));

  // Variable container layout
  auto variableContainerLayout = new QVBoxLayout;
  variableContainerLayout->addWidget(xVariableContainer);
  variableContainerLayout->addWidget(this->dataPtr->yVariableContainer);
  variableContainerLayout->setSpacing(0);
  variableContainerLayout->setContentsMargins(0, 0, 0, 0);

  // Start with an empty plot
  this->dataPtr->emptyPlot = new IncrementalPlot(this);
  this->connect(this->dataPtr->emptyPlot, SIGNAL(VariableAdded(std::string)),
      this, SLOT(OnAddVariableFromPlot(std::string)));
  showGridAct->setChecked(this->dataPtr->emptyPlot->IsShowGrid());
  showHoverLineAct->setChecked(this->dataPtr->emptyPlot->IsShowHoverLine());

  // Splitter to add subsequent plots
  this->dataPtr->plotSplitter = new QSplitter(Qt::Vertical);
  this->dataPtr->plotSplitter->setVisible(false);
  this->dataPtr->plotSplitter->setChildrenCollapsible(false);

  auto plotsLayout = new QVBoxLayout();
  plotsLayout->addWidget(this->dataPtr->emptyPlot);
  plotsLayout->addWidget(this->dataPtr->plotSplitter);

  auto plotScrollArea = new QScrollArea(this);
  plotScrollArea->setLineWidth(0);
  plotScrollArea->setFrameShape(QFrame::NoFrame);
  plotScrollArea->setFrameShadow(QFrame::Plain);
  plotScrollArea->setSizePolicy(QSizePolicy::Minimum,
                                QSizePolicy::Expanding);
  plotScrollArea->setWidgetResizable(true);
  plotScrollArea->viewport()->installEventFilter(this);
  plotScrollArea->setLayout(plotsLayout);

  // Main layout
  auto mainLayout = new QVBoxLayout;
  mainLayout->addLayout(titleSettingsLayout);
  mainLayout->addLayout(variableContainerLayout);
  mainLayout->addWidget(plotScrollArea);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(mainLayout);
  this->setMinimumSize(640, 480);

  // Periodically update the plots
  auto displayTimer = new QTimer(this);
  this->connect(displayTimer, SIGNAL(timeout()), this, SLOT(Update()));
  displayTimer->start(30);
}

/////////////////////////////////////////////////
void Plot::ShowContextMenu(const QPoint &/*_pos*/)
{
  // Do nothing so the panner works
}

/////////////////////////////////////////////////
// TODO
void Plot::OnExport()
{
  // Get the plots that have data.
  bool hasData = false;

  for (const auto &plot : this->Plots())
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
/*    std::list<Canvas *> canvases;
    canvases.push_back(canvas);
    auto dialog = new ExportDialog(this, canvases);
    dialog->setModal(true);
    dialog->show();*/
  }
}

/////////////////////////////////////////////////
unsigned int Plot::AddVariableToPill(const std::string &_variable,
    const unsigned int _plotId)
{
  // Target a new pill by default
  auto targetId = VariablePill::EmptyVariable;

  // find a variable that belongs to the specified plotId and make that the
  // the target variable that the new variable will be added to
  auto it = this->dataPtr->plotData.find(_plotId);
  if (it != this->dataPtr->plotData.end() &&
      !it->second->variableCurves.empty())
  {
    targetId = it->second->variableCurves.begin()->first;
  }

  // add to container and let the signals/slots do the work on adding the
  // a new plot with the curve in the overloaded AddVariable function
  return this->dataPtr->yVariableContainer->AddVariablePill(_variable,
      targetId);
}

/////////////////////////////////////////////////
void Plot::AddVariableToPlot(const unsigned int _id,
                             const std::string &_variable,
                             const unsigned int _plotId)
{
  unsigned int plotId;
  if (_plotId == EmptyPlot)
  {
    // create new plot for the variable and add plot to canvas
    plotId = this->AddPlot();
  }
  else
    plotId = _plotId;

  // add variable to existing plot
  auto it = this->dataPtr->plotData.find(plotId);
  if (it == this->dataPtr->plotData.end())
  {
    ignerr << "Failed to find plot [" << plotId << "]" << std::endl;
    return;
  }

  auto p = it->second;
  auto curve = p->plot->AddCurve(_variable);
  auto c = curve.lock();
  if (c)
  {
    p->variableCurves[_id] = c->Id();
  }
  else
  {
    ignerr << "Unable to add curve to plot" << std::endl;
    return;
  }

  // Hide initial empty plot
  if (!this->dataPtr->plotData.empty())
    this->ShowEmptyPlot(false);

  this->dataPtr->topicCurve.AddCurve(_variable, curve);
}

/////////////////////////////////////////////////
void Plot::RemoveVariable(const unsigned int _id,
    const unsigned int _plotId)
{
  auto plotData = this->dataPtr->plotData.end();

  // Loop through all data looking for a matching variable
  if (_plotId == EmptyPlot)
  {
    for (auto pIt = this->dataPtr->plotData.begin();
        pIt != this->dataPtr->plotData.end(); ++pIt)
    {
      auto curve = pIt->second->variableCurves.find(_id);
      if (curve != pIt->second->variableCurves.end())
      {
        plotData = pIt;
        break;
      }
    }
  }
  // Get the plot which the variable belongs to
  else
  {
    plotData = this->dataPtr->plotData.find(_plotId);
  }

  // Variable not found
  if (plotData == this->dataPtr->plotData.end())
  {
    ignerr << "Failed to find plot data" << std::endl;
    return;
  }

  // Get curve
  auto curve = plotData->second->variableCurves.find(_id);
  if (curve == plotData->second->variableCurves.end())
  {
    ignerr << "Failed to find curve [" << _id << "]" << std::endl;
    return;
  }

  // Remove from curve manager
  auto curveId = curve->second;
  auto plotCurve = plotData->second->plot->Curve(curveId);
  this->dataPtr->topicCurve.RemoveCurve(plotCurve);

  // Remove from map
  plotData->second->variableCurves.erase(curve);

  // Remove from plot
  plotData->second->plot->RemoveCurve(curveId);

  // Remove from variable pill container
  this->dataPtr->yVariableContainer->RemoveVariablePill(_id);

  // Delete whole plot if this was its last variable
  if (plotData->second->variableCurves.empty())
  {
    this->RemovePlot(plotData->first);
  }
}

/////////////////////////////////////////////////
unsigned int Plot::AddPlot()
{
  // Create plot
  auto plot = new IncrementalPlot(this);
  plot->setAutoDelete(false);
  plot->ShowGrid(this->dataPtr->emptyPlot->IsShowGrid());
  plot->ShowHoverLine(this->dataPtr->emptyPlot->IsShowHoverLine());
  this->connect(plot, SIGNAL(VariableAdded(std::string)), this,
      SLOT(OnAddVariableFromPlot(std::string)));

  // Store data
  auto p = new Data;
  p->id = this->dataPtr->globalPlotId++;
  p->plot = plot;
  this->dataPtr->plotData[p->id] = p;

  // Add to splitter
  this->dataPtr->plotSplitter->addWidget(plot);
  this->UpdateAxisLabel();

  return p->id;
}

/////////////////////////////////////////////////
bool Plot::RemovePlot(const unsigned int _id)
{
  if (_id == EmptyPlot)
  {
    ignerr << "Trying to delete placeholder empty plot." << std::endl;
    return false;
  }

  auto it = this->dataPtr->plotData.find(_id);
  if (it == this->dataPtr->plotData.end())
  {
    ignerr << "Failed to find plot [" << _id << "]" << std::endl;
    return false;
  }

  // Remove the plot if it does not contain any variableCurves (curves)
  if (it->second->variableCurves.empty())
  {
    it->second->plot->hide();
    delete it->second->plot;
    delete it->second;
    this->dataPtr->plotData.erase(it);

    // Show empty plot if all plots are gone
    if (this->dataPtr->plotData.empty())
      this->ShowEmptyPlot(true);

    return true;
  }

  // remove all variableCurves except last one
  unsigned int plotId = it->first;
  while (it->second->variableCurves.size() > 1)
  {
    auto v = it->second->variableCurves.begin();
    this->RemoveVariable(v->first, plotId);
  }

  // Remove last variable - this will also recursively call this function and
  // delete the plot which causes plot data iterator to be invalid. So do this
  // last.
  this->RemoveVariable(it->second->variableCurves.begin()->first, plotId);

  return true;
}

/////////////////////////////////////////////////
void Plot::Clear()
{
  while (!this->dataPtr->plotData.empty())
  {
    auto p = this->dataPtr->plotData.begin();
    this->RemovePlot(p->first);
  }

  // FIXME: Show empty plot
  this->ShowEmptyPlot(true);
}

/////////////////////////////////////////////////
void Plot::OnAddVariableFromPlot(const std::string &_variable)
{
  auto plot = qobject_cast<IncrementalPlot *>(QObject::sender());

  if (!plot)
    return;

  // Create a new pill when adding to the initial empty plot
  if (plot == this->dataPtr->emptyPlot)
  {
    this->AddVariableToPill(_variable);
    return;
  }

  // Add to an existing pill
  for (const auto &it : this->dataPtr->plotData)
  {
    if (plot == it.second->plot)
    {
      this->AddVariableToPill(_variable, it.second->id);
      return;
    }
  }

  ignerr << "There's no pill corresponding to plot where variable was dropped"
         << std::endl;
}

/////////////////////////////////////////////////
void Plot::OnAddVariableFromPill(const unsigned int _id,
    const std::string &_variable, const unsigned int _colocatedId)
{
  // Add to an existing plot
  if (_colocatedId != VariablePill::EmptyVariable)
  {
    for (const auto it : this->dataPtr->plotData)
    {
      const auto v = it.second->variableCurves.find(_colocatedId);
      if (v != it.second->variableCurves.end())
      {
        this->AddVariableToPlot(_id, _variable, it.second->id);
        return;
      }
    }
  }
  // Add variable to a new plot
  else
  {
    this->AddVariableToPlot(_id, _variable);
    return;
  }

  ignerr << "Failed to add variable to a plot." << std::endl;
}

/////////////////////////////////////////////////
void Plot::OnRemoveVariableFromPill(const unsigned int _id,
    const unsigned int /*_colocatedId*/)
{
  this->RemoveVariable(_id);
}

/////////////////////////////////////////////////
void Plot::OnMoveVariableFromPill(const unsigned int _id,
    const unsigned int _targetId)
{
  auto plotIt = this->dataPtr->plotData.end();
  auto targetPlotIt = this->dataPtr->plotData.end();
  unsigned int curveId = 0;

  // find plot which the variable belongs to
  // find target plot (if any) that the variable will be moved to
  for (auto it = this->dataPtr->plotData.begin();
      it != this->dataPtr->plotData.end(); ++it)
  {
    auto v = it->second->variableCurves.find(_id);
    if (v != it->second->variableCurves.end())
    {
      plotIt = it;
      curveId = v->second;
    }

    if (it->second->variableCurves.find(_targetId) !=
        it->second->variableCurves.end())
    {
      targetPlotIt = it;
    }

    if (plotIt != this->dataPtr->plotData.end() &&
        targetPlotIt != this->dataPtr->plotData.end())
      break;
  }

  if (plotIt == this->dataPtr->plotData.end())
  {
    ignerr << "Couldn't find plot containing variable [" << _id << "]"
           << std::endl;
    return;
  }

  // detach from old plot and attach to new one
  auto p = plotIt->second;

  // detach variable from plot (qwt plot doesn't seem to do anything
  // apart from setting the plot item to null)
  CurvePtr plotCurve = p->plot->DetachCurve(curveId);
  p->variableCurves.erase(p->variableCurves.find(_id));

  if (targetPlotIt != this->dataPtr->plotData.end())
  {
    // attach variable to target plot
    targetPlotIt->second->plot->AttachCurve(plotCurve);
    targetPlotIt->second->variableCurves[_id] = plotCurve->Id();
  }
  else
  {
    // create new plot
    unsigned int plotId = this->AddPlot();
    auto it = this->dataPtr->plotData.find(plotId);
    assert(it != this->dataPtr->plotData.end());
    Data *newPlotData = it->second;
    // attach curve to plot
    newPlotData->plot->AttachCurve(plotCurve);
    newPlotData->variableCurves[_id] = plotCurve->Id();

    // hide initial empty plot
    if (!this->dataPtr->plotData.empty())
      this->ShowEmptyPlot(false);
  }

  // delete plot if empty
  if (p->variableCurves.empty())
  {
    p->plot->hide();

    // careful about deleting by iterator (plotIt) as it may have been
    // changed if a new plot is added to the vector
    for (auto it = this->dataPtr->plotData.begin();
        it != this->dataPtr->plotData.end(); ++it)
    {
      if (it->second == p)
      {
        this->dataPtr->plotData.erase(it);
        break;
      }
    }

    p->plot->detachItems(QwtPlotItem::Rtti_PlotItem, false);
    delete p->plot;
    delete p;

    this->UpdateAxisLabel();
  }
}

/////////////////////////////////////////////////
void Plot::Update()
{
  // Update all the plots
  for (auto p : this->dataPtr->plotData)
    p.second->plot->Update();
}

/////////////////////////////////////////////////
bool Plot::eventFilter(QObject *_o, QEvent *_e)
{
  if (_e->type() == QEvent::Wheel)
  {
    _e->ignore();
    return true;
  }

  return QWidget::eventFilter(_o, _e);
}

/////////////////////////////////////////////////
std::vector<IncrementalPlot *> Plot::Plots() const
{
  std::vector<IncrementalPlot *> plots;
  for (const auto it : this->dataPtr->plotData)
    plots.push_back(it.second->plot);

  return plots;
}

/////////////////////////////////////////////////
std::string Plot::UniqueFilePath(const std::string &_pathAndName,
    const std::string &_extension) const
{
  std::string result = _pathAndName + "." + _extension;
  int count = 1;
  struct stat buf;

  // Check if file exists and change name accordingly
  while (stat(result.c_str(), &buf) != -1)
  {
    result = _pathAndName + "(" + std::to_string(count++) + ")." + _extension;
  }

  return result;
}

/////////////////////////////////////////////////
void Plot::OnClear()
{
  // Ask for confirmation
  std::string msg = "Are you sure you want to clear all fields? \n\n"
        "This will remove all the plots in this canvas. \n";

  QMessageBox msgBox(QMessageBox::Warning, QString("Clear canvas?"),
      QString(msg.c_str()));
  msgBox.setWindowFlags(Qt::Window | Qt::WindowTitleHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  auto cancelButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);
  auto clearButton = msgBox.addButton("Clear", QMessageBox::AcceptRole);
  msgBox.setDefaultButton(clearButton);
  msgBox.setEscapeButton(cancelButton);
  msgBox.show();
  msgBox.move(this->mapToGlobal(this->pos()));
  msgBox.exec();
  if (msgBox.clickedButton() != clearButton)
    return;

  this->Clear();
}

/////////////////////////////////////////////////
void Plot::OnShowGrid(const bool _show)
{
  this->dataPtr->emptyPlot->ShowGrid(_show);

  for (const auto &it : this->dataPtr->plotData)
    it.second->plot->ShowGrid(_show);
}

/////////////////////////////////////////////////
void Plot::OnShowHoverLine(const bool _show)
{
  this->dataPtr->emptyPlot->ShowHoverLine(_show);

  for (const auto &it : this->dataPtr->plotData)
    it.second->plot->ShowHoverLine(_show);
}

/////////////////////////////////////////////////
void Plot::UpdateAxisLabel()
{
  // show the x-axis label in the last plot only
  for (int i = 0; i < this->dataPtr->plotSplitter->count(); ++i)
  {
    IncrementalPlot *p =
        qobject_cast<IncrementalPlot *>(this->dataPtr->plotSplitter->widget(i));

    if (p)
    {
      std::string label = "";
      if (i == (this->dataPtr->plotSplitter->count()-1))
        label = "Time (seconds)";
      p->ShowAxisLabel(IncrementalPlot::X_BOTTOM_AXIS, label);
    }
  }
}

/////////////////////////////////////////////////
void Plot::ShowEmptyPlot(const bool _show)
{
  this->dataPtr->emptyPlot->setVisible(_show);
  this->dataPtr->plotSplitter->setVisible(!_show);
  this->UpdateAxisLabel();
}

/////////////////////////////////////////////////
void Plot::Export(const std::string &_dirName,
    const FileType _type) const
{
  std::string plotTitle = this->Title();

  // Cleanup the title
  std::replace(plotTitle.begin(), plotTitle.end(), '/', '_');
  std::replace(plotTitle.begin(), plotTitle.end(), '?', ':');

  std::string filePrefix = _dirName + "/" + plotTitle;

  if (_type == FileType::PDFFile)
    this->ExportPDF(filePrefix);
  else if (_type == FileType::CSVFile)
    this->ExportCSV(filePrefix);
}

/////////////////////////////////////////////////
void Plot::ExportPDF(const std::string &_filePrefix) const
{
  // Render the plot to a PDF
  int index = 0;
  for (const auto it : this->dataPtr->plotData)
  {
    std::string suffix =
        this->dataPtr->plotData.size() > 1 ? std::to_string(index) : "";

    std::string filename = this->UniqueFilePath(_filePrefix + suffix, "pdf");

    IncrementalPlot *plot = it.second->plot;

    QSizeF docSize(plot->canvas()->width() + plot->legend()->width(),
                   plot->canvas()->height());

    QwtPlotRenderer renderer;
    renderer.renderDocument(plot, QString(filename.c_str()), docSize, 20);

    ignmsg << "Plot exported to file [" << filename << "]" << std::endl;

    index++;
  }
}

/////////////////////////////////////////////////
void Plot::ExportCSV(const std::string &_filePrefix) const
{
  // Save data from each curve into a separate file.
  for (const auto it : this->dataPtr->plotData)
  {
    for (const auto &curve : it.second->plot->Curves())
    {
      auto c = curve.lock();
      if (!c)
        continue;

      // Cleanup the label
      std::string label = c->Label();
      std::replace(label.begin(), label.end(), '/', '_');
      std::replace(label.begin(), label.end(), '?', ':');

      std::string filename =
        this->UniqueFilePath(_filePrefix + "-" + label, "csv");

      std::ofstream out(filename);
      // \todo: fix hardcoded sim_time
      out << "time, " << c->Label() << std::endl;
      for (unsigned int j = 0; j < c->Size(); ++j)
      {
        ignition::math::Vector2d pt = c->Point(j);
        out << pt.X() << ", " << pt.Y() << std::endl;
      }
      out.close();

      ignmsg << "Plot exported to file [" << filename << "]" << std::endl;
    }
  }
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::plot::Plot,
                                  ignition::gui::Plugin)
