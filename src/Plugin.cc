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

#include "ignition/gui/Plugin.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
void Plugin::Load(const tinyxml2::XMLElement *_pluginElem)
{
  // Read default params
  if (_pluginElem)
  {
    if (auto titleElem = _pluginElem->FirstChildElement("title"))
      this->title = titleElem->GetText();

    if (auto hasTitleElem = _pluginElem->FirstChildElement("has_titlebar"))
    {
      bool has = true;
      hasTitleElem->QueryBoolText(&has);
      this->hasTitlebar = has;
    }
  }

  // Setup default context menu
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  this->connect(this,
      SIGNAL(customContextMenuRequested(const QPoint &)),
      this, SLOT(ShowContextMenu(const QPoint &)));

  this->LoadConfig(_pluginElem);
}

/////////////////////////////////////////////////
void Plugin::ShowContextMenu(const QPoint &_pos)
{
  // Close action
  QAction closeAct(QString::fromStdString("Close [" + this->title + "]"),
      this);
  this->connect(&closeAct, SIGNAL(triggered()), this->parent(), SLOT(close()));

  // Context menu
  QMenu contextMenu(tr("Context menu"), this);
  contextMenu.addAction(&closeAct);
  contextMenu.exec(this->mapToGlobal(_pos));
}

