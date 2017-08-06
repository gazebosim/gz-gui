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

#include <algorithm>
#include <mutex>
#include <string>
#include <vector>
#include <google/protobuf/message.h>
#include <ignition/common/PluginMacros.hh>
#include <ignition/transport/Node.hh>

#include "ignition/common/URI.hh"
#include "ignition/gui/plugins/TopicViewer.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
/// \brief Delegate that handles drawing the topic tree
class PlotItemDelegate : public QStyledItemDelegate
{
  /// \brief The data roles
  public: enum DataRole
  {
    /// \brief Text which will be displayed for the user.
    DISPLAY_NAME = Qt::UserRole + 100,

    /// \brief URI including detailed query about a single plot value. This is
    /// the information carried during a drag-drop operation.
    URI_QUERY,

    /// \brief Data type name, such as "Double" or "Bool", used to display type
    /// information to the user. Or something like "model", "link", used to
    /// choose icons.
    TYPE,

    /// \brief Flag indicating whether to expand the item or not.
    TO_EXPAND
  };

  /// \brief Constructor
  public: PlotItemDelegate() = default;

  /// \brief Destructor
  public: virtual ~PlotItemDelegate() = default;

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
      // FIXME: Find a non-hardcoded way to get the bg color.
      QBrush brush(QColor("#e2e2e2"));
      _painter->save();
      _painter->fillRect(titleRect, brush);
      _painter->restore();
    }

    // Handle hover style.
    if (typeName != "title" && _opt.state & QStyle::State_MouseOver)
    {
      _painter->setPen(QPen(QColor(200, 200, 200, 0), 0));
      _painter->setBrush(QColor(200, 200, 200));
      _painter->drawRect(_opt.rect);
    }

    // Paint the type icon.
    if (typeName == "model" || typeName == "link" || typeName == "collision" ||
        typeName == "visual" || typeName == "joint")
    {
      double iconSize = 15;

      textRect.adjust(iconSize + 6, 5, 0, -5);
      QRectF iconRect = _opt.rect;
      iconRect.setTop(iconRect.top() + (_opt.rect.height()/2.0 - iconSize/2.0));

      QIcon icon(":/images/" + typeName  + ".svg");
      _painter->drawPixmap(iconRect.left(), iconRect.top(),
          icon.pixmap(iconSize, iconSize));
    }
    // Titles.
    else if (typeName == "title")
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

    _painter->setPen(QColor(30, 30, 30));

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
/// Customize the item model so that we can pass along the correct MIME
/// information during a drag-drop.
class PlotItemModel : public QStandardItemModel
{
  /////////////////////////////////////////////////
  /// \brief Custom MIME data function.
  /// \param[in] _indexes List of selected items.
  /// \return Mime data for the selected items.
  public: QMimeData *mimeData(const QModelIndexList &_indexes) const
  {
    QMimeData *curMimeData = new QMimeData();

    for (auto const &idx : _indexes)
    {
      if (idx.isValid())
      {
        QString text = this->data(idx,
            PlotItemDelegate::URI_QUERY).toString();
        curMimeData->setData("application/x-item", text.toLatin1().data());

        break;
      }
    }

    return curMimeData;
  }
};

/////////////////////////////////////////////////
std::string humanReadableKey(const std::string &_key)
{
  std::string humanKey = _key;
  humanKey[0] = std::toupper(humanKey[0]);
  std::replace(humanKey.begin(), humanKey.end(), '_', ' ');
  return humanKey;
}

/////////////////////////////////////////////////
bool SearchModel::filterAcceptsRow(const int _srcRow,
      const QModelIndex &_srcParent) const
{
  // Item index in search model.
  auto id = this->sourceModel()->index(_srcRow, 0, _srcParent);

  // Ignore titles.
  if (this->sourceModel()->data(id, PlotItemDelegate::TYPE).toString() ==
      "title")
  {
    return false;
  }

  // Collapsed by default.
  this->sourceModel()->blockSignals(true);
  this->sourceModel()->setData(id, false, PlotItemDelegate::TO_EXPAND);
  this->sourceModel()->blockSignals(false);

  // Empty search matches everything.
  if (this->search.isEmpty())
    return true;

  // Each word must match at least once, either self, parent or child.
  auto words = this->search.split(" ");
  for (auto word : words)
  {
    if (word.isEmpty())
      continue;

    // Expand this if at least one child contains the word.
    // Note that this is not enough for this to be accepted, we need to match
    // all words.
    if (this->hasChildAcceptsItself(id, word))
    {
      this->sourceModel()->blockSignals(true);
      this->sourceModel()->setData(id, true, PlotItemDelegate::TO_EXPAND);
      this->sourceModel()->blockSignals(false);
    }

    // At least one of the children fits rule 1.
    if (this->hasAcceptedChildren(_srcRow, _srcParent))
      continue;

    // Row itself contains this word.
    if (this->filterAcceptsRowItself(_srcRow, _srcParent, word))
      continue;

    // One of the ancestors contains this word.
    QModelIndex parentIndex = _srcParent;
    bool parentAccepted = false;
    while (parentIndex.isValid())
    {
      if (this->filterAcceptsRowItself(parentIndex.row(),
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
bool SearchModel::filterAcceptsRowItself(const int _srcRow,
    const QModelIndex &_srcParent, const QString _word) const
{
  auto id = this->sourceModel()->index(_srcRow, 0, _srcParent);

  return (this->sourceModel()->data(id,
      this->filterRole()).toString().contains(_word, Qt::CaseInsensitive));
}

/////////////////////////////////////////////////
bool SearchModel::hasAcceptedChildren(const int _srcRow,
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
bool SearchModel::hasChildAcceptsItself(const QModelIndex &_srcParent,
      const QString &_word) const
{
  for (int i = 0; i < this->sourceModel()->rowCount(_srcParent); ++i)
  {
    // Check immediate children.
    if (this->filterAcceptsRowItself(i, _srcParent, _word))
      return true;

    // Check grandchildren.
    auto item = this->sourceModel()->index(i, 0, _srcParent);
    if (this->hasChildAcceptsItself(item, _word))
      return true;
  }

  return false;
}

/////////////////////////////////////////////////
void SearchModel::SetSearch(const QString &_search)
{
  this->search = _search;
  this->filterChanged();
}

/////////////////////////////////////////////////
/// \brief Private data for the TopicViewer class
class ignition::gui::plugins::TopicViewerPrivate
{
  /// \brief Model to hold topics data.
  public: PlotItemModel *topicsModel;

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
  auto plotItemDelegate = new PlotItemDelegate;

  // The model that will hold data to be displayed in the topic tree view.
  this->dataPtr->topicsModel = new PlotItemModel;
  this->dataPtr->topicsModel->setObjectName("plotTopicsModel");
  this->dataPtr->topicsModel->setParent(this);

  // A proxy model to filter topic model.
  this->dataPtr->searchTopicsModel = new SearchModel;
  this->dataPtr->searchTopicsModel->setFilterRole(
      PlotItemDelegate::DISPLAY_NAME);
  this->dataPtr->searchTopicsModel->setSourceModel(this->dataPtr->topicsModel);

  // Search field.
  auto searchIcon = new QLabel();
  searchIcon->setPixmap(QPixmap(":/images/search.svg"));

  auto searchEdit = new QLineEdit();
  searchEdit->setPlaceholderText("Start typing to search...");
  searchEdit->setObjectName("plotLineEdit");
  this->connect(searchEdit, SIGNAL(textChanged(QString)), this,
      SLOT(UpdateSearch(QString)));

  auto searchField = new QHBoxLayout();
  searchField->addWidget(searchIcon);
  searchField->addWidget(searchEdit);

  // A tree to visualize topics search results.
  this->dataPtr->searchTopicsTree = new QTreeView;
  this->dataPtr->searchTopicsTree->setObjectName("plotTree");
  this->dataPtr->searchTopicsTree->setAnimated(true);
  this->dataPtr->searchTopicsTree->setHeaderHidden(true);
  this->dataPtr->searchTopicsTree->setExpandsOnDoubleClick(true);
  this->dataPtr->searchTopicsTree->setModel(this->dataPtr->searchTopicsModel);
  this->dataPtr->searchTopicsTree->setItemDelegate(plotItemDelegate);
  this->dataPtr->searchTopicsTree->setEditTriggers(
      QAbstractItemView::NoEditTriggers);
  this->dataPtr->searchTopicsTree->setDragEnabled(true);
  this->dataPtr->searchTopicsTree->setDragDropMode(QAbstractItemView::DragOnly);
  connect(this->dataPtr->searchTopicsTree, SIGNAL(clicked(const QModelIndex &)),
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
  mainFrame->setObjectName("plotTopicViewerFrame");
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
  connect(timer, SIGNAL(timeout()), this, SLOT(FillTopics()));
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
      topicItem->setData(topic.c_str(), PlotItemDelegate::DISPLAY_NAME);
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
      {
        auto humanName = humanReadableKey(name);

        auto *childItem = new QStandardItem();
        childItem->setData(humanName.c_str(),
            PlotItemDelegate::DISPLAY_NAME);

        switch (field->type())
        {
          case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            childItem->setData("Double", PlotItemDelegate::TYPE);
            break;
          case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            childItem->setData("Float", PlotItemDelegate::TYPE);
            break;
          case google::protobuf::FieldDescriptor::TYPE_INT64:
            childItem->setData("Int 64", PlotItemDelegate::TYPE);
            break;
          case google::protobuf::FieldDescriptor::TYPE_UINT64:
            childItem->setData("Uint 64", PlotItemDelegate::TYPE);
            break;
          case google::protobuf::FieldDescriptor::TYPE_INT32:
            childItem->setData("Int 32", PlotItemDelegate::TYPE);
            break;
          case google::protobuf::FieldDescriptor::TYPE_UINT32:
            childItem->setData("Uint 32", PlotItemDelegate::TYPE);
            break;
          case google::protobuf::FieldDescriptor::TYPE_BOOL:
            childItem->setData("Bool", PlotItemDelegate::TYPE);
            break;
          default:
            continue;
        }
        childItem->setToolTip(
            "<font size=3><p><b>Type</b>: " + childItem->data(
            PlotItemDelegate::TYPE).toString() +
            "</p></font>");

        std::string dataName = _uri + "/" + name;
        childItem->setData(dataName.c_str(), PlotItemDelegate::URI_QUERY);
        childItem->setDragEnabled(true);

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
          childItem->setData(humanName.c_str(),
              PlotItemDelegate::DISPLAY_NAME);
          childItem->setData(dataName.c_str(), PlotItemDelegate::URI_QUERY);
          childItem->setData("Double", PlotItemDelegate::TYPE);
          childItem->setDragEnabled(true);
          childItem->setToolTip(
              "<font size=3><p><b>Type</b>: " + childItem->data(
              PlotItemDelegate::TYPE).toString() +
              "</p></font>");

          _item->appendRow(childItem);
        }
        // Custom RPY widgets for orientation.
        else if (field->message_type()->name() == "Quaternion")
        {
          auto *quatItem = new QStandardItem();
          quatItem->setData(name.c_str(), PlotItemDelegate::DISPLAY_NAME);
          _item->appendRow(quatItem);

          std::vector<std::string> rpy = {"roll", "pitch", "yaw"};
          for (auto it : rpy)
          {
            auto humanName = humanReadableKey(it);
            std::string dataName = _uri + "/" + name + "/" + it;

            auto *childItem = new QStandardItem();
            childItem->setData(QString::fromStdString(humanName),
                PlotItemDelegate::DISPLAY_NAME);
            childItem->setData(dataName.c_str(), PlotItemDelegate::URI_QUERY);
            childItem->setData("Double", PlotItemDelegate::TYPE);
            childItem->setToolTip(
                "<font size=3><p><b>Type</b>: " + childItem->data(
                PlotItemDelegate::TYPE).toString() +
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
          childItem->setData(name.c_str(), PlotItemDelegate::DISPLAY_NAME);
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

    bool expand = _model->data(item,
        PlotItemDelegate::TO_EXPAND).toBool();

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
