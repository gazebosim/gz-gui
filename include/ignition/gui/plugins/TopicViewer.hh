/*
 * Copyright (C) 2016 Open Source Robotics Foundation
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
#ifndef IGNITION_GUI_PLUGINS_TOPICVIEWER_HH_
#define IGNITION_GUI_PLUGINS_TOPICVIEWER_HH_

#ifndef Q_MOC_RUN
  #include <ignition/gui/qt.h>
#endif

#include <memory>
#include <string>

#include "ignition/gui/Plugin.hh"
#include "ignition/msgs.hh"

namespace google
{
  namespace protobuf
  {
    class Message;
  }
}

namespace ignition
{
namespace gui
{
namespace plugins
{
  /// \brief Customize the proxy model to display search results.
  class SearchModel : public QSortFilterProxyModel
  {
    /// \brief Customize so we accept rows where:
    /// 1. Each of the words can be found in its ancestors or itself, but not
    /// necessarily all words on the same row, or
    /// 2. One of its descendants matches rule 1, or
    /// 3. One of its ancestors matches rule 1.
    ///
    /// For example this structure:
    /// - a
    /// -- b
    /// -- c
    /// --- d
    ///
    /// * A search of "a" will display all rows.
    /// * A search of "b" or "a b" will display "a" and "b".
    /// * A search of "c", "d", "a c", "a d", "a c d" or "c d" will display
    /// "a", "c" and "d".
    /// * A search of "a b c d", "b c" or "b d" will display nothing.
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

    /// \brief Check if any of the children is fully accepted.
    /// \param[in] _srcRow Row on the source model.
    /// \param[in] _srcParent Parent on the source model.
    /// \return True if any of the children match.
    public: bool hasAcceptedChildren(const int _srcRow,
                                     const QModelIndex &_srcParent) const;

    /// \brief Check if any of the children accepts a specific word.
    /// \param[in] _srcParent Parent on the source model.
    /// \param[in] _word Word to be checked.
    /// \return True if any of the children match.
    public: bool hasChildAcceptsItself(const QModelIndex &_srcParent,
                                       const QString &_word) const;

    /// \brief Set a new search value.
    /// \param[in] _search Full search string.
    public: void SetSearch(const QString &_search);

    /// \brief Full search string.
    public: QString search;
  };

  // Forward declare private data class
  class TopicViewerPrivate;

  /// \brief A TopicViewer for the plot window, where plottable items can be
  /// dragged from.
  class TopicViewer : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: TopicViewer();

    /// \brief Destructor
    public: ~TopicViewer();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Fill an item with properties from a protobuf message.
    /// Only plottable fields such as int, double and bool are displayed.
    /// \param[in] _msg A basic message from the topic's message type.
    /// \param[in] _item Pointer to the item which will be filled.
    /// \param[in] _uri The current URI.
    private: void FillFromMsg(google::protobuf::Message *_msg,
                              QStandardItem *_item,
                              const std::string &_uri);

    /// \brief Expand items in the given tree view based on their model data.
    /// \param[in] _model Search model.
    /// \param[in] _tree Tree view.
    /// \param[in] _srcParent Model index of the parent to be checked.
    private: void ExpandChildren(QSortFilterProxyModel *_model,
                                 QTreeView *_tree,
                                 const QModelIndex &_srcParent) const;

    /// \brief Callback when the user has modified the search.
    /// \param[in] _search Latest search.
    private slots: void UpdateSearch(const QString &_search);

    /// \brief Expand given items tree on single click.
    /// \param[in] _index Index of item within the tree.
    private slots: void ExpandTree(const QModelIndex &_index);

    /// \brief Fill the topics model.
    private slots: void FillTopics();

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<TopicViewerPrivate> dataPtr;
  };
}
}
}
#endif
