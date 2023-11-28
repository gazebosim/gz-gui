/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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

#include <algorithm>
#include <list>
#include <map>
#include <string>

#include <QQmlProperty>

#include <gz/msgs/boolean.pb.h>
#include <gz/msgs/marker.pb.h>
#include <gz/msgs/marker_v.pb.h>
#include <gz/msgs/world_stats.pb.h>

#include <gz/common/Console.hh>
#include <gz/common/Profiler.hh>
#include <gz/common/StringUtils.hh>

#include <gz/math/Rand.hh>

#include <gz/plugin/Register.hh>

#include "gz/rendering/Marker.hh"
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>

#include <gz/msgs/Utility.hh>
#include <gz/transport/Node.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/GuiEvents.hh"
#include "gz/gui/Helpers.hh"
#include "gz/gui/MainWindow.hh"

#include "MarkerManager.hh"

namespace gz::gui::plugins
{
/// \brief Private data class for MarkerManager
class MarkerManager::Implementation
{
  /// \brief Update markers based on msgs received
  public: void OnRender();

  /// \brief Initialize services and subcriptions
  public: void Initialize();

  /// \brief Processes a marker message.
  /// \param[in] _msg The message data.
  /// \return True if the marker was processed successfully.
  public: bool ProcessMarkerMsg(const gz::msgs::Marker &_msg);

  /// \brief Services callback that returns a list of markers.
  /// \param[out] _rep Service reply
  /// \return True on success.
  public: bool OnList(gz::msgs::Marker_V &_rep);

  /// \brief Callback that receives marker messages.
  /// \param[in] _req The marker message.
  public: void OnMarkerMsg(const gz::msgs::Marker &_req);

  /// \brief Callback that receives multiple marker messages.
  /// \param[in] _req The vector of marker messages
  /// \param[in] _res Response data
  /// \return True if the request is received
  public: bool OnMarkerMsgArray(const gz::msgs::Marker_V &_req,
              gz::msgs::Boolean &_res);

  /// \brief Subscriber callback when new world statistics are received
  public: void OnWorldStatsMsg(const gz::msgs::WorldStatistics &_msg);

  /// \brief Sets Visual from marker message.
  /// \param[in] _msg The message data.
  /// \param[out] _visualPtr The visual pointer to set.
  public: void SetVisual(const gz::msgs::Marker &_msg,
                         const rendering::VisualPtr &_visualPtr);

  /// \brief Sets Marker from marker message.
  /// \param[in] _msg The message data.
  /// \param[out] _markerPtr The message pointer to set.
  public: void SetMarker(const gz::msgs::Marker &_msg,
                         const rendering::MarkerPtr &_markerPtr);

  /// \brief Converts a Gazebo msg material to Gazebo Rendering
  //         material.
  //  \param[in] _msg The message data.
  //  \return Converted rendering material, if any.
  public: rendering::MaterialPtr MsgToMaterial(
    const gz::msgs::Marker &_msg);

  /// \brief Converts a Gazebo msg render type to Gazebo Rendering
  /// \param[in] _msg The message data
  /// \return Converted rendering type, if any.
  public: gz::rendering::MarkerType MsgToType(
                    const gz::msgs::Marker &_msg);

  //// \brief Pointer to the rendering scene
  public: rendering::ScenePtr scene{nullptr};

  /// \brief Mutex to protect message list.
  public: std::mutex mutex;

  /// \brief List of marker message to process.
  public: std::list<gz::msgs::Marker> markerMsgs;

  /// \brief Map of visuals
  public: std::map<std::string,
      std::map<uint64_t, gz::rendering::VisualPtr>> visuals;

  /// \brief Gazebo node
  public: gz::transport::Node node {gz::transport::NodeOptions()};

  /// \brief Topic name for the marker service
  public: std::string topicName = "/marker";

  /// \brief Sim time according to world stats message
  public: std::chrono::steady_clock::duration simTime;

  /// \brief Previous sim time received
  public: std::chrono::steady_clock::duration lastSimTime;

  /// \brief The last marker message received
  public: gz::msgs::Marker msg;

  /// \brief True to print console warnings if the user tries to perform an
  /// action with an inexistent marker.
  public: bool warnOnActionFailure{true};
};

/////////////////////////////////////////////////
void MarkerManager::Implementation::Initialize()
{
  if (!this->scene)
  {
    gzerr << "Scene pointer is invalid" << std::endl;
    return;
  }

  if (this->topicName.empty())
  {
    gzerr << "Unable to advertise marker service. Topic name empty."
           << std::endl;
    return;
  }

  // Advertise the list service
  if (!this->node.Advertise(this->topicName + "/list",
      &Implementation::OnList, this))
  {
    gzerr << "Unable to advertise to the " << this->topicName
           << "/list service.\n";
  }

  gzdbg << "Advertise " << this->topicName << "/list service.\n";

  // Advertise to the marker service
  if (!this->node.Advertise(this->topicName,
        &Implementation::OnMarkerMsg, this))
  {
    gzerr << "Unable to advertise to the " << this->topicName
           << " service.\n";
  }

  gzdbg << "Advertise " << this->topicName << "/list.\n";

  // Advertise to the marker_array service
  if (!this->node.Advertise(this->topicName + "_array",
        &Implementation::OnMarkerMsgArray, this))
  {
    gzerr << "Unable to advertise to the " << this->topicName
           << "_array service.\n";
  }

  gzdbg << "Advertise " << this->topicName << "_array.\n";
}

/////////////////////////////////////////////////
void MarkerManager::Implementation::OnRender()
{
  if (!this->scene)
  {
    this->scene = rendering::sceneFromFirstRenderEngine();
    if (nullptr == this->scene)
      return;

    this->Initialize();
  }

  std::lock_guard<std::mutex> lock(this->mutex);
  // Process the marker messages.
  for (auto markerIter = this->markerMsgs.begin();
       markerIter != this->markerMsgs.end();)
  {
    this->ProcessMarkerMsg(*markerIter);
    this->markerMsgs.erase(markerIter++);
  }

  // Erase any markers that have a lifetime.
  for (auto mit = this->visuals.begin();
       mit != this->visuals.end();)
  {
    for (auto it = mit->second.cbegin();
         it != mit->second.cend(); ++it)
    {
      if (it->second->GeometryCount() == 0u)
        continue;

      gz::rendering::MarkerPtr markerPtr =
            std::dynamic_pointer_cast<gz::rendering::Marker>
            (it->second->GeometryByIndex(0u));
      if (markerPtr != nullptr)
      {
        if (markerPtr->Lifetime().count() != 0 &&
            (markerPtr->Lifetime() <= this->simTime ||
            this->simTime < this->lastSimTime))
        {
          this->scene->DestroyVisual(it->second);
          it = mit->second.erase(it);
          break;
        }
      }
    }

    // Erase a namespace if it's empty
    if (mit->second.empty())
      mit = this->visuals.erase(mit);
    else
      ++mit;
  }
  this->lastSimTime = this->simTime;
}

/////////////////////////////////////////////////
bool MarkerManager::Implementation::OnList(gz::msgs::Marker_V &_rep)
{
  std::lock_guard<std::mutex> lock(this->mutex);
  _rep.clear_marker();

  // Create the list of visuals
  for (const auto &mIter : this->visuals)
  {
    for (const auto &iter : mIter.second)
    {
      gz::msgs::Marker *markerMsg = _rep.add_marker();
      markerMsg->set_ns(mIter.first);
      markerMsg->set_id(iter.first);
    }
  }

  return true;
}

/////////////////////////////////////////////////
void MarkerManager::Implementation::OnMarkerMsg(const gz::msgs::Marker &_req)
{
  std::lock_guard<std::mutex> lock(this->mutex);
  this->markerMsgs.push_back(_req);
}

/////////////////////////////////////////////////
bool MarkerManager::Implementation::OnMarkerMsgArray(
    const gz::msgs::Marker_V&_req, gz::msgs::Boolean &_res)
{
  std::lock_guard<std::mutex> lock(this->mutex);
  std::copy(_req.marker().begin(), _req.marker().end(),
            std::back_inserter(this->markerMsgs));
  _res.set_data(true);
  return true;
}

//////////////////////////////////////////////////
bool MarkerManager::Implementation::ProcessMarkerMsg(
    const gz::msgs::Marker &_msg)
{
  // Get the namespace, if it exists. Otherwise, use the global namespace
  std::string ns;
  if (!_msg.ns().empty())
  {
    ns = _msg.ns();
  }

  // Get the namespace that the marker belongs to
  auto nsIter = this->visuals.find(ns);

  // If an id is given
  size_t id;
  if (_msg.id() != 0)
  {
    id = _msg.id();
  }
  // Otherwise generate unique id
  else
  {
    id = gz::math::Rand::IntUniform(0, gz::math::MAX_I32);

    // Make sure it's unique if namespace is given
    if (nsIter != this->visuals.end())
    {
      while (nsIter->second.find(id) != nsIter->second.end())
        id = gz::math::Rand::IntUniform(gz::math::MIN_UI32,
                                        gz::math::MAX_UI32);
    }
  }

  // Get visual for this namespace and id
  std::map<uint64_t, rendering::VisualPtr>::iterator visualIter;
  if (nsIter != this->visuals.end())
    visualIter = nsIter->second.find(id);

  // Add/modify a marker
  if (_msg.action() == gz::msgs::Marker::ADD_MODIFY)
  {
    // Modify an existing marker, identified by namespace and id
    if (nsIter != this->visuals.end() &&
        visualIter != nsIter->second.end())
    {
      if (visualIter->second->GeometryCount() > 0u)
      {
        // TODO(anyone): Update so that multiple markers can
        //               be attached to one visual
        gz::rendering::MarkerPtr markerPtr =
              std::dynamic_pointer_cast<gz::rendering::Marker>
              (visualIter->second->GeometryByIndex(0));

        visualIter->second->RemoveGeometryByIndex(0);

        // Set the visual values from the Marker Message
        this->SetVisual(_msg, visualIter->second);

        // Set the marker values from the Marker Message
        this->SetMarker(_msg, markerPtr);

        visualIter->second->AddGeometry(markerPtr);
      }
    }
    // Otherwise create a new marker
    else
    {
      // Create the name for the marker
      std::string name = "__GZ_MARKER_VISUAL_" + ns + "_" +
                         std::to_string(id);

      // Create the new marker
      rendering::VisualPtr visualPtr = this->scene->CreateVisual(name);

      // Create and load the marker
      rendering::MarkerPtr markerPtr = this->scene->CreateMarker();

      // Set the visual values from the Marker Message
      this->SetVisual(_msg, visualPtr);

      // Set the marker values from the Marker Message
      this->SetMarker(_msg, markerPtr);

      // Add populated marker to the visual
      visualPtr->AddGeometry(markerPtr);

      // Add visual to root visual
      if (!visualPtr->HasParent())
      {
        this->scene->RootVisual()->AddChild(visualPtr);
      }

      // Store the visual
      this->visuals[ns][id] = visualPtr;
    }
  }
  // Remove a single marker
  else if (_msg.action() == gz::msgs::Marker::DELETE_MARKER)
  {
    // Remove the marker if it can be found.
    if (nsIter != this->visuals.end() &&
        visualIter != nsIter->second.end())
    {
      this->scene->DestroyVisual(visualIter->second);
      this->visuals[ns].erase(visualIter);

      // Remove namespace if empty
      if (this->visuals[ns].empty())
        this->visuals.erase(nsIter);
    }
    else
    {
      if (this->warnOnActionFailure)
      {
        gzwarn << "Unable to delete marker with id[" << id << "] "
                << "in namespace[" << ns << "]" << std::endl;
      }
      return false;
    }
  }
  // Remove all markers, or all markers in a namespace
  else if (_msg.action() == gz::msgs::Marker::DELETE_ALL)
  {
    // If given namespace doesn't exist
    if (!ns.empty() && nsIter == this->visuals.end())
    {
      if (this->warnOnActionFailure)
      {
        gzwarn << "Unable to delete all markers in namespace[" << ns
                << "], namespace can't be found." << std::endl;
      }
      return false;
    }
    // Remove all markers in the specified namespace
    else if (nsIter != this->visuals.end())
    {
      for (const auto &it : nsIter->second)
      {
        this->scene->DestroyVisual(it.second);
      }
      nsIter->second.clear();
      this->visuals.erase(nsIter);
    }
    // Remove all markers in all namespaces.
    else
    {
      for (nsIter = this->visuals.begin();
           nsIter != this->visuals.end(); ++nsIter)
      {
        for (const auto &it : nsIter->second)
        {
          this->scene->DestroyVisual(it.second);
        }
      }
      this->visuals.clear();
    }
  }
  else
  {
    gzerr << "Unknown marker action[" << _msg.action() << "]\n";
    return false;
  }

  return true;
}

/////////////////////////////////////////////////
void MarkerManager::Implementation::SetVisual(const gz::msgs::Marker &_msg,
                           const rendering::VisualPtr &_visualPtr)
{
  // Set Visual Scale
  // The scale for points is used as the size of each point, so skip it here.
  if (_msg.has_scale() && _msg.type() != gz::msgs::Marker::POINTS)
  {
    _visualPtr->SetLocalScale(_msg.scale().x(),
                              _msg.scale().y(),
                              _msg.scale().z());
  }

  // Set Visual Pose
  if (_msg.has_pose())
  {
    math::Pose3d pose(_msg.pose().position().x(),
                      _msg.pose().position().y(),
                      _msg.pose().position().z(),
                      _msg.pose().orientation().w(),
                      _msg.pose().orientation().x(),
                      _msg.pose().orientation().y(),
                      _msg.pose().orientation().z());
    pose.Correct();
    _visualPtr->SetLocalPose(pose);
  }

  // Set Visual Parent
  if (!_msg.parent().empty())
  {
    if (_visualPtr->HasParent())
    {
      _visualPtr->Parent()->RemoveChild(_visualPtr);
    }

    rendering::VisualPtr parent = this->scene->VisualByName(_msg.parent());

    if (parent)
    {
      parent->AddChild(_visualPtr);
    }
    else
    {
      gzerr << "No visual with the name[" << _msg.parent() << "]\n";
    }
  }

  // todo(anyone) Update Marker Visibility
}

/////////////////////////////////////////////////
void MarkerManager::Implementation::SetMarker(const gz::msgs::Marker &_msg,
                           const rendering::MarkerPtr &_markerPtr)
{
  _markerPtr->SetLayer(_msg.layer());

  // Set Marker Lifetime
  std::chrono::steady_clock::duration lifetime =
    std::chrono::seconds(_msg.lifetime().sec()) +
    std::chrono::nanoseconds(_msg.lifetime().nsec());

  if (lifetime.count() != 0)
  {
    _markerPtr->SetLifetime(lifetime + this->simTime);
  }
  else
  {
    _markerPtr->SetLifetime(std::chrono::seconds(0));
  }
  // Set Marker Render Type
  gz::rendering::MarkerType markerType = MsgToType(_msg);
  _markerPtr->SetType(markerType);

  // Set Marker Material
  if (_msg.has_material())
  {
    rendering::MaterialPtr materialPtr = MsgToMaterial(_msg);
    _markerPtr->SetMaterial(materialPtr, true /* clone */);

    // clean up material after clone
    this->scene->DestroyMaterial(materialPtr);
  }

  // Assume the presence of points means we clear old ones
  if (_msg.point().size() > 0)
  {
    _markerPtr->ClearPoints();
  }

  // Set Marker Points
  for (int i = 0; i < _msg.point().size(); ++i)
  {
    math::Vector3d vector(
        _msg.point(i).x(),
        _msg.point(i).y(),
        _msg.point(i).z());

    math::Color color = msgs::Convert(_msg.material().diffuse());
    if (i < _msg.materials().size())
    {
      color = msgs::Convert(_msg.materials(i).diffuse());
    }
    _markerPtr->AddPoint(vector, color);
  }
  if (_msg.has_scale())
  {
    _markerPtr->SetSize(_msg.scale().x());
  }
}

/////////////////////////////////////////////////
rendering::MaterialPtr
MarkerManager::Implementation::MsgToMaterial(const gz::msgs::Marker &_msg)
{
  rendering::MaterialPtr material = this->scene->CreateMaterial();

  material->SetAmbient(
      _msg.material().ambient().r(),
      _msg.material().ambient().g(),
      _msg.material().ambient().b(),
      _msg.material().ambient().a());

  material->SetDiffuse(
      _msg.material().diffuse().r(),
      _msg.material().diffuse().g(),
      _msg.material().diffuse().b(),
      _msg.material().diffuse().a());

  material->SetSpecular(
      _msg.material().specular().r(),
      _msg.material().specular().g(),
      _msg.material().specular().b(),
      _msg.material().specular().a());

  material->SetEmissive(
      _msg.material().emissive().r(),
      _msg.material().emissive().g(),
      _msg.material().emissive().b(),
      _msg.material().emissive().a());

  material->SetLightingEnabled(_msg.material().lighting());

  return material;
}

/////////////////////////////////////////////////
gz::rendering::MarkerType
MarkerManager::Implementation::MsgToType(const gz::msgs::Marker &_msg)
{
  gz::msgs::Marker_Type marker = this->msg.type();
  if (marker != _msg.type() && _msg.type() != gz::msgs::Marker::NONE)
  {
    marker = _msg.type();
    this->msg.set_type(_msg.type());
  }
  switch (marker)
  {
    case gz::msgs::Marker::BOX:
      return gz::rendering::MarkerType::MT_BOX;
    case gz::msgs::Marker::CAPSULE:
      return gz::rendering::MarkerType::MT_CAPSULE;
    case gz::msgs::Marker::CYLINDER:
      return gz::rendering::MarkerType::MT_CYLINDER;
    case gz::msgs::Marker::LINE_STRIP:
      return gz::rendering::MarkerType::MT_LINE_STRIP;
    case gz::msgs::Marker::LINE_LIST:
      return gz::rendering::MarkerType::MT_LINE_LIST;
    case gz::msgs::Marker::POINTS:
      return gz::rendering::MarkerType::MT_POINTS;
    case gz::msgs::Marker::SPHERE:
      return gz::rendering::MarkerType::MT_SPHERE;
    case gz::msgs::Marker::TEXT:
      return gz::rendering::MarkerType::MT_TEXT;
    case gz::msgs::Marker::TRIANGLE_FAN:
      return gz::rendering::MarkerType::MT_TRIANGLE_FAN;
    case gz::msgs::Marker::TRIANGLE_LIST:
      return gz::rendering::MarkerType::MT_TRIANGLE_LIST;
    case gz::msgs::Marker::TRIANGLE_STRIP:
      return gz::rendering::MarkerType::MT_TRIANGLE_STRIP;
    default:
      gzerr << "Unable to create marker of type[" << _msg.type() << "]\n";
      break;
  }
  return gz::rendering::MarkerType::MT_NONE;
}

/////////////////////////////////////////////////
void MarkerManager::Implementation::OnWorldStatsMsg(
  const gz::msgs::WorldStatistics &_msg)
{
  std::lock_guard<std::mutex> lock(this->mutex);
  std::chrono::steady_clock::duration timePoint;
  if (_msg.has_sim_time())
  {
    timePoint = math::secNsecToDuration(
        _msg.sim_time().sec(),
        _msg.sim_time().nsec());
    this->simTime = timePoint;
  }
  else if (_msg.has_real_time())
  {
    timePoint = math::secNsecToDuration(
        _msg.real_time().sec(),
        _msg.real_time().nsec());
    this->simTime = timePoint;
  }
}

/////////////////////////////////////////////////
MarkerManager::MarkerManager()
  : dataPtr(gz::utils::MakeUniqueImpl<Implementation>())
{
}

/////////////////////////////////////////////////
MarkerManager::~MarkerManager() = default;

/////////////////////////////////////////////////
void MarkerManager::LoadConfig(const tinyxml2::XMLElement * _pluginElem)
{
  if (this->title.empty())
    this->title = "Marker Manager";

  // Custom parameters
  std::string statsTopic;
  if (_pluginElem)
  {
    auto elem = _pluginElem->FirstChildElement("topic_name");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      std::string topic = transport::TopicUtils::AsValidTopic(elem->GetText());
      if (!topic.empty())
      {
        this->dataPtr->topicName = topic;
      }
      else
      {
        gzerr << "the provided topic is no allowed. Using default ["
               << this->dataPtr->topicName << "]"<<  std::endl;
      }
    }

    if ((elem = _pluginElem->FirstChildElement("warn_on_action_failure")))
    {
      if (elem->QueryBoolText(&this->dataPtr->warnOnActionFailure) !=
          tinyxml2::XML_SUCCESS)
      {
        gzerr << "Faild to parse <warn_on_action_failure> value: "
               << elem->GetText() << std::endl;
      }
    }

    // Stats topic
    auto statsTopicElem = _pluginElem->FirstChildElement("stats_topic");
    if (nullptr != statsTopicElem && nullptr != statsTopicElem->GetText())
      statsTopic = statsTopicElem->GetText();
  }

  // World name from window, to construct default topics and services
  std::string worldName = "example";
  auto worldNames = gui::worldNames();
  if (!worldNames.empty())
    worldName = worldNames[0].toStdString();

  // Subscribe to world stats
  // Service specified with different world name
  auto parts = common::Split(statsTopic, '/');
  if (!worldName.empty() &&
      parts.size() == 4 &&
      parts[0] == "" &&
      parts[1] == "world" &&
      parts[2] != worldName &&
      parts[3] == "stats")
  {
    gzwarn << "Ignoring topic [" << statsTopic
            << "], world name different from [" << worldName
            << "]. Fix or remove your <stats_topic> tag." << std::endl;

    statsTopic = "/world/" + worldName + "/stats";
  }

  if (statsTopic.empty() && !worldName.empty())
  {
    statsTopic = "/world/" + worldName + "/stats";
  }

  statsTopic = transport::TopicUtils::AsValidTopic(statsTopic);
  if (!statsTopic.empty())
  {
    // Subscribe to world_stats
    if (!this->dataPtr->node.Subscribe(statsTopic,
        &Implementation::OnWorldStatsMsg, this->dataPtr.get()))
    {
      gzerr << "Failed to subscribe to [" << statsTopic << "]" << std::endl;
    }
    else
    {
      gzmsg << "Listening to stats on [" << statsTopic << "]" << std::endl;
    }
  }
  else
  {
    gzerr << "Failed to create valid topic for world [" << worldName << "]"
           << std::endl;
  }

  QQmlProperty::write(this->PluginItem(), "topicName",
      QString::fromStdString(this->dataPtr->topicName));
  QQmlProperty::write(this->PluginItem(), "statsTopic",
      QString::fromStdString(statsTopic));

  App()->findChild<MainWindow *>()->installEventFilter(this);
}

/////////////////////////////////////////////////
bool MarkerManager::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == events::Render::kType)
  {
    this->dataPtr->OnRender();
  }
  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}
}  // namespace gz::gui::plugins

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::MarkerManager,
              gz::gui::Plugin)
