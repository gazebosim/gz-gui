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

#include <google/protobuf/message.h>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>
#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/transport/Node.hh>

#include "ignition/gui/DragDropModel.hh"
#include "ignition/gui/Enums.hh"
#include "ignition/gui/SearchModel.hh"
#include "ignition/gui/plugins/TopicsStats.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
/// \brief Delegate that handles drawing the topic table
class TableItemDelegate : public QStyledItemDelegate
{
  /// \brief Constructor
  public: TableItemDelegate() = default;

  /// \brief Destructor
  public: virtual ~TableItemDelegate() = default;

  /// \brief Custom paint function.
  /// \param[in] _painter Pointer to the QT painter.
  /// \param[in] _opt Item options.
  /// \param[in] _index Item model index.
  public: void paint(QPainter *_painter, const QStyleOptionViewItem &_opt,
      const QModelIndex &_index) const
  {
    auto textRect = _opt.rect;
    textRect.adjust(10, 12, 10, 12);

    // Custom options.
    QString topicName = qvariant_cast<QString>(_index.data(
        DataRole::DISPLAY_NAME));

    if (topicName.isEmpty())
    {
      ignerr << "Empty topic name, something went wrong." << std::endl;
      return;
    }

    // Draw text
    _painter->setPen(QApplication::palette().text().color());

    auto searchModel = dynamic_cast<const SearchModel *>(_index.model());

    // Create a bold font.
    QFont fontBold, fontRegular;
    fontBold.setFamily("Roboto Bold");
    fontBold.setWeight(QFont::Bold);
    QFontMetrics fmBold(fontBold);

    // Create a regular font.
    fontRegular.setFamily("Roboto Regular");
    fontRegular.setWeight(QFont::Normal);
    QFontMetrics fmRegular(fontRegular);

    // Case insensitive search.
    std::string text(topicName.toStdString());
    std::string upperText(text);
    std::transform(upperText.begin(), upperText.end(),
        upperText.begin(), ::toupper);

    // Split search into words.
    QStringList wordsStringList = searchModel->search.toUpper().split(" ");

    std::vector<std::string> wordsUpper;
    for (auto word : wordsStringList)
    {
      if (word.isEmpty())
        continue;
      wordsUpper.push_back(word.toStdString());
    }

    // Find the portions of text that match the search words, and should
    // therefore be bold.
    //
    // Bold map: key = position of bold text start, value = bold text length.
    std::map<size_t, size_t> bold;
    std::for_each(wordsUpper.begin(), wordsUpper.end(),
        [upperText, &bold](const std::string &_word)
        {
          size_t pos = upperText.find(_word);
          // Find all occurences of _word .
          while (pos != std::string::npos)
          {
            // Use longest word starting at a given position.
            bold[pos] = std::max(bold[pos], _word.size());
            pos = upperText.find(_word, pos + 1);
          }
        });

    // Paint the text from left to right.
    size_t renderPos = 0;
    for (std::map<size_t, size_t>::iterator iter = bold.begin();
         iter != bold.end(); ++iter)
    {
      // Start of bold text.
      size_t start = iter->first;

      // Length of bold text.
      size_t len = iter->second;

      // Check if start is before the current render position.
      if (renderPos > start)
      {
        // It's possible that the bold text goes beyond the current render
        // position. If so, adjust the start and length appropriately.
        if (start + len > renderPos)
        {
          len = (start + len) - renderPos;
          start = renderPos;
        }
        // Otherwise this bold text has already been rendered, so skip.
        else
        {
          continue;
        }
      }

      // First paint text that is not bold.
      auto textStr = QString::fromStdString(
          text.substr(renderPos, start - renderPos));
      renderPos += (start - renderPos);

      _painter->setFont(fontRegular);
      _painter->drawText(textRect, textStr);

      // Move rect to the right.
      textRect.adjust(fmRegular.width(textStr), 0, 0, 0);

      // Next, paint text that is bold.
      textStr = QString::fromStdString(text.substr(renderPos, len));
      renderPos += len;

      _painter->setFont(fontBold);
      _painter->drawText(textRect, textStr);

      // Move rect to the right.
      textRect.adjust(fmBold.width(textStr), 0, 0, 0);
    }

    // Render any remaining text.
    if (renderPos < text.size())
    {
      auto textStr = QString::fromStdString(text.substr(renderPos));
      _painter->setFont(fontRegular);
      _painter->drawText(textRect, textStr);
    }

    QColor gridColor = _opt.widget->palette().color(
        _opt.widget->backgroundRole()).toHsv();

    // Draw grid
    gridColor.setHsv(gridColor.hue(), gridColor.saturation(),
        gridColor.value() > 127 ? gridColor.value() - 20 :
                                  gridColor.value() + 20);

    _painter->setPen(gridColor);

    QPoint p1 = QPoint(_opt.rect.bottomLeft().x()-1,
                       _opt.rect.bottomLeft().y());
    QPoint p2 = QPoint(_opt.rect.bottomRight().x()+1,
                       _opt.rect.bottomRight().y());
    _painter->drawLine(p1, p2);
  }

  /// \brief Size hint tells QT how big an item is.
  /// \param[in] _option Style options
  /// \param[in] _index Item model index
  public: QSize sizeHint(const QStyleOptionViewItem &_option,
                         const QModelIndex &_index) const
  {
    QSize size = QStyledItemDelegate::sizeHint(_option, _index);
    QFontMetrics fm(QApplication::font());

    // Make it slightly larger.
    size.setHeight(fm.height() + 10);

    return size;
  }
};

/// \brief Private data for the TopicsStats class.
class BasicStats
{
  /// \brief Total number of messages received.
  public: uint64_t numMessages = 0;

  /// \brief Number of messages received during the last second.
  public: uint64_t numMessagesLastSec = 0;

  /// \brief Number of bytes received during the last second.
  public: uint64_t numBytesLastSec = 0;

  /// \brief A pointer to the QT item storing the total number of messages.
  public: QStandardItem *numMessagesItem = nullptr;

  /// \brief A pointer to the QT item storing the number of messages received
  /// during the last second.
  public: QStandardItem *numMessagesLastSecItem = nullptr;

  /// \brief A pointer to the QT item storing the number of bytes received
  /// during the last second.
  public: QStandardItem *numBytesLastSecItem = nullptr;
};

/// \brief Private data for the TopicsStats class
class ignition::gui::plugins::TopicsStatsPrivate
{
  /// \brief Model to hold topics data.
  public: DragDropModel *topicsModel;

  /// \brief Proxy model to filter topics data.
  public: SearchModel *searchTopicsModel;

  /// \brief View holding the search topics tree.
  public: QTableView *searchTopicsTable;

  /// \brief Communication node.
  public: ignition::transport::Node node;

  /// \brief Topics displayed in the last update.
  public: std::vector<std::string> prevTopics;

  /// \brief Stores the stats per topic. The key is the topic name and the value
  /// an object that stores the stats.
  public: std::map<std::string, BasicStats> stats;

  /// \brief Mutex to protect the models model update.
  public: std::mutex modelsMutex;
};

/////////////////////////////////////////////////
TopicsStats::TopicsStats()
  : Plugin(),
    dataPtr(new TopicsStatsPrivate)
{
}

/////////////////////////////////////////////////
TopicsStats::~TopicsStats()
{
}

/////////////////////////////////////////////////
void TopicsStats::LoadConfig(const tinyxml2::XMLElement */*_pluginElem*/)
{
  if (this->title.empty())
    this->title = "Topics stats";

  // Create a view delegate, to handle drawing items in the tree view.
  auto topicsItemDelegate = new TableItemDelegate;

  // The model that will hold data to be displayed in the topic tree view.
  this->dataPtr->topicsModel = new DragDropModel;
  this->dataPtr->topicsModel->setObjectName("topicsModel");
  this->dataPtr->topicsModel->setParent(this);
  this->dataPtr->topicsModel->setColumnCount(4);

  // A proxy model to filter topic model.
  this->dataPtr->searchTopicsModel = new SearchModel;
  this->dataPtr->searchTopicsModel->setFilterRole(DataRole::DISPLAY_NAME);
  this->dataPtr->searchTopicsModel->setSourceModel(this->dataPtr->topicsModel);

  this->dataPtr->searchTopicsModel->setHeaderData(0, Qt::Horizontal,
      QString("Topic"), Qt::DisplayRole);
  this->dataPtr->searchTopicsModel->setHeaderData(1, Qt::Horizontal,
      QString("Num messages"), Qt::DisplayRole);
  this->dataPtr->searchTopicsModel->setHeaderData(2, Qt::Horizontal,
      QString("Frequency"), Qt::DisplayRole);
  this->dataPtr->searchTopicsModel->setHeaderData(3, Qt::Horizontal,
      QString("Bandwidth"), Qt::DisplayRole);

  // Search field.
  auto searchIcon = new QLabel();
  searchIcon->setPixmap(QPixmap(":/images/search.svg"));

  auto searchEdit = new QLineEdit();
  searchEdit->setPlaceholderText("Start typing to search...");
  searchEdit->setObjectName("topicLineEdit");
  this->connect(searchEdit, SIGNAL(textChanged(QString)), this,
      SLOT(UpdateSearch(QString)));

  auto searchField = new QHBoxLayout();
  searchField->addWidget(searchIcon);
  searchField->addWidget(searchEdit);

  // Visualize topics search results.
  this->dataPtr->searchTopicsTable = new QTableView;
  this->dataPtr->searchTopicsTable->setObjectName("topicsTable");
  this->dataPtr->searchTopicsTable->verticalHeader()->setVisible(false);
  this->dataPtr->searchTopicsTable->verticalHeader()->setDefaultSectionSize(42);
  this->dataPtr->searchTopicsTable->setShowGrid(false);
  this->dataPtr->searchTopicsTable->setSelectionBehavior(
      QAbstractItemView::SelectRows);
  this->dataPtr->searchTopicsTable->horizontalHeader()->setHighlightSections(
      false);
  this->dataPtr->searchTopicsTable->horizontalHeader()->setStretchLastSection(
      true);

  this->dataPtr->searchTopicsTable->setModel(this->dataPtr->searchTopicsModel);
  this->dataPtr->searchTopicsTable->setItemDelegate(topicsItemDelegate);
  this->dataPtr->searchTopicsTable->setEditTriggers(
    QAbstractItemView::NoEditTriggers);
  this->dataPtr->searchTopicsTable->setDragEnabled(true);
  this->dataPtr->searchTopicsTable->setDragDropMode(
    QAbstractItemView::DragOnly);

  this->dataPtr->searchTopicsTable->setColumnWidth(0, 350);
  this->dataPtr->searchTopicsTable->setColumnWidth(1, 120);
  this->dataPtr->searchTopicsTable->setColumnWidth(2, 120);
  this->dataPtr->searchTopicsTable->setColumnWidth(3, 120);

  auto splitter = new QSplitter(Qt::Vertical, this);
  splitter->addWidget(this->dataPtr->searchTopicsTable);
  splitter->setCollapsible(0, false);
  splitter->setStretchFactor(0, 1);

  auto searchLayout = new QVBoxLayout();
  searchLayout->addLayout(searchField);
  searchLayout->addWidget(splitter);

  auto searchWidget = new QWidget();
  searchWidget->setLayout(searchLayout);

  // Main frame
  auto mainFrameLayout = new QVBoxLayout;
  mainFrameLayout->addWidget(searchWidget);
  mainFrameLayout->setContentsMargins(0, 0, 0, 0);

  auto mainFrame = new QFrame(this);
  mainFrame->setObjectName("TopicsStatsFrame");
  mainFrame->setLayout(mainFrameLayout);

  auto mainLayout = new QHBoxLayout;
  mainLayout->addWidget(mainFrame);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  this->setMinimumWidth(730);
  this->setLayout(mainLayout);

  this->UpdateSearch("");

  // A periodic event to update the topic list.
  auto timer = new QTimer(this);
  this->connect(timer, SIGNAL(timeout()), this, SLOT(FillTopics()));
  timer->start(1000);
}

/////////////////////////////////////////////////
void TopicsStats::FillTopics()
{
  // Get all the unique topics.
  std::vector<std::string> topics;
  this->dataPtr->node.TopicList(topics);

  std::lock_guard<std::mutex> lock(this->dataPtr->modelsMutex);

  // Remove expired topics.
  for (auto i = 0u; i < this->dataPtr->prevTopics.size(); ++i)
  {
    auto topic = this->dataPtr->prevTopics.at(i);
    if (std::find(topics.begin(), topics.end(), topic) == topics.end())
    {
      this->dataPtr->topicsModel->takeRow(i);

      // Unsubscribe from the topic.
     this->dataPtr->node.Unsubscribe(topic);

      // Do not track stats for this topic anymore.
      this->dataPtr->stats.erase(topic);
    }
  }

  // Add new topics.
  for (auto i = 0u; i < topics.size(); ++i)
  {
    auto topic = topics.at(i);

    if (std::find(this->dataPtr->prevTopics.begin(),
          this->dataPtr->prevTopics.end(), topic) ==
          this->dataPtr->prevTopics.end())
    {
      // Subscribe to the topic.
      auto cb = std::bind(&TopicsStats::OnMessage, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);
      if (!this->dataPtr->node.Subscribe(topic, cb))
      {
        std::cerr << "Error subscribing to [" << topic << "]" << std::endl;
        continue;
      }

      // Start tracking stats for this topic.
      this->dataPtr->stats[topic] = BasicStats();

      auto topicItem = new QStandardItem();
      topicItem->setData(topic.c_str(), DataRole::DISPLAY_NAME);
      this->dataPtr->topicsModel->insertRow(i, topicItem);

      this->dataPtr->stats[topic].numMessagesItem = new QStandardItem();
      this->dataPtr->topicsModel->setItem(
        i, 1, this->dataPtr->stats[topic].numMessagesItem);

      this->dataPtr->stats[topic].numMessagesLastSecItem = new QStandardItem();
      this->dataPtr->topicsModel->setItem(
        i, 2, this->dataPtr->stats[topic].numMessagesLastSecItem);

      this->dataPtr->stats[topic].numBytesLastSecItem = new QStandardItem();
      this->dataPtr->topicsModel->setItem(
        i, 3, this->dataPtr->stats[topic].numBytesLastSecItem);
    }
  }

  this->UpdateGUIStats();
  this->ResetStats();
  this->dataPtr->prevTopics = topics;
}

/////////////////////////////////////////////////
void TopicsStats::UpdateGUIStats()
{
  // Update stats.
  for (auto &statsPair : this->dataPtr->stats)
  {
    // Total number of messages received.
    statsPair.second.numMessagesItem->setData(
        QString::number(statsPair.second.numMessages),
        DataRole::DISPLAY_NAME);

    // Number of messages received during the last second.
    std::string f = std::to_string(statsPair.second.numMessagesLastSec) + " Hz";
    statsPair.second.numMessagesLastSecItem->setData(
        QString::fromStdString(f),
        DataRole::DISPLAY_NAME);

    // Number of bytes received during the last second.
    double bandwidth = statsPair.second.numBytesLastSec;

    std::string units;
    if (bandwidth < 1000)
      units = "B/s";
    else if (bandwidth < 1000000)
    {
      bandwidth /= 1000.0;
      units = "KB/s";
    }
    else
    {
      bandwidth /= 1000000.0;
      units = "MB/s";
    }

    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << bandwidth << " " << units;
    statsPair.second.numBytesLastSecItem->setData(
        QString::fromStdString(stream.str()),
        DataRole::DISPLAY_NAME);
  }
}

/////////////////////////////////////////////////
void TopicsStats::OnMessage(const char */*_msgData*/, const size_t _size,
    const ignition::transport::MessageInfo &_info)
{
  auto topic = _info.Topic();

  std::lock_guard<std::mutex> lock(this->dataPtr->modelsMutex);

  const auto &statsPair = this->dataPtr->stats.find(topic);
  if (statsPair == this->dataPtr->stats.end())
  {
    std::cerr << "Unable to find stats for [" << topic << "]" << std::endl;
    return;
  }
  auto &stats = statsPair->second;

  // Update the total number of messages received.
  stats.numMessages++;

  // Update the number of messages received during the last second.
  stats.numMessagesLastSec++;

  // Update the number of bytes received during the last second.
  stats.numBytesLastSec += _size;
}

/////////////////////////////////////////////////
void TopicsStats::ResetStats()
{
  for (auto &topicPair : this->dataPtr->stats)
  {
    topicPair.second.numMessagesLastSec = 0;
    topicPair.second.numBytesLastSec = 0;
  }
}

/////////////////////////////////////////////////
void TopicsStats::UpdateSearch(const QString &_search)
{
  this->dataPtr->searchTopicsModel->SetSearch(_search);
}

// Register this plugin.
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::TopicsStats,
                                  ignition::gui::Plugin)
