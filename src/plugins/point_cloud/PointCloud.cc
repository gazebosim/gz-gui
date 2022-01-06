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

/*
 * Development of this module has been funded by the Monterey Bay Aquarium
 * Research Institute (MBARI) and the David and Lucile Packard Foundation
 */

#include "ignition/msgs/pointcloud_packed.pb.h"

#include <string>
#include <utility>
#include <vector>

#include "PointCloud.hh"

#include <ignition/common/Console.hh>
#include <ignition/common/Profiler.hh>

#include <ignition/plugin/Register.hh>

#include <ignition/math/Color.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/math/Pose3.hh>

#include <ignition/msgs/PointCloudPackedUtils.hh>
#include <ignition/msgs/Utility.hh>

#include <ignition/transport/Node.hh>

#include <ignition/gui/Application.hh>
#include <ignition/gui/Conversions.hh>
#include <ignition/gui/GuiEvents.hh>
#include <ignition/gui/MainWindow.hh>

/// \brief Private data class for PointCloud
class ignition::gui::plugins::PointCloudPrivate
{
  /// \brief Transport node
  public: ignition::transport::Node node;

  /// \brief Name of topic for PointCloudPacked
  public: std::string pointCloudTopic{""};

  /// \brief Name of topic for FloatV
  public: std::string floatVTopic{""};

  /// \brief List of topics publishing PointCloudPacked.
  public: QStringList pointCloudTopicList;

  /// \brief List of topics publishing FloatV.
  public: QStringList floatVTopicList;

  /// \brief Protect variables changed from transport and the user
  public: std::recursive_mutex mutex;

  /// \brief Point cloud message containing location of data
  public: ignition::msgs::PointCloudPacked pointCloudMsg;

  /// \brief Message holding a float vector.
  public: ignition::msgs::Float_V floatVMsg;

  /// \brief Minimum value in latest float vector
  public: float minFloatV{std::numeric_limits<float>::max()};

  /// \brief Maximum value in latest float vector
  public: float maxFloatV{-std::numeric_limits<float>::max()};

  /// \brief Color for minimum value
  public: ignition::math::Color minColor{1.0f, 0.0f, 0.0f, 1.0f};

  /// \brief Color for maximum value
  public: ignition::math::Color maxColor{0.0f, 1.0f, 0.0f, 1.0f};

  /// \brief Size of each point
  public: float pointSize{20};

  /// \brief True if showing
  public: bool showing{true};
};

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
PointCloud::PointCloud()
  : ignition::gui::Plugin(),
    dataPtr(std::make_unique<PointCloudPrivate>())
{
}

/////////////////////////////////////////////////
PointCloud::~PointCloud()
{
  this->ClearMarkers();
}

/////////////////////////////////////////////////
void PointCloud::LoadConfig(const tinyxml2::XMLElement *)
{
  if (this->title.empty())
    this->title = "Point cloud";

  ignition::gui::App()->findChild<
    ignition::gui::MainWindow *>()->installEventFilter(this);
}

//////////////////////////////////////////////////
void PointCloud::OnPointCloudTopic(const QString &_pointCloudTopic)
{
  std::lock_guard<std::recursive_mutex>(this->dataPtr->mutex);
  // Unsubscribe from previous choice
  if (!this->dataPtr->pointCloudTopic.empty() &&
      !this->dataPtr->node.Unsubscribe(this->dataPtr->pointCloudTopic))
  {
    ignerr << "Unable to unsubscribe from topic ["
           << this->dataPtr->pointCloudTopic <<"]" <<std::endl;
  }

  // Clear visualization
  this->ClearMarkers();

  this->dataPtr->pointCloudTopic = _pointCloudTopic.toStdString();

  // Request service
  this->dataPtr->node.Request(this->dataPtr->pointCloudTopic,
      &PointCloud::OnPointCloudService, this);

  // Create new subscription
  if (!this->dataPtr->node.Subscribe(this->dataPtr->pointCloudTopic,
                            &PointCloud::OnPointCloud, this))
  {
    ignerr << "Unable to subscribe to topic ["
           << this->dataPtr->pointCloudTopic << "]\n";
    return;
  }
  ignmsg << "Subscribed to " << this->dataPtr->pointCloudTopic << std::endl;
}

//////////////////////////////////////////////////
void PointCloud::OnFloatVTopic(const QString &_floatVTopic)
{
  std::lock_guard<std::recursive_mutex>(this->dataPtr->mutex);
  // Unsubscribe from previous choice
  if (!this->dataPtr->floatVTopic.empty() &&
      !this->dataPtr->node.Unsubscribe(this->dataPtr->floatVTopic))
  {
    ignerr << "Unable to unsubscribe from topic ["
           << this->dataPtr->floatVTopic <<"]" <<std::endl;
  }

  // Clear visualization
  this->ClearMarkers();

  this->dataPtr->floatVTopic = _floatVTopic.toStdString();

  // Request service
  this->dataPtr->node.Request(this->dataPtr->floatVTopic,
      &PointCloud::OnFloatVService, this);

  // Create new subscription
  if (!this->dataPtr->node.Subscribe(this->dataPtr->floatVTopic,
                            &PointCloud::OnFloatV, this))
  {
    ignerr << "Unable to subscribe to topic ["
           << this->dataPtr->floatVTopic << "]\n";
    return;
  }
  ignmsg << "Subscribed to " << this->dataPtr->floatVTopic << std::endl;
}

//////////////////////////////////////////////////
void PointCloud::Show(bool _show)
{
  this->dataPtr->showing = _show;
  if (_show)
  {
    this->PublishMarkers();
  }
  else
  {
    this->ClearMarkers();
  }
}

/////////////////////////////////////////////////
void PointCloud::OnRefresh()
{
  std::lock_guard<std::recursive_mutex>(this->dataPtr->mutex);
  ignmsg << "Refreshing topic list for point cloud messages." << std::endl;

  // Clear
  this->dataPtr->pointCloudTopicList.clear();
  this->dataPtr->floatVTopicList.clear();

  // Get updated list
  std::vector<std::string> allTopics;
  this->dataPtr->node.TopicList(allTopics);
  for (auto topic : allTopics)
  {
    std::vector<ignition::transport::MessagePublisher> publishers;
    this->dataPtr->node.TopicInfo(topic, publishers);
    for (auto pub : publishers)
    {
      if (pub.MsgTypeName() == "ignition.msgs.PointCloudPacked")
      {
        this->dataPtr->pointCloudTopicList.push_back(
            QString::fromStdString(topic));
      }
      else if (pub.MsgTypeName() == "ignition.msgs.Float_V")
      {
        this->dataPtr->floatVTopicList.push_back(QString::fromStdString(topic));
      }
    }
  }
  // Handle floats first, so by the time we get the point cloud it can be colored
  if (this->dataPtr->floatVTopicList.size() > 0)
  {
    this->OnFloatVTopic(this->dataPtr->floatVTopicList.at(0));
  }
  if (this->dataPtr->pointCloudTopicList.size() > 0)
  {
    this->OnPointCloudTopic(this->dataPtr->pointCloudTopicList.at(0));
  }

  this->PointCloudTopicListChanged();
  this->FloatVTopicListChanged();
}

/////////////////////////////////////////////////
QStringList PointCloud::PointCloudTopicList() const
{
  return this->dataPtr->pointCloudTopicList;
}

/////////////////////////////////////////////////
void PointCloud::SetPointCloudTopicList(
    const QStringList &_pointCloudTopicList)
{
  this->dataPtr->pointCloudTopicList = _pointCloudTopicList;
  this->PointCloudTopicListChanged();
}

/////////////////////////////////////////////////
QStringList PointCloud::FloatVTopicList() const
{
  return this->dataPtr->floatVTopicList;
}

/////////////////////////////////////////////////
void PointCloud::SetFloatVTopicList(
    const QStringList &_floatVTopicList)
{
  this->dataPtr->floatVTopicList = _floatVTopicList;
  this->FloatVTopicListChanged();
}

//////////////////////////////////////////////////
void PointCloud::OnPointCloud(
    const ignition::msgs::PointCloudPacked &_msg)
{
  std::lock_guard<std::recursive_mutex>(this->dataPtr->mutex);
  this->dataPtr->pointCloudMsg = _msg;
  this->PublishMarkers();
}

//////////////////////////////////////////////////
void PointCloud::OnFloatV(const ignition::msgs::Float_V &_msg)
{
  std::lock_guard<std::recursive_mutex>(this->dataPtr->mutex);
  this->dataPtr->floatVMsg = _msg;

  this->dataPtr->minFloatV = std::numeric_limits<float>::max();
  this->dataPtr->maxFloatV = -std::numeric_limits<float>::max();

  for (auto i = 0; i < _msg.data_size(); ++i)
  {
    auto data = _msg.data(i);
    if (data < this->dataPtr->minFloatV)
      this->SetMinFloatV(data);
    if (data > this->dataPtr->maxFloatV)
      this->SetMaxFloatV(data);
  }

  // TODO(chapulina) Publishing whenever we get a new point cloud and a new
  // floatV is good in case these topics are out of sync. But here they're
  // synchronized, so in practice we're publishing markers twice for each
  // PC+float that we get.
  this->PublishMarkers();
}

//////////////////////////////////////////////////
void PointCloud::OnPointCloudService(
    const ignition::msgs::PointCloudPacked &_msg, bool _result)
{
  if (!_result)
  {
    ignerr << "Service request failed." << std::endl;
    return;
  }
  this->OnPointCloud(_msg);
}

//////////////////////////////////////////////////
void PointCloud::OnFloatVService(
    const ignition::msgs::Float_V &_msg, bool _result)
{
  if (!_result)
  {
    ignerr << "Service request failed." << std::endl;
    return;
  }
  this->OnFloatV(_msg);
}

//////////////////////////////////////////////////
void PointCloud::PublishMarkers()
{
  IGN_PROFILE("PointCloud::PublishMarkers");

  if (!this->dataPtr->showing)
    return;

  // If point cloud empty, do nothing.
  if (this->dataPtr->pointCloudMsg.height() == 0 &&
      this->dataPtr->pointCloudMsg.width() == 0)
  {
    return;
  }

  std::lock_guard<std::recursive_mutex>(this->dataPtr->mutex);
  ignition::msgs::Marker marker;
  marker.set_ns(this->dataPtr->pointCloudTopic + this->dataPtr->floatVTopic);
  marker.set_id(1);
  marker.set_action(ignition::msgs::Marker::ADD_MODIFY);
  marker.set_type(ignition::msgs::Marker::POINTS);
  marker.set_visibility(ignition::msgs::Marker::GUI);

  ignition::msgs::Set(marker.mutable_scale(),
    ignition::math::Vector3d::One * this->dataPtr->pointSize);

  ignition::msgs::PointCloudPackedIterator<float>
      iterX(this->dataPtr->pointCloudMsg, "x");
  ignition::msgs::PointCloudPackedIterator<float>
      iterY(this->dataPtr->pointCloudMsg, "y");
  ignition::msgs::PointCloudPackedIterator<float>
      iterZ(this->dataPtr->pointCloudMsg, "z");

  // Index of point in point cloud, visualized or not
  int ptIdx{0};
  auto minC = this->dataPtr->minColor;
  auto maxC = this->dataPtr->maxColor;
  auto floatRange = this->dataPtr->maxFloatV - this->dataPtr->minFloatV;
  for (;iterX != iterX.End() &&
        iterY != iterY.End() &&
        iterZ != iterZ.End(); ++iterX, ++iterY, ++iterZ, ++ptIdx)
  {
    // Value from float vector, if available. Otherwise publish all data as
    // zeroes.
    float dataVal = 0.0;
    if (this->dataPtr->floatVMsg.data().size() > ptIdx)
    {
      dataVal = this->dataPtr->floatVMsg.data(ptIdx);
    }

    // Don't visualize NaN
    if (std::isnan(dataVal))
      continue;

    auto ratio = floatRange > 0 ?
        (dataVal - this->dataPtr->minFloatV) / floatRange : 0.0f;
    ignition:: math::Color color{
      minC.R() + (maxC.R() - minC.R()) * ratio,
      minC.G() + (maxC.G() - minC.G()) * ratio,
      minC.B() + (maxC.B() - minC.B()) * ratio
    };

    ignition::msgs::Set(marker.add_materials()->mutable_diffuse(), color);

    ignition::msgs::Set(marker.add_point(), ignition::math::Vector3d(
      *iterX,
      *iterY,
      *iterZ));
  }

  igndbg << "Visualizing " << marker.point_size() << " points"
    << std::endl;

  this->dataPtr->node.Request("/marker", marker);
}

//////////////////////////////////////////////////
void PointCloud::ClearMarkers()
{
  if (this->dataPtr->pointCloudTopic.empty())
    return;

  std::lock_guard<std::recursive_mutex>(this->dataPtr->mutex);
  ignition::msgs::Marker msg;
  msg.set_ns(this->dataPtr->pointCloudTopic + this->dataPtr->floatVTopic);
  msg.set_id(0);
  msg.set_action(ignition::msgs::Marker::DELETE_ALL);

  igndbg << "Clearing markers on "
    << this->dataPtr->pointCloudTopic + this->dataPtr->floatVTopic
    << std::endl;

  this->dataPtr->node.Request("/marker", msg);
}

/////////////////////////////////////////////////
QColor PointCloud::MinColor() const
{
  return ignition::gui::convert(this->dataPtr->minColor);
}

/////////////////////////////////////////////////
void PointCloud::SetMinColor(const QColor &_minColor)
{
  this->dataPtr->minColor = ignition::gui::convert(_minColor);
  this->MinColorChanged();
  this->PublishMarkers();
}

/////////////////////////////////////////////////
QColor PointCloud::MaxColor() const
{
  return ignition::gui::convert(this->dataPtr->maxColor);
}

/////////////////////////////////////////////////
void PointCloud::SetMaxColor(const QColor &_maxColor)
{
  this->dataPtr->maxColor = ignition::gui::convert(_maxColor);
  this->MaxColorChanged();
  this->PublishMarkers();
}

/////////////////////////////////////////////////
float PointCloud::MinFloatV() const
{
  return this->dataPtr->minFloatV;
}

/////////////////////////////////////////////////
void PointCloud::SetMinFloatV(float _minFloatV)
{
  this->dataPtr->minFloatV = _minFloatV;
  this->MinFloatVChanged();
}

/////////////////////////////////////////////////
float PointCloud::MaxFloatV() const
{
  return this->dataPtr->maxFloatV;
}

/////////////////////////////////////////////////
void PointCloud::SetMaxFloatV(float _maxFloatV)
{
  this->dataPtr->maxFloatV = _maxFloatV;
  this->MaxFloatVChanged();
}

/////////////////////////////////////////////////
float PointCloud::PointSize() const
{
  return this->dataPtr->pointSize;
}

/////////////////////////////////////////////////
void PointCloud::SetPointSize(float _pointSize)
{
  this->dataPtr->pointSize = _pointSize;
  this->PointSizeChanged();
  this->PublishMarkers();
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::PointCloud,
                    ignition::gui::Plugin)
