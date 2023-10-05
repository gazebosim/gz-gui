/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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
#ifndef GZ_GUI_PLOTTINGINTERFACE_HH_
#define GZ_GUI_PLOTTINGINTERFACE_HH_

#include <QObject>
#include <QString>
#include <QMap>
#include <QVariant>
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <map>
#include <set>
#include <string>
#include <memory>
#include <limits>

#include "gz/gui/Export.hh"

#include <gz/utils/ImplPtr.hh>

namespace gz::gui
{
/// \brief Plot Data containter to hold value and registered charts
/// Can be a Field or a PlotComponent
/// Used by PlottingInterface and Gazebo Plotting
class GZ_GUI_VISIBLE PlotData
{
  /// \brief Constructor
  public: PlotData();

  /// \brief Destructor
  public: ~PlotData();

  /// \brief Set the field Value
  /// \param[in] _value the set value
  public: void SetValue(const double _value);

  /// \brief Get the field value
  /// \return value of the field
  public: double Value() const;

  /// \brief Set the field arrival time
  /// \param[in] _time arrival time to set it
  public: void SetTime(const double _time);

  /// \brief Get the arrival time
  /// \return arrival time
  public: double Time() const;

  /// \brief Register a chart that plot that field
  /// \param[in] _chart chart ID to be registered
  public: void AddChart(int _chart);

  /// \brief UnRegister a chart from plotting that field
  public: void RemoveChart(int _chart);

  /// \brief Number of registered charts
  /// \return Charts count
  public: int ChartCount() const;

  /// \brief Get all registered charts to that field
  /// \return Set of registered charts
  public: std::set<int> &Charts();

  /// \brief Private data member.
  GZ_UTILS_UNIQUE_IMPL_PTR(dataPtr)
};

/// \brief Plotting Topic to handle published topics & their registered fields
class GZ_GUI_VISIBLE Topic : public QObject
{
  Q_OBJECT

  /// \brief Constructor
  public: explicit Topic(const std::string &_name);

  /// \brief Destructor
  public: ~Topic();

  /// \brief Get topic name
  /// \return Topic name
  public: std::string &Name() const;

  /// \brief Register a chart to a field
  /// \param[in] _fieldPath model path to the field as an ID
  /// \param[in] _chart Chart ID
  public: void Register(const std::string &_fieldPath, int _chart);

  /// \brief Remove field from the plot
  /// \param[in] _fieldPath model path to the field as an ID
  /// \param[in] _chart Chart ID
  public: void UnRegister(const std::string &_fieldPath, int _chart);

  /// \brief size of registered fields
  /// \return fields size
  public: int FieldCount() const;

  /// \brief Get the registered fields
  /// \return Map of fields to their plots
  public: std::map<std::string, PlotData *> &Fields();

  /// \brief Callback to receive messages
  /// \param[in] _msg the published msg from the topic
  public: void Callback(const google::protobuf::Message &_msg);

  /// \brief Check if msg has header field and get its time
  /// \param[in] _msg msg to check its header
  /// \param[out] _headerTime header sim time
  public: bool HasHeader(const google::protobuf::Message &_msg,
                         double &_headerTime);

  /// \brief update the plot
  /// \param[in] _field field path or ID
  public: void UpdateGui(const std::string &_field);

  /// \brief update the GUI and plot the topic's fields values
  /// \param[in] _chart chart ID
  /// \param[in] _fieldID field path ID
  /// \param[in] _x x coordinates of the plot point
  /// \param[in] _y y coordinates of the plot point
  signals: void plot(int _chart, QString _fieldID, double _x, double _y);

  /// \brief update the current time with the default time of the plotting timer
  /// \param[in] _time current time of the plotting timer
  public: void SetPlottingTimeRef(const std::shared_ptr<double> &_time);

  /// \brief Private data member.
  GZ_UTILS_UNIQUE_IMPL_PTR(dataPtr)
};

/// \brief Handle transport topics subscribing for one object (Chart)
class GZ_GUI_VISIBLE Transport : public QObject
{
  Q_OBJECT

  /// \brief Constructor
  public: Transport();

  /// \brief Destructor
  public: ~Transport();

  /// \brief Unsubscribe/deattatch a field from a certain chart
  /// \param[in] _topic topic name
  /// \param[in] _fieldPath field path ID
  /// \param[in] _chart chart ID
  public: void Unsubscribe(const std::string &_topic,
                           const std::string &_fieldPath,
                           int _chart);

  /// \brief Subscribe/attatch a field from a certain chart
  /// \param[in] _topic topic name
  /// \param[in] _fieldPath field path ID
  /// \param[in] _chart chart ID
  /// \param[in] _time ref to current plotting time
  public: void Subscribe(const std::string &_topic,
                         const std::string &_fieldPath,
                         int _chart, const std::shared_ptr<double> &_time);

  /// \brief Unsubscribe from non-exist topics in the transport
  public slots: void UnsubscribeOutdatedTopics();

  /// \brief Get the registered topics
  /// \return Topics list
  public: const std::map<std::string, Topic*> &Topics();

  /// \brief Slot for receiving topics signal at each topic callback to plot
  /// \param[in] _chart chart ID
  /// \param[in] _fieldID field path ID
  /// \param[in] _x x coordinates of the plot point
  /// \param[in] _y y coordinates of the plot point
  public slots: void onPlot(int _chart, QString _fieldID, double _x, double _y);

  /// \brief notify the Plotting Interface to plot
  /// \param[in] _chart chart ID
  /// \param[in] _fieldID field path ID
  /// \param[in] _x x coordinates of the plot point
  /// \param[in] _y y coordinates of the plot point
  signals: void plot(int _chart, QString _fieldID, double _x, double _y);

  private:
  /// \brief Private data member.
  GZ_UTILS_UNIQUE_IMPL_PTR(dataPtr)
};

/// \brief Plotting Interface
/// Responsible for plotting transport msgs-fields
/// Used by TransportPlotting Plugin & GazeboPlotting Plugin
/// Accepts dragged items from TopicViewer Plugin & ComponentInspector Plugin
class GZ_GUI_VISIBLE PlottingInterface : public QObject
{
  Q_OBJECT

  /// \brief Constructor
  public: explicit PlottingInterface();

  /// \brief Destructor
  public: ~PlottingInterface();

  /// \brief subscribe to a field to plotted on a chart
  /// \brief param[in] _topic the topic that includes that field
  /// \brief param[in] _fieldPath path to the field to reach it from the msg
  /// \brief param[in] _chart chart id to be attached to that field
  public slots: void subscribe(int _chart,
                               QString _fieldPath,
                               QString _topic);

  /// \brief unsubscribe from a field and deattach it from a chart
  /// \brief param[in] _topic the topic that includes that field
  /// \brief param[in] _fieldPath path to the field to reach it from the msg
  /// \brief param[in] _chart chart id to be deattached to that field
  public slots: void unsubscribe(int _chart,
                                 QString _fieldPath,
                                 QString _topic);

  /// \brief Get the timeout of updating the plot
  /// \return updating plot timeout
  public: float Timeout() const;

  /// \brief slot to get triggered to plot a point and send its data to the UI
  /// \param[in] _chart chart ID
  /// \param[in] _fieldID field path ID
  /// \param[in] _x x coordinates of the plot point
  /// \param[in] _y y coordinates of the plot point
  public slots: void onPlot(int _chart, QString _fieldID, double _x, double _y);

  /// \brief plot a point to a chart
  /// \param[in] _chart chart ID
  /// \param[in] _fieldID field path ID
  /// \param[in] _x x coordinates of the plot point
  /// \param[in] _y y coordinates of the plot point
  signals: void plot(int _chart, QString _fieldID, double _x, double _y);

  /// \brief called by Qml to register a chart to a component attribute
  /// \param[in] _entity entity id which has the component
  /// \param[in] _typeId component type id
  /// \param[in] _type component data type
  /// \param[in] _attribute component specefice attribte
  /// \param[in] _chart chart id
  public slots: void onComponentSubscribe(QString _entity,
                                          QString _typeId,
                                          QString _type,
                                          QString _attribute,
                                          int _chart);

  /// \brief called by Qml to remove a chart from a component attribute
  /// \param[in] _entity entity id which has the component
  /// \param[in] _typeId component type id
  /// \param[in] _attribute component specefice attribte
  /// \param[in] _chart chart id
  public slots: void onComponentUnSubscribe(QString _entity,
                                            QString _typeId,
                                            QString _attribute,
                                            int _chart);

  /// \brief Notify the gazebo plugin to subscribe to a component data
  /// \param[in] _entity entity id which has the component
  /// \param[in] _typeId component type id
  /// \param[in] _type component data type
  /// \param[in] _attribute component specefice attribte
  /// \param[in] _chart chart id
  signals: void ComponentSubscribe(uint64_t _entity,
                                   uint64_t _typeId,
                                   const std::string &_type,
                                   const std::string &_attribute,
                                   int _chart);

  /// \brief Notify the gazebo plugin to unsubscribe a component data
  /// \param[in] _entity entity id which has the component
  /// \param[in] _typeId component type id
  /// \param[in] _attribute component specefice attribte
  /// \param[in] _chart chart id
  signals: void ComponentUnSubscribe(uint64_t _entity,
                                     uint64_t _typeId,
                                     const std::string &_attribute,
                                     int _chart);

  /// \brief Create suitable file path with unique name and extention
  /// \param[in] _path path selected from the UI
  /// \param[in] _name file name
  /// \param[in] _extention file extention (csv or pdf)
  public slots: std::string FilePath(QString _path, std::string _name,
                                     std::string _extention);

  /// \brief export plot graphs to csv files
  /// \param[in] _path path of folder to save the csv files
  /// \param[in] _chart plot id to make its name unique
  /// \param[in] _serieses serieses (graphs) of the plot
  /// \return True if successfully export, False if any error
  public slots: bool exportCSV(QString _path, int _chart,
                               QMap< QString, QVariant> _serieses);

  /// \brief Get Component Name based on its type Id
  /// \param[in] _typeId type Id of the component
  /// \return Component name
  signals: std::string ComponentName(uint64_t _typeId);

  /// \brief configration of the timer
  public: void InitTimer();

  /// \brief update the plotting tool time
  public slots: void UpdateTime();

  /// \brief Private data member.
  private: GZ_UTILS_UNIQUE_IMPL_PTR(dataPtr)
};
}  // namespace gz::gui
#endif  // GZ_GUI_PLOTTINGINTERFACE_HH_
