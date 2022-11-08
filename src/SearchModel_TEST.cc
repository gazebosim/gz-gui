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

#include <gtest/gtest.h>

#include <gz/common/Console.hh>

#include "test_config.hh"  // NOLINT(build/include)

#include "gz/gui/Enums.hh"
#include "gz/gui/SearchModel.hh"

using namespace gz;
using namespace gui;

/////////////////////////////////////////////////
/// Helper function to count the rows of a nested model
int countRowsOfIndex(const QModelIndex &_index = QModelIndex())
{
  int count = 0;
  auto model = _index.model();
  int rowCount = model->rowCount(_index);
  count += rowCount;
  for (int r = 0; r < rowCount; ++r)
    count += countRowsOfIndex(model->index(r, 0, _index));
  return count;
}

/////////////////////////////////////////////////
TEST(SearchModelTest, FlatStructure)
{
  common::Console::SetVerbosity(4);

  // A source model
  auto sourceModel = new QStandardItemModel();
  ASSERT_NE(nullptr, sourceModel);

  std::vector<std::string> items = {"foo", "bar", "foobar", "foofoo"};
  for (size_t i = 0; i < items.size(); ++i)
  {
    auto it = new QStandardItem();
    ASSERT_NE(nullptr, it);
    it->setData(items[i].c_str(), DataRole::DISPLAY_NAME);
    sourceModel->insertRow(i, it);
  }

  // Add a title
  {
    auto it = new QStandardItem();
    ASSERT_NE(nullptr, it);
    it->setData("The Title", DataRole::DISPLAY_NAME);
    it->setData("title", DataRole::TYPE);
    sourceModel->insertRow(items.size(), it);
  }

  EXPECT_EQ(sourceModel->rowCount(), static_cast<int>(items.size() + 1));

  // A search model
  auto searchModel = new SearchModel();
  ASSERT_NE(nullptr, searchModel);

  searchModel->setFilterRole(DataRole::DISPLAY_NAME);
  searchModel->setSourceModel(sourceModel);

  // Starts with empty search - all items visible minus title
  EXPECT_EQ(searchModel->rowCount(), sourceModel->rowCount() - 1);

  // Searches
  searchModel->SetSearch("lala");
  EXPECT_EQ(searchModel->rowCount(), 0);

  searchModel->SetSearch("foo");
  EXPECT_EQ(searchModel->rowCount(), 3);

  searchModel->SetSearch("bar");
  EXPECT_EQ(searchModel->rowCount(), 2);

  searchModel->SetSearch("foobar");
  EXPECT_EQ(searchModel->rowCount(), 1);

  searchModel->SetSearch("ob");
  EXPECT_EQ(searchModel->rowCount(), 1);
}

/////////////////////////////////////////////////
TEST(SearchModelTest, NestedStructure)
{
  common::Console::SetVerbosity(4);

  // A source model
  auto sourceModel = new QStandardItemModel();
  ASSERT_TRUE(sourceModel != nullptr);

  // Items structure
  // - a
  // -- b
  // -- c
  // --- d
  auto a = new QStandardItem();
  a->setData("a", DataRole::DISPLAY_NAME);
  sourceModel->insertRow(0, a);

  auto b = new QStandardItem();
  b->setData("b", DataRole::DISPLAY_NAME);
  a->appendRow(b);

  auto c = new QStandardItem();
  c->setData("c", DataRole::DISPLAY_NAME);
  a->appendRow(c);

  auto d = new QStandardItem();
  d->setData("d", DataRole::DISPLAY_NAME);
  c->appendRow(d);

  // 4 rows: a and its 3 descendants
  EXPECT_TRUE(sourceModel->hasIndex(0, 0));
  EXPECT_EQ(countRowsOfIndex(sourceModel->index(0, 0)) + 1, 4);

  // A search model
  auto searchModel = new SearchModel();
  ASSERT_TRUE(searchModel != nullptr);

  searchModel->setFilterRole(DataRole::DISPLAY_NAME);
  searchModel->setSourceModel(sourceModel);

  // Starts with empty search - all visible
  EXPECT_EQ(countRowsOfIndex(sourceModel->index(0, 0)),
            countRowsOfIndex(searchModel->index(0, 0)));

  // Searches
  searchModel->SetSearch("a b c d");
  EXPECT_EQ(searchModel->rowCount(), 0);

  // Search for "a" has all rows, but it's not expanded
  searchModel->SetSearch("a");
  EXPECT_TRUE(sourceModel->hasIndex(0, 0));
  auto id = searchModel->index(0, 0);
  EXPECT_FALSE(searchModel->data(id, DataRole::TO_EXPAND).toBool());
  EXPECT_EQ(countRowsOfIndex(id) + 1, 4);

  // Searches which only have rows "a" and "b"
  for (auto s : {"b", "a b"})
  {
    searchModel->SetSearch(s);
    EXPECT_TRUE(sourceModel->hasIndex(0, 0));
    id = searchModel->index(0, 0);
    EXPECT_TRUE(searchModel->data(id, DataRole::TO_EXPAND).toBool());
    EXPECT_EQ(countRowsOfIndex(id) + 1, 2);
  }

  // Searches which only have rows "a", "c", "d"
  for (auto s : {"c", "d", "a c", "a d", "a c d", "c d"})
  {
    searchModel->SetSearch(s);
    EXPECT_TRUE(sourceModel->hasIndex(0, 0));
    id = searchModel->index(0, 0);
    EXPECT_TRUE(searchModel->data(id, DataRole::TO_EXPAND).toBool());
    EXPECT_EQ(countRowsOfIndex(id) + 1, 3);
  }
}

