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
#ifndef IGNITION_GUI_PLUGINS_TOPICSSTATS_HH_
#define IGNITION_GUI_PLUGINS_TOPICSSTATS_HH_

#ifndef Q_MOC_RUN
  #include <ignition/gui/qt.h>
#endif

#include <memory>

#include "ignition/gui/Plugin.hh"

// Forward declarations.
namespace google
{
  namespace protobuf
  {
    class Message;
  }
}

namespace ignition
{
  namespace transport
  {
    class MessageInfo;
  }
namespace gui
{
namespace plugins
{
  /// \brief Customize the proxy model to display search results.
  class SearchModel : public QSortFilterProxyModel
  {
    /// \brief Accept a row if the words can be found in itself.
    ///
    /// \param[in] _srcRow Row on the source model.
    /// \param[in] _srcParent Parent on the source model.
    /// \return True if row is accepted.
    public: bool filterAcceptsRow(const int _srcRow,
                                  const QModelIndex &_srcParent) const;

    /// \brief Check if row contains the word on itself.
    /// \param[in] _srcRow Row on the source model.
    /// \param[in] _srcParent Parent on the source model.
    /// \param[in] _word Word to be checked.
    /// \return True if row matches.
    public: bool filterAcceptsRowItself(const int _srcRow,
                                        const QModelIndex &_srcParent,
                                        const QString _word) const;

    // Documentation inherited.
    public: QVariant headerData(int _section,
                                Qt::Orientation _orientation,
                                int _role) const;

    /// \brief Set a new search value.
    /// \param[in] _search Full search string.
    public: void SetSearch(const QString &_search);

    /// \brief Full search string.
    public: QString search;
  };

  // Forward declare private data class.
  class TopicsStatsPrivate;

  /// \brief A TopicsStats for the plot window, where plottable items can be
  /// dragged from.
  class TopicsStats : public Plugin
  {
    Q_OBJECT

    /// \brief Default constructor.
    public: TopicsStats();

    /// \brief Default destructor.
    public: ~TopicsStats();

    // Documentation inherited.
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Function called each time a topic update is received.
    /// Note that this callback uses the generic signature, hence it may receive
    /// messages with different types.
    /// \param[in] _msg The new message received.
    /// \param[in] _info Meta-information about the message received.
    private: void OnMessage(const google::protobuf::Message &_msg,
                            const ignition::transport::MessageInfo &_info);

    /// \brief Reset all the stats.
    private: void ResetStats();

    /// \brief Update the GUI with the new stats.
    private: void UpdateGUIStats();

    /// \brief Callback when the user has modified the search.
    /// \param[in] _search Latest search.
    private slots: void UpdateSearch(const QString &_search);

    /// \brief Fill the topics model.
    private slots: void FillTopics();

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<TopicsStatsPrivate> dataPtr;
  };
}
}
}
#endif
