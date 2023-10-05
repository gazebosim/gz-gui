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
#ifndef GZ_GUI_SEARCHMODEL_HH_
#define GZ_GUI_SEARCHMODEL_HH_

#include "gz/gui/Export.hh"
#include "gz/gui/qt.h"

namespace gz::gui
{
/// \brief Customize the proxy model to display search results.
///
/// Features:
///
/// * This has been tested with QTreeView and QTableView.
/// * Manages expansion of nested items through DataRole::TO_EXPAND when
///   applicable
/// * Items with DataRole::TYPE == "title" are ignored
///
class GZ_GUI_VISIBLE SearchModel : public QSortFilterProxyModel
{
  /// \brief Overloaded Qt method. Customize so we accept rows where:
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
  public: bool FilterAcceptsRowItself(const int _srcRow,
                                      const QModelIndex &_srcParent,
                                      const QString &_word) const;

  /// \brief Check if any of the children is fully accepted.
  /// \param[in] _srcRow Row on the source model.
  /// \param[in] _srcParent Parent on the source model.
  /// \return True if any of the children match.
  public: bool HasAcceptedChildren(const int _srcRow,
                                   const QModelIndex &_srcParent) const;

  /// \brief Check if any of the children accepts a specific word.
  /// \param[in] _srcParent Parent on the source model.
  /// \param[in] _word Word to be checked.
  /// \return True if any of the children match.
  public: bool HasChildAcceptsItself(const QModelIndex &_srcParent,
                                     const QString &_word) const;

  /// \brief Set a new search value.
  /// \param[in] _search Full search string.
  public: void SetSearch(const QString &_search);

  /// \brief Full search string.
  public: QString search;
};
}  // namespace gz::gui
#endif
