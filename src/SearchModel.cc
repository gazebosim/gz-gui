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

#include <gz/common/Console.hh>

#include "gz/gui/Enums.hh"
#include "gz/gui/SearchModel.hh"

namespace gz::gui
{
/////////////////////////////////////////////////
bool SearchModel::filterAcceptsRow(const int _srcRow,
      const QModelIndex &_srcParent) const
{
  // Item index in search model.
  auto id = this->sourceModel()->index(_srcRow, 0, _srcParent);

  // Ignore titles.
  if (this->sourceModel()->data(id, DataRole::TYPE).toString() ==
      "title")
  {
    return false;
  }

  // Collapsed by default.
  this->sourceModel()->blockSignals(true);
  this->sourceModel()->setData(id, false, DataRole::TO_EXPAND);
  this->sourceModel()->blockSignals(false);

  // Empty search matches everything.
  if (this->search.isEmpty())
    return true;

  // Each word must match at least once, either self, parent or child.
  auto words = this->search.split(" ");
  for (const auto &word : words)
  {
    if (word.isEmpty())
      continue;

    // Expand this if at least one child contains the word.
    // Note that this is not enough for this to be accepted, we need to match
    // all words.
    if (this->HasChildAcceptsItself(id, word))
    {
      this->sourceModel()->blockSignals(true);
      this->sourceModel()->setData(id, true, DataRole::TO_EXPAND);
      this->sourceModel()->blockSignals(false);
    }

    // At least one of the children fits rule 1.
    if (this->HasAcceptedChildren(_srcRow, _srcParent))
      continue;

    // Row itself contains this word.
    if (this->FilterAcceptsRowItself(_srcRow, _srcParent, word))
      continue;

    // One of the ancestors contains this word.
    QModelIndex parentIndex = _srcParent;
    bool parentAccepted = false;
    while (parentIndex.isValid())
    {
      if (this->FilterAcceptsRowItself(parentIndex.row(),
          parentIndex.parent(), word))
      {
        parentAccepted = true;
        break;
      }
      parentIndex = parentIndex.parent();
    }

    if (parentAccepted)
      continue;

    // This word can't be found on the row or a parent, and no child is fully
    // accepted.
    return false;
  }

  return true;
}

/////////////////////////////////////////////////
bool SearchModel::FilterAcceptsRowItself(const int _srcRow,
    const QModelIndex &_srcParent, const QString &_word) const
{
  auto id = this->sourceModel()->index(_srcRow, 0, _srcParent);

  return (this->sourceModel()->data(id,
      this->filterRole()).toString().contains(_word, Qt::CaseInsensitive));
}

/////////////////////////////////////////////////
bool SearchModel::HasAcceptedChildren(const int _srcRow,
      const QModelIndex &_srcParent) const
{
  auto item = sourceModel()->index(_srcRow, 0, _srcParent);

  if (!item.isValid())
    return false;

  for (int i = 0; i < item.model()->rowCount(item); ++i)
  {
    if (this->filterAcceptsRow(i, item))
      return true;
  }

  return false;
}

/////////////////////////////////////////////////
bool SearchModel::HasChildAcceptsItself(const QModelIndex &_srcParent,
      const QString &_word) const
{
  for (int i = 0; i < this->sourceModel()->rowCount(_srcParent); ++i)
  {
    // Check immediate children.
    if (this->FilterAcceptsRowItself(i, _srcParent, _word))
      return true;

    // Check grandchildren.
    auto item = this->sourceModel()->index(i, 0, _srcParent);
    if (this->HasChildAcceptsItself(item, _word))
      return true;
  }

  return false;
}

/////////////////////////////////////////////////
void SearchModel::SetSearch(const QString &_search)
{
  this->search = _search;

  // Trigger repaint on whole model
  this->invalidateFilter();

  // TODO(anyone): Figure out why filterChanged works for TopicViewer but not
  // TopicsStats
  emit this->layoutChanged();
}
}  // namespace gz::gui
