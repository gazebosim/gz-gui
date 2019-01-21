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
#include <mutex>
#include <string>
#include <vector>
#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/transport/Node.hh>

#include "ignition/common/URI.hh"
#include "ignition/gui/DragDropModel.hh"
#include "ignition/gui/Enums.hh"
#include "ignition/gui/SearchModel.hh"
#include "ignition/gui/plugins/TopicViewer.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
/// \brief Delegate that handles drawing the topic tree
class TreeItemDelegate : public QStyledItemDelegate
{
  /// \brief Constructor
  public: TreeItemDelegate() = default;

  /// \brief Destructor
  public: virtual ~TreeItemDelegate() = default;

  /// \brief Custom paint function.
  /// \param[in] _painter Pointer to the QT painter.
  /// \param[in] _opt Item options.
  /// \param[in] _index Item model index.
  public: void paint(QPainter *_painter, const QStyleOptionViewItem &_opt,
      const QModelIndex &_index) const
  {
    auto textRect = _opt.rect;

    // Custom options.
    QString topicName = qvariant_cast<QString>(_index.data(DISPLAY_NAME));
    QString typeName = qvariant_cast<QString>(_index.data(TYPE));

    // TODO: Change to QApplication::font() once Roboto is used everywhere.
    QFont fontBold, fontRegular;

    // Create a bold font.
    fontBold.setFamily("Roboto Bold");
    fontBold.setWeight(QFont::Bold);
    QFontMetrics fmBold(fontBold);

    // Create a regular font.
    fontRegular.setFamily("Roboto Regular");
    fontRegular.setWeight(QFont::Normal);
    QFontMetrics fmRegular(fontRegular);

    if (typeName == "title")
    {
      // Erase the branch image for titles.
      QRectF titleRect = _opt.rect;
      titleRect.setLeft(titleRect.left() - 13);
      QBrush brush(_opt.widget->palette().color(
            _opt.widget->backgroundRole()));
      _painter->save();
      _painter->fillRect(titleRect, brush);
      _painter->restore();
    }

    // Handle hover style.
    if (typeName != "title" && _opt.state & QStyle::State_MouseOver)
    {
      _painter->setPen(QApplication::palette().highlight().color());
      _painter->setBrush(QApplication::palette().highlight().color());
      _painter->drawRect(_opt.rect);
    }

    // Titles.
    if (typeName == "title")
    {
      textRect.adjust(-15, 5, 0, -5);
    }
    // Plottable items.
    else if (!typeName.isEmpty())
    {
      // Paint icon.
      double iconSize = 20;

      QRectF iconRect = _opt.rect;
      iconRect.setTop(iconRect.top() + (_opt.rect.height()/2.0 - iconSize/2.0));

      QIcon icon(":/images/graph_line.svg");
      _painter->drawPixmap(iconRect.left(), iconRect.top(),
          icon.pixmap(iconSize, iconSize));

      // Move text.
      textRect.adjust(iconSize + 5, 5, 0, -5);
    }
    // Normal expandable items.
    else
    {
      // Otherwise use a rectangle that is sized for the just the topic name.
      textRect.adjust(0, 5, 0, -5);
    }

    _painter->setPen(QApplication::palette().text().color());

    // If this is a search result.
    auto searchModel = dynamic_cast<const SearchModel *>(_index.model());
    if (searchModel && !topicName.isEmpty())
    {
      std::string text(topicName.toStdString());

      // Case insensitive search.
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
    }
    else
    {
      _painter->setFont(typeName == "title" ? fontBold : fontRegular);
      _painter->drawText(textRect, topicName);
    }
  }

  /// \brief Size hint tells QT how big an item is.
  /// \param[in] _option Style options
  /// \param[in] _index Item model index
  public: QSize sizeHint(const QStyleOptionViewItem &_option,
                         const QModelIndex &_index) const
  {
    QSize size = QStyledItemDelegate::sizeHint(_option, _index);

    // TODO: Change to QApplication::font() once Roboto is used everywhere.
    QFont font("Roboto Regular");
    QFontMetrics fm(font);

    // Make it slightly larger.
    size.setHeight(fm.height() + 10);

    return size;
  }
};

/////////////////////////////////////////////////
std::string humanReadableKey(const std::string &_key)
{
  std::string humanKey = _key;
  humanKey[0] = toupper(humanKey[0]);
  std::replace(humanKey.begin(), humanKey.end(), '_', ' ');
  return humanKey;
}

/////////////////////////////////////////////////
/// \brief Private data for the TopicViewer class
class ignition::gui::plugins::TopicViewerPrivate
{
  /// \brief Model to hold topics data.
  public: DragDropModel *topicsModel;

  /// \brief Proxy model to filter topics data.
  public: SearchModel *searchTopicsModel;

  /// \brief View holding the search topics tree.
  public: QTreeView *searchTopicsTree;

  /// \brief Communication node.
  public: ignition::transport::Node node;

  /// \brief Topics displayed in the last update.
  public: std::vector<std::string> prevTopics;

  /// \brief Mutex to protect the models model update.
  public: std::mutex modelsMutex;
};

/////////////////////////////////////////////////
TopicViewer::TopicViewer()
  : Plugin(),
    dataPtr(new TopicViewerPrivate)
{
}

/////////////////////////////////////////////////
TopicViewer::~TopicViewer()
{
}

/////////////////////////////////////////////////
void TopicViewer::LoadConfig(const tinyxml2::XMLElement */*_pluginElem*/)
{
  if (this->title.empty())
    this->title = "Topic viewer";

  // Create a view delegate, to handle drawing items in the tree view.
  auto topicsItemDelegate = new TreeItemDelegate;

  // The model that will hold data to be displayed in the topic tree view.
  this->dataPtr->topicsModel = new DragDropModel;
  this->dataPtr->topicsModel->setObjectName("topicsModel");
  this->dataPtr->topicsModel->setParent(this);

  // A proxy model to filter topic model.
  this->dataPtr->searchTopicsModel = new SearchModel;
  this->dataPtr->searchTopicsModel->setFilterRole(DataRole::DISPLAY_NAME);
  this->dataPtr->searchTopicsModel->setSourceModel(this->dataPtr->topicsModel);

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

  // A tree to visualize topics search results.
  this->dataPtr->searchTopicsTree = new QTreeView;
  this->dataPtr->searchTopicsTree->setObjectName("topicsTree");
  this->dataPtr->searchTopicsTree->setAnimated(true);
  this->dataPtr->searchTopicsTree->setHeaderHidden(true);
  this->dataPtr->searchTopicsTree->setExpandsOnDoubleClick(true);
  this->dataPtr->searchTopicsTree->setModel(this->dataPtr->searchTopicsModel);
  this->dataPtr->searchTopicsTree->setItemDelegate(topicsItemDelegate);
  this->dataPtr->searchTopicsTree->setEditTriggers(
      QAbstractItemView::NoEditTriggers);
  this->dataPtr->searchTopicsTree->setDragEnabled(true);
  this->dataPtr->searchTopicsTree->setDragDropMode(QAbstractItemView::DragOnly);
  this->connect(this->dataPtr->searchTopicsTree,
      SIGNAL(clicked(const QModelIndex &)),
      this, SLOT(ExpandTree(const QModelIndex &)));

  auto splitter = new QSplitter(Qt::Vertical, this);
  splitter->addWidget(this->dataPtr->searchTopicsTree);
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
  mainFrame->setObjectName("topicViewerFrame");
  mainFrame->setLayout(mainFrameLayout);

  auto mainLayout = new QHBoxLayout;
  mainLayout->addWidget(mainFrame);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  this->setMinimumWidth(350);
  this->setLayout(mainLayout);

  this->UpdateSearch("");

  // A periodic event to update the topic list.
  auto timer = new QTimer(this);
  this->connect(timer, SIGNAL(timeout()), this, SLOT(FillTopics()));
  timer->start(1000);
}

/////////////////////////////////////////////////
void TopicViewer::FillTopics()
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
      this->dataPtr->topicsModel->takeRow(i);
  }

  // Add new topics.
  for (auto i = 0u; i < topics.size(); ++i)
  {
    auto topic = topics.at(i);

    if (std::find(this->dataPtr->prevTopics.begin(),
          this->dataPtr->prevTopics.end(), topic) ==
          this->dataPtr->prevTopics.end())
    {
      auto topicItem = new QStandardItem();
      topicItem->setData(topic.c_str(), DataRole::DISPLAY_NAME);
      this->dataPtr->topicsModel->insertRow(i, topicItem);

      // Create a message from this topic to find out its fields.
      std::vector<ignition::transport::MessagePublisher> publishers;
      this->dataPtr->node.TopicInfo(topic, publishers);
      // ToDo: Multiple publishers of different type.
      auto msgType = publishers.front().MsgTypeName();
      auto msg = ignition::msgs::Factory::New(msgType);
      // ToDo: What if msgType was not found.
      this->FillFromMsg(msg.get(), topicItem, topic + "?p=");
    }
  }

  this->dataPtr->prevTopics = topics;
}

/////////////////////////////////////////////////
void TopicViewer::FillFromMsg(google::protobuf::Message *_msg,
    QStandardItem *_item, const std::string &_uri)
{
  if (!_msg || !_item)
    return;

  auto ref = _msg->GetReflection();
  if (!ref)
    return;

  auto descriptor = _msg->GetDescriptor();
  if (!descriptor)
    return;

  auto count = descriptor->field_count();

  // Go through all fields in this message.
  for (int i = 0; i < count; ++i)
  {
    auto field = descriptor->field(i);
    if (!field)
      return;

    auto name = field->name();

    if (field->is_repeated())
      continue;

    switch (field->type())
    {
      case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
      case google::protobuf::FieldDescriptor::TYPE_FLOAT:
      case google::protobuf::FieldDescriptor::TYPE_INT64:
      case google::protobuf::FieldDescriptor::TYPE_UINT64:
      case google::protobuf::FieldDescriptor::TYPE_INT32:
      case google::protobuf::FieldDescriptor::TYPE_UINT32:
      case google::protobuf::FieldDescriptor::TYPE_BOOL:
      case google::protobuf::FieldDescriptor::TYPE_STRING:
      {
        auto humanName = humanReadableKey(name);

        auto *childItem = new QStandardItem();
        childItem->setData(humanName.c_str(), DataRole::DISPLAY_NAME);
        childItem->setDragEnabled(true);

        switch (field->type())
        {
          case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            childItem->setData("Double", DataRole::TYPE);
            break;
          case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            childItem->setData("Float", DataRole::TYPE);
            break;
          case google::protobuf::FieldDescriptor::TYPE_INT64:
            childItem->setData("Int 64", DataRole::TYPE);
            break;
          case google::protobuf::FieldDescriptor::TYPE_UINT64:
            childItem->setData("Uint 64", DataRole::TYPE);
            break;
          case google::protobuf::FieldDescriptor::TYPE_INT32:
            childItem->setData("Int 32", DataRole::TYPE);
            break;
          case google::protobuf::FieldDescriptor::TYPE_UINT32:
            childItem->setData("Uint 32", DataRole::TYPE);
            break;
          case google::protobuf::FieldDescriptor::TYPE_BOOL:
            childItem->setData("Bool", DataRole::TYPE);
            break;
          case google::protobuf::FieldDescriptor::TYPE_STRING:
            childItem->setData("String", DataRole::TYPE);
            childItem->setDragEnabled(false);
            break;
          default:
            continue;
        }
        childItem->setToolTip(
            "<font size=3><p><b>Type</b>: " + childItem->data(
            DataRole::TYPE).toString() +
            "</p></font>");

        std::string dataName = _uri + "/" + name;
        childItem->setData(dataName.c_str(), DataRole::URI_QUERY);

        _item->appendRow(childItem);
        break;
      }
      // Message within a message.
      case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
      {
        // Treat time as double.
        if (field->message_type()->name() == "Time")
        {
          auto humanName = humanReadableKey(name);
          std::string dataName = _uri + "/" + name;

          auto *childItem = new QStandardItem();
          childItem->setData(humanName.c_str(), DataRole::DISPLAY_NAME);
          childItem->setData(dataName.c_str(), DataRole::URI_QUERY);
          childItem->setData("Double", DataRole::TYPE);
          childItem->setDragEnabled(true);
          childItem->setToolTip(
              "<font size=3><p><b>Type</b>: " + childItem->data(
              DataRole::TYPE).toString() +
              "</p></font>");

          _item->appendRow(childItem);
        }
        // Custom RPY widgets for orientation.
        else if (field->message_type()->name() == "Quaternion")
        {
          auto *quatItem = new QStandardItem();
          quatItem->setData(name.c_str(), DataRole::DISPLAY_NAME);
          _item->appendRow(quatItem);

          std::vector<std::string> rpy = {"roll", "pitch", "yaw"};
          for (auto it : rpy)
          {
            auto humanName = humanReadableKey(it);
            std::string dataName = _uri + "/" + name + "/" + it;

            auto *childItem = new QStandardItem();
            childItem->setData(QString::fromStdString(humanName),
                DataRole::DISPLAY_NAME);
            childItem->setData(dataName.c_str(), DataRole::URI_QUERY);
            childItem->setData("Double", DataRole::TYPE);
            childItem->setToolTip(
                "<font size=3><p><b>Type</b>: " + childItem->data(
                DataRole::TYPE).toString() +
                "</p></font>");
            childItem->setDragEnabled(true);

            quatItem->appendRow(childItem);
          }
        }
        // Create a collapsible list for submessages.
        else
        {
          auto fieldMsg = (ref->MutableMessage(_msg, field));
          auto *childItem = new QStandardItem();
          childItem->setData(name.c_str(), DataRole::DISPLAY_NAME);
          _item->appendRow(childItem);
          this->FillFromMsg(fieldMsg, childItem, _uri + "/" + name);
        }
        break;
      }
      default:
      {
        continue;
      }
    }
  }
}

/////////////////////////////////////////////////
void TopicViewer::UpdateSearch(const QString &_search)
{
  this->dataPtr->searchTopicsModel->SetSearch(_search);

  // Expand / collapse
  this->ExpandChildren(this->dataPtr->searchTopicsModel,
      this->dataPtr->searchTopicsTree, QModelIndex());
}

/////////////////////////////////////////////////
void TopicViewer::ExpandChildren(QSortFilterProxyModel *_model,
    QTreeView *_tree, const QModelIndex &_srcParent) const
{
  if (!_model || !_tree)
    return;

  for (int i = 0; i < _model->rowCount(_srcParent); ++i)
  {
    auto item = _model->index(i, 0, _srcParent);
    if (!item.isValid())
      return;

    bool expand = _model->data(item, DataRole::TO_EXPAND).toBool();

    _tree->setExpanded(item, expand);

    this->ExpandChildren(_model, _tree, item);
  }
}

/////////////////////////////////////////////////
void TopicViewer::ExpandTree(const QModelIndex &_index)
{
  auto tree = qobject_cast<QTreeView *>(QObject::sender());

  if (!tree)
    return;

  tree->setExpanded(_index, !tree->isExpanded(_index));
}

// Register this plugin.
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::TopicViewer,
                                  ignition::gui::Plugin)
