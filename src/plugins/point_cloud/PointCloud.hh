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

/*
 * Development of this module has been funded by the Monterey Bay Aquarium
 * Research Institute (MBARI) and the David and Lucile Packard Foundation
 */

#ifndef IGNITION_GUI_PLUGINS_POINTCLOUD_HH_
#define IGNITION_GUI_PLUGINS_POINTCLOUD_HH_

#include <ignition/msgs/float_v.pb.h>
#include <ignition/msgs/pointcloud_packed.pb.h>
#include <ignition/utils/ImplPtr.hh>

#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  /// \brief Visualize PointCloudPacked messages in the 3D scene.
  /// By default, the whole cloud is displayed using a single color. Users
  /// can optionally choose a topic publishing FloatV messages which will be
  /// used to color all points with a color gradient according to their values.
  /// NaN values on the FloatV message aren't displayed.
  class PointCloud : public ignition::gui::Plugin
  {
    Q_OBJECT

    /// \brief List of topics publishing PointCloudPacked messages
    Q_PROPERTY(
      QStringList pointCloudTopicList
      READ PointCloudTopicList
      WRITE SetPointCloudTopicList
      NOTIFY PointCloudTopicListChanged
    )

    /// \brief List of topics publishing FloatV messages
    Q_PROPERTY(
      QStringList floatVTopicList
      READ FloatVTopicList
      WRITE SetFloatVTopicList
      NOTIFY FloatVTopicListChanged
    )

    /// \brief Color for minimum value
    Q_PROPERTY(
      QColor minColor
      READ MinColor
      WRITE SetMinColor
      NOTIFY MinColorChanged
    )

    /// \brief Color for maximum value
    Q_PROPERTY(
      QColor maxColor
      READ MaxColor
      WRITE SetMaxColor
      NOTIFY MaxColorChanged
    )

    /// \brief Minimum value
    Q_PROPERTY(
      float minFloatV
      READ MinFloatV
      WRITE SetMinFloatV
      NOTIFY MinFloatVChanged
    )

    /// \brief Maximum value
    Q_PROPERTY(
      float maxFloatV
      READ MaxFloatV
      WRITE SetMaxFloatV
      NOTIFY MaxFloatVChanged
    )

    /// \brief Point size
    Q_PROPERTY(
      float pointSize
      READ PointSize
      WRITE SetPointSize
      NOTIFY PointSizeChanged
    )

    /// \brief Constructor
    public: PointCloud();

    /// \brief Destructor
    public: ~PointCloud() override;

    // Documentation inherited
    public: void LoadConfig(const tinyxml2::XMLElement *_pluginElem) override;

    /// \brief Callback function for point cloud topic.
    /// \param[in] _msg Point cloud message
    public: void OnPointCloud(const ignition::msgs::PointCloudPacked &_msg);

    /// \brief Callback function for point cloud service
    /// \param[in] _msg Point cloud message
    /// \param[out] _result True on success.
    public: void OnPointCloudService(
        const ignition::msgs::PointCloudPacked &_msg, bool _result);

    /// \brief Get the topic list
    /// \return List of topics
    public: Q_INVOKABLE QStringList PointCloudTopicList() const;

    /// \brief Set the topic list from a string
    /// \param[in] _pointCloudTopicList List of topics.
    public: Q_INVOKABLE void SetPointCloudTopicList(
        const QStringList &_pointCloudTopicList);

    /// \brief Notify that topic list has changed
    signals: void PointCloudTopicListChanged();

    /// \brief Set topic to subscribe to for point cloud.
    /// \param[in] _topicName Name of selected topic
    public: Q_INVOKABLE void OnPointCloudTopic(const QString &_topicName);

    /// \brief Callback function for float vector topic.
    /// \param[in] _msg Float vector message
    public: void OnFloatV(const ignition::msgs::Float_V &_msg);

    /// \brief Callback function for point cloud service
    /// \param[in] _msg Float vector message
    /// \param[out] _result True on success.
    public: void OnFloatVService(
        const ignition::msgs::Float_V &_msg, bool _result);

    /// \brief Get the topic list
    /// \return List of topics
    public: Q_INVOKABLE QStringList FloatVTopicList() const;

    /// \brief Set the topic list from a string
    /// \param[in] _floatVTopicList List of topics.
    public: Q_INVOKABLE void SetFloatVTopicList(
        const QStringList &_floatVTopicList);

    /// \brief Notify that topic list has changed
    signals: void FloatVTopicListChanged();

    /// \brief Set topic to subscribe to for float vectors.
    /// \param[in] _topicName Name of selected topic
    public: Q_INVOKABLE void OnFloatVTopic(const QString &_topicName);

    /// \brief Get the minimum color
    /// \return Minimum color
    public: Q_INVOKABLE QColor MinColor() const;

    /// \brief Set the minimum color
    /// \param[in] _minColor Minimum color.
    public: Q_INVOKABLE void SetMinColor(const QColor &_minColor);

    /// \brief Notify that minimum color has changed
    signals: void MinColorChanged();

    /// \brief Get the maximum color
    /// \return Maximum color
    public: Q_INVOKABLE QColor MaxColor() const;

    /// \brief Set the maximum color
    /// \param[ax] _maxColor Maximum color.
    public: Q_INVOKABLE void SetMaxColor(const QColor &_maxColor);

    /// \brief Notify that maximum color has changed
    signals: void MaxColorChanged();

    /// \brief Get the minimum value
    /// \return Minimum value
    public: Q_INVOKABLE float MinFloatV() const;

    /// \brief Set the minimum value
    /// \param[in] _minFloatV Minimum value.
    public: Q_INVOKABLE void SetMinFloatV(float _minFloatV);

    /// \brief Notify that minimum value has changed
    signals: void MinFloatVChanged();

    /// \brief Get the maximum value
    /// \return Maximum value
    public: Q_INVOKABLE float MaxFloatV() const;

    /// \brief Set the maximum value
    /// \param[ax] _maxFloatV Maximum value.
    public: Q_INVOKABLE void SetMaxFloatV(float _maxFloatV);

    /// \brief Notify that maximum value has changed
    signals: void MaxFloatVChanged();

    /// \brief Get the point size
    /// \return Maximum value
    public: Q_INVOKABLE float PointSize() const;

    /// \brief Set the point size
    /// \param[ax] _pointSize Maximum value.
    public: Q_INVOKABLE void SetPointSize(float _pointSize);

    /// \brief Notify that point size has changed
    signals: void PointSizeChanged();

    /// \brief Set whether to show the point cloud.
    /// \param[in] _show Boolean value for displaying the points.
    public: Q_INVOKABLE void Show(bool _show);

    /// \brief Callback when refresh button is pressed.
    public: Q_INVOKABLE void OnRefresh();

    /// \brief Makes a request to populate the scene with markers
    private: void PublishMarkers();

    /// \brief Makes a request to delete all markers related to the point cloud.
    private: void ClearMarkers();

    /// \internal
    /// \brief Pointer to private data
    IGN_UTILS_UNIQUE_IMPL_PTR(dataPtr)
  };
}
}
}
#endif
