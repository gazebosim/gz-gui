/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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

#include "gz/msgs/boolean.pb.h"
#include "gz/msgs/float_v.pb.h"
#include "gz/msgs/marker.pb.h"
#include "gz/msgs/pointcloud_packed.pb.h"

#include <algorithm>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <gz/common/Console.hh>
#include <gz/common/Profiler.hh>
#include <gz/math/Color.hh>
#include <gz/math/Pose3.hh>
#include <gz/math/Vector3.hh>
#include <gz/msgs/PointCloudPackedUtils.hh>
#include <gz/msgs/Utility.hh>
#include <gz/plugin/Register.hh>
#include <gz/transport/Node.hh>

#include <gz/gui/Application.hh>
#include <gz/gui/Conversions.hh>
#include <gz/gui/GuiEvents.hh>
#include <gz/gui/MainWindow.hh>

#include "PointCloud.hh"

namespace gz::gui::plugins
{
/// \brief Private data class for PointCloud
class PointCloudPrivate
{
  /// \brief Makes a request to populate the scene with markers
  public: void PublishMarkers();

  /// \brief Makes a request to delete all markers related to the point cloud.
  public: void ClearMarkers();

  /// \brief Transport node
  public: gz::transport::Node node;

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

  /// \brief Point cloud message containing XYZ positions
  public: gz::msgs::PointCloudPacked pointCloudMsg;

  /// \brief Message holding a float vector.
  public: gz::msgs::Float_V floatVMsg;

  /// \brief Minimum value in latest float vector
  public: float minFloatV{std::numeric_limits<float>::max()};

  /// \brief Maximum value in latest float vector
  public: float maxFloatV{-std::numeric_limits<float>::max()};

  /// \brief Color for minimum value, changeable at runtime
  public: gz::math::Color minColor{1.0f, 0.0f, 0.0f, 1.0f};

  /// \brief Color for maximum value, changeable at runtime
  public: gz::math::Color maxColor{0.0f, 1.0f, 0.0f, 1.0f};

  /// \brief Size of each point, changeable at runtime
  public: float pointSize{20};

  /// \brief True if showing, changeable at runtime
  public: bool showing{true};
};

/////////////////////////////////////////////////
PointCloud::PointCloud()
  : gz::gui::Plugin(),
    dataPtr(std::make_unique<PointCloudPrivate>())
{
}

/////////////////////////////////////////////////
PointCloud::~PointCloud()
{
  this->dataPtr->ClearMarkers();
}

/////////////////////////////////////////////////
void PointCloud::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Point cloud";

  // Parameters from XML
  if (_pluginElem)
  {
    auto pointCloudTopicElem =
        _pluginElem->FirstChildElement("point_cloud_topic");
    if (nullptr != pointCloudTopicElem &&
        nullptr != pointCloudTopicElem->GetText())
    {
      this->SetPointCloudTopicList({pointCloudTopicElem->GetText()});
      this->OnPointCloudTopic(this->dataPtr->pointCloudTopicList.at(0));
    }

    auto floatVTopicElem =
        _pluginElem->FirstChildElement("float_v_topic");
    if (nullptr != floatVTopicElem &&
        nullptr != floatVTopicElem->GetText())
    {
      this->SetFloatVTopicList({floatVTopicElem->GetText()});
      this->OnFloatVTopic(this->dataPtr->floatVTopicList.at(0));
    }

  }

  gz::gui::App()->findChild<
    gz::gui::MainWindow *>()->installEventFilter(this);
}

//////////////////////////////////////////////////
void PointCloud::OnPointCloudTopic(const QString &_pointCloudTopic)
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);
  // Unsubscribe from previous choice
  if (!this->dataPtr->pointCloudTopic.empty() &&
      !this->dataPtr->node.Unsubscribe(this->dataPtr->pointCloudTopic))
  {
    gzerr << "Unable to unsubscribe from topic ["
           << this->dataPtr->pointCloudTopic <<"]" <<std::endl;
  }

  // Clear visualization
  this->dataPtr->ClearMarkers();

  this->dataPtr->pointCloudTopic = _pointCloudTopic.toStdString();

  // Request service
  this->dataPtr->node.Request(this->dataPtr->pointCloudTopic,
      &PointCloud::OnPointCloudService, this);

  // Create new subscription
  if (!this->dataPtr->node.Subscribe(this->dataPtr->pointCloudTopic,
                            &PointCloud::OnPointCloud, this))
  {
    gzerr << "Unable to subscribe to topic ["
           << this->dataPtr->pointCloudTopic << "]\n";
    return;
  }
  gzmsg << "Subscribed to " << this->dataPtr->pointCloudTopic << std::endl;
}

//////////////////////////////////////////////////
void PointCloud::OnFloatVTopic(const QString &_floatVTopic)
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);
  // Unsubscribe from previous choice
  if (!this->dataPtr->floatVTopic.empty() &&
      !this->dataPtr->node.Unsubscribe(this->dataPtr->floatVTopic))
  {
    gzerr << "Unable to unsubscribe from topic ["
           << this->dataPtr->floatVTopic <<"]" <<std::endl;
  }

  // Clear visualization
  this->dataPtr->ClearMarkers();

  this->dataPtr->floatVTopic = _floatVTopic.toStdString();

  // Request service
  this->dataPtr->node.Request(this->dataPtr->floatVTopic,
      &PointCloud::OnFloatVService, this);

  // Create new subscription
  if (!this->dataPtr->node.Subscribe(this->dataPtr->floatVTopic,
                            &PointCloud::OnFloatV, this))
  {
    gzerr << "Unable to subscribe to topic ["
           << this->dataPtr->floatVTopic << "]\n";
    return;
  }
  gzmsg << "Subscribed to " << this->dataPtr->floatVTopic << std::endl;
}

//////////////////////////////////////////////////
void PointCloud::Show(bool _show)
{
  this->dataPtr->showing = _show;
  if (_show)
  {
    this->dataPtr->PublishMarkers();
  }
  else
  {
    this->dataPtr->ClearMarkers();
  }
}

/////////////////////////////////////////////////
void PointCloud::OnRefresh()
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);
  gzmsg << "Refreshing topic list for point cloud messages." << std::endl;

  // Clear
  this->dataPtr->pointCloudTopicList.clear();
  this->dataPtr->floatVTopicList.clear();

  // Get updated list
  std::vector<std::string> allTopics;
  this->dataPtr->node.TopicList(allTopics);
  for (auto topic : allTopics)
  {
    std::vector<gz::transport::MessagePublisher> publishers;
    std::vector<gz::transport::MessagePublisher> subscribers;
    this->dataPtr->node.TopicInfo(topic, publishers, subscribers);
    for (auto pub : publishers)
    {
      if (pub.MsgTypeName() == "gz.msgs.PointCloudPacked")
      {
        this->dataPtr->pointCloudTopicList.push_back(
            QString::fromStdString(topic));
      }
      else if (pub.MsgTypeName() == "gz.msgs.Float_V")
      {
        this->dataPtr->floatVTopicList.push_back(QString::fromStdString(topic));
      }
    }
  }
  // Handle floats first, so by the time we get the point cloud it can be
  // colored
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
    const gz::msgs::PointCloudPacked &_msg)
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);
  this->dataPtr->pointCloudMsg = _msg;
  this->dataPtr->PublishMarkers();
}

//////////////////////////////////////////////////
void PointCloud::OnFloatV(const gz::msgs::Float_V &_msg)
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);
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
  this->dataPtr->PublishMarkers();
}

//////////////////////////////////////////////////
void PointCloud::OnPointCloudService(
    const gz::msgs::PointCloudPacked &_msg, bool _result)
{
  if (!_result)
  {
    gzerr << "Service request failed." << std::endl;
    return;
  }
  this->OnPointCloud(_msg);
}

//////////////////////////////////////////////////
void PointCloud::OnFloatVService(
    const gz::msgs::Float_V &_msg, bool _result)
{
  if (!_result)
  {
    gzerr << "Service request failed." << std::endl;
    return;
  }
  this->OnFloatV(_msg);
}

//////////////////////////////////////////////////
void PointCloudPrivate::PublishMarkers()
{
  GZ_PROFILE("PointCloud::PublishMarkers");

  if (!this->showing)
    return;

  // If point cloud empty, do nothing.
  if (this->pointCloudMsg.height() == 0 &&
      this->pointCloudMsg.width() == 0)
  {
    return;
  }

  std::lock_guard<std::recursive_mutex> lock(this->mutex);
  gz::msgs::Marker marker;
  marker.set_ns(this->pointCloudTopic + this->floatVTopic);
  marker.set_id(1);
  marker.set_action(gz::msgs::Marker::ADD_MODIFY);
  marker.set_type(gz::msgs::Marker::POINTS);
  marker.set_visibility(gz::msgs::Marker::GUI);

  gz::msgs::Set(marker.mutable_scale(),
    gz::math::Vector3d::One * this->pointSize);

  gz::msgs::PointCloudPackedIterator<float>
      iterX(this->pointCloudMsg, "x");
  gz::msgs::PointCloudPackedIterator<float>
      iterY(this->pointCloudMsg, "y");
  gz::msgs::PointCloudPackedIterator<float>
      iterZ(this->pointCloudMsg, "z");

  // Index of point in point cloud, visualized or not
  int ptIdx{0};
  auto minC = this->minColor;
  auto maxC = this->maxColor;
  auto floatRange = this->maxFloatV - this->minFloatV;
  auto num_points =
    this->pointCloudMsg.data().size() / this->pointCloudMsg.point_step();
  if (static_cast<int>(num_points) != this->floatVMsg.data().size())
  {
    gzwarn << "Float message and pointcloud are not of the same size,"
      <<" visualization may not be accurate" << std::endl;
  }
  if (this->pointCloudMsg.data().size() % this->pointCloudMsg.point_step() != 0)
  {
    gzwarn << "Mal-formatted pointcloud" << std::endl;
  }

  for (; ptIdx < std::min<int>(this->floatVMsg.data().size(), num_points);
    ++iterX, ++iterY, ++iterZ, ++ptIdx)
  {
    // Value from float vector, if available. Otherwise publish all data as
    // zeroes.
    float dataVal = this->floatVMsg.data(ptIdx);

    // Don't visualize NaN
    if (std::isnan(dataVal))
      continue;

    auto ratio = floatRange > 0 ?
        (dataVal - this->minFloatV) / floatRange : 0.0f;
    gz:: math::Color color{
      minC.R() + (maxC.R() - minC.R()) * ratio,
      minC.G() + (maxC.G() - minC.G()) * ratio,
      minC.B() + (maxC.B() - minC.B()) * ratio
    };

    gz::msgs::Set(marker.add_materials()->mutable_diffuse(), color);
    gz::msgs::Set(marker.add_point(), gz::math::Vector3d(
      *iterX,
      *iterY,
      *iterZ));
  }

  this->node.Request("/marker", marker);
}

//////////////////////////////////////////////////
void PointCloudPrivate::ClearMarkers()
{
  if (this->pointCloudTopic.empty())
    return;

  std::lock_guard<std::recursive_mutex> lock(this->mutex);
  gz::msgs::Marker msg;
  msg.set_ns(this->pointCloudTopic + this->floatVTopic);
  msg.set_id(0);
  msg.set_action(gz::msgs::Marker::DELETE_ALL);

  gzdbg << "Clearing markers on "
    << this->pointCloudTopic + this->floatVTopic
    << std::endl;

  this->node.Request("/marker", msg);
}

/////////////////////////////////////////////////
QColor PointCloud::MinColor() const
{
  return gz::gui::convert(this->dataPtr->minColor);
}

/////////////////////////////////////////////////
void PointCloud::SetMinColor(const QColor &_minColor)
{
  this->dataPtr->minColor = gz::gui::convert(_minColor);
  this->MinColorChanged();
  this->dataPtr->PublishMarkers();
}

/////////////////////////////////////////////////
QColor PointCloud::MaxColor() const
{
  return gz::gui::convert(this->dataPtr->maxColor);
}

/////////////////////////////////////////////////
void PointCloud::SetMaxColor(const QColor &_maxColor)
{
  this->dataPtr->maxColor = gz::gui::convert(_maxColor);
  this->MaxColorChanged();
  this->dataPtr->PublishMarkers();
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
  this->dataPtr->PublishMarkers();
}
}  // namespace gz::gui::plugins

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::PointCloud,
                    gz::gui::Plugin)
