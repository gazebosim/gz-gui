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

#include <ignition/common/PluginMacros.hh>

#include "ignition/gui/EditableLabel.hh"
#include "ignition/gui/plugins/plot/Curve.hh"
#include "ignition/gui/plugins/plot/IncrementalPlot.hh"
#include "ignition/gui/plugins/plot/Plot.hh"
#include "ignition/gui/VariablePill.hh"
#include "ignition/gui/VariablePillContainer.hh"

class ignition::gui::plugins::plot::PlotPrivate
{
  /// \brief ToDo: Only here for testing purposes.
  public: EditableLabel *label = nullptr;

  /// \brief ToDo: Only here for testing purposes.
  public: VariablePillContainer *yVariableContainer = nullptr;

  /// \brief ToDo: Only here for testing purposes.
  public: IncrementalPlot *emptyPlot = nullptr;
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

  this->dataPtr->label = new EditableLabel("An editable label");

  VariablePillContainer *xVariableContainer = new VariablePillContainer(this);
  xVariableContainer->SetText("x ");
  xVariableContainer->SetMaxSize(1);
  xVariableContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  xVariableContainer->setContentsMargins(0, 0, 0, 0);
  // \todo: fix hardcoded x axis
  xVariableContainer->AddVariablePill("time");
  xVariableContainer->setEnabled(true);

  this->dataPtr->yVariableContainer = new VariablePillContainer(this);
  this->dataPtr->yVariableContainer->SetText("y ");
  this->dataPtr->yVariableContainer->setSizePolicy(
      QSizePolicy::Expanding, QSizePolicy::Fixed);
  this->dataPtr->yVariableContainer->setContentsMargins(0, 0, 0, 0);

  this->dataPtr->yVariableContainer->AddVariablePill("pose/X");
  this->dataPtr->yVariableContainer->AddVariablePill("pose/Y");

  // Empty plot.
  this->dataPtr->emptyPlot = new IncrementalPlot(this);

  // TODO: Remove curves once the plugin is fully implemented
  auto curve01 = this->dataPtr->emptyPlot->AddCurve("Curve 01");
  auto c01 = curve01.lock();
  c01->AddPoint(math::Vector2d(1.2, 94));
  c01->AddPoint(math::Vector2d(2.3, 99));
  c01->AddPoint(math::Vector2d(3.6, 97));

  auto curve02 = this->dataPtr->emptyPlot->AddCurve("Curve 02");
  auto c02 = curve02.lock();
  c02->AddPoint(math::Vector2d(1.6, 95));
  c02->AddPoint(math::Vector2d(4.6, 91));
  c02->AddPoint(math::Vector2d(7.0, 90));

  QVBoxLayout *variableContainerLayout = new QVBoxLayout;
  variableContainerLayout->addWidget(this->dataPtr->label);
  variableContainerLayout->addWidget(xVariableContainer);
  variableContainerLayout->addWidget(this->dataPtr->yVariableContainer);
  variableContainerLayout->addWidget(this->dataPtr->emptyPlot);
  variableContainerLayout->setSpacing(0);
  variableContainerLayout->setContentsMargins(0, 0, 0, 0);

  QFrame *mainFrame = new QFrame;
  mainFrame->setObjectName("plotCanvasFrame");
  QVBoxLayout *mainFrameLayout = new QVBoxLayout;
  mainFrameLayout->addLayout(variableContainerLayout);
  mainFrameLayout->setContentsMargins(0, 0, 0, 0);
  mainFrame->setLayout(mainFrameLayout);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(mainFrame);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(mainLayout);
}

/////////////////////////////////////////////////
void Plot::ShowContextMenu(const QPoint &/*_pos*/)
{
  // Do nothing so the panner works
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::plot::Plot,
                                  ignition::gui::Plugin)
