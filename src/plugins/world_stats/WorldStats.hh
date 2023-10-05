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

#ifndef GZ_GUI_PLUGINS_WORLDSTATS_HH_
#define GZ_GUI_PLUGINS_WORLDSTATS_HH_

#include <memory>

#include <gz/msgs/world_stats.pb.h>

#include "gz/gui/Export.hh"
#include "gz/gui/Plugin.hh"

#ifndef _WIN32
#  define WorldStats_EXPORTS_API
#else
#  if (defined(WorldStats_EXPORTS))
#    define WorldStats_EXPORTS_API __declspec(dllexport)
#  else
#    define WorldStats_EXPORTS_API __declspec(dllimport)
#  endif
#endif

namespace gz::gui::plugins
{
class WorldStatsPrivate;

/// \brief This plugin provides a time panel which may display:
/// * Simulation time
/// * Real time
/// * Real time factor
/// * Iterations
///
/// ## Configuration
///
/// * \<sim_time\> : Set to true to display a sim time widget, false by
///                  default.
/// * \<real_time\> : True to display a real time widget, false by default.
/// * \<real_time_factor\> : True to display a real time factor widget,
///                          false by default.
/// * \<iterations\> : True to display an iterations widget, false by default.
/// * \<topic\> : Topic to receive world statistics, optional. If not present,
///               the plugin will attempt to create a topic with the main
///               window's `worldName` property.
///
/// If no elements are filled for the plugin, all properties will be
/// displayed.
class WorldStats_EXPORTS_API WorldStats: public gz::gui::Plugin
{
  Q_OBJECT

  /// \brief Real time factor
  Q_PROPERTY(
    QString realTimeFactor
    READ RealTimeFactor
    WRITE SetRealTimeFactor
    NOTIFY RealTimeFactorChanged
  )

  /// \brief Sim time
  Q_PROPERTY(
    QString simTime
    READ SimTime
    WRITE SetSimTime
    NOTIFY SimTimeChanged
  )

  /// \brief Real time
  Q_PROPERTY(
    QString realTime
    READ RealTime
    WRITE SetRealTime
    NOTIFY RealTimeChanged
  )

  /// \brief Iterations
  Q_PROPERTY(
    QString iterations
    READ Iterations
    WRITE SetIterations
    NOTIFY IterationsChanged
  )

  /// \brief Constructor
  public: WorldStats();

  /// \brief Destructor
  public: virtual ~WorldStats();

  // Documentation inherited
  public: void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

  /// \brief Callback in main thread when diagnostics come in
  public slots: void ProcessMsg();

  /// \brief Get the message type as a string, for example
  /// \return Message type
  public: Q_INVOKABLE QString RealTimeFactor() const;

  /// \brief Set the message type from a string, for example
  /// \param[in] _realTimeFactor Message type
  public: Q_INVOKABLE void SetRealTimeFactor(const QString &_realTimeFactor);

  /// \brief Notify that message type has changed
  signals: void RealTimeFactorChanged();

  /// \brief Get the message type as a string, for example
  /// \return Message type
  public: Q_INVOKABLE QString SimTime() const;

  /// \brief Set the message type from a string, for example
  /// \param[in] _simTime Message type
  public: Q_INVOKABLE void SetSimTime(const QString &_simTime);

  /// \brief Notify that message type has changed
  signals: void SimTimeChanged();

  /// \brief Get the message type as a string, for example
  /// \return Message type
  public: Q_INVOKABLE QString RealTime() const;

  /// \brief Set the message type from a string, for example
  /// \param[in] _realTime Message type
  public: Q_INVOKABLE void SetRealTime(const QString &_realTime);

  /// \brief Notify that message type has changed
  signals: void RealTimeChanged();

  /// \brief Get the message type as a string, for example
  /// \return Message type
  public: Q_INVOKABLE QString Iterations() const;

  /// \brief Set the message type from a string, for example
  /// \param[in] _iterations Message type
  public: Q_INVOKABLE void SetIterations(const QString &_iterations);

  /// \brief Notify that message type has changed
  signals: void IterationsChanged();

  /// \brief Subscriber callback when new world statistics are received
  private: void OnWorldStatsMsg(const gz::msgs::WorldStatistics &_msg);

  // Private data
  private: std::unique_ptr<WorldStatsPrivate> dataPtr;
};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_WORLDSTATS_HH_
