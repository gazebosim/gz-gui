/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#include <memory>
#include <mutex>
#include <vector>

#include <ignition/msgs.hh>
#include <ignition/common/URI.hh>
#include <ignition/common/Util.hh>

#include "ignition/gui/plugins/plot/Curve.hh"
#include "ignition/gui/plugins/plot/IncrementalPlot.hh"
#include "ignition/gui/plugins/plot/Plot.hh"
#include "ignition/gui/plugins/plot/Manager.hh"
#include "ignition/gui/plugins/plot/TopicCurveHandler.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

namespace ignition
{
namespace gui
{
namespace plugins
{
namespace plot
{
  /// \brief Private data for the Manager class
  class ManagerPrivate
  {
    /// \brief Mutex to protect plot manager updates.
    public: std::mutex mutex;

    /// \brief Handler for updating topic curves
    public: TopicCurveHandler topicCurve;

    /// \brief A list of plot windows.
    public: std::vector<Plot *> windows;
  };
}
}
}
}

/////////////////////////////////////////////////
Manager::Manager()
  : dataPtr(new ManagerPrivate())
{
}

/////////////////////////////////////////////////
Manager::~Manager()
{
  this->dataPtr->windows.clear();
}

/////////////////////////////////////////////////
void Manager::AddTopicCurve(const std::string &_topic, CurveWeakPtr _curve)
{
  this->dataPtr->topicCurve.AddCurve(_topic, _curve);
}

/////////////////////////////////////////////////
void Manager::RemoveTopicCurve(CurveWeakPtr _curve)
{
  this->dataPtr->topicCurve.RemoveCurve(_curve);
}

/////////////////////////////////////////////////
void Manager::AddWindow(Plot *_window)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->windows.push_back(_window);
}

/////////////////////////////////////////////////
void Manager::RemoveWindow(Plot *_window)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  for (auto it = this->dataPtr->windows.begin();
      it != this->dataPtr->windows.end(); ++it)
  {
    if ((*it) == _window)
    {
      this->dataPtr->windows.erase(it);
      return;
    }
  }
}

/////////////////////////////////////////////////
std::string Manager::HumanReadableName(const std::string &_uri) const
{
  std::string label;

  // expected name format:
  //   scheme: data://
  //   path:   world/world_name/model/model_name/link/link_name
  //   query:  ?p=param_type/param_name
  // convert to friendly name:
  //   name:   model_name/link_name?param_name
  common::URI uri(_uri);
  if (!uri.Valid())
    return _uri;

  common::URIPath path = uri.Path();
  common::URIQuery query = uri.Query();
  std::vector<std::string> pathTokens = common::split(path.Str(), "/");
  std::vector<std::string> queryTokens = common::split(query.Str(), "=/");

  // min path token size 2: [world, world_name]
  // min query token size 3: [p, param_type, param_name]
  if (pathTokens.size() < 2 || queryTokens.size() < 3)
    return label;

  // path: start from model name and ignore world and entity type str for now
  std::string pathStr;
  for (unsigned int i = 3; i < pathTokens.size(); i+=2)
  {
    if (!pathStr.empty())
      pathStr += "/";
    pathStr += pathTokens[i];
  }

  // query: take only first param name
  std::string queryStr;
  for (unsigned int i = 2; i < queryTokens.size(); i+=2)
  {
    if (!queryStr.empty())
      queryStr +="/";
    queryStr += queryTokens[i];
  }
  label = pathStr + "?" + queryStr;

  return label;
}
