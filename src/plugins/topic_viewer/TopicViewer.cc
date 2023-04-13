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

#include <QModelIndex>
#include <QStandardItem>
#include <QString>

#include <deque>
#include <map>
#include <string>
#include <vector>

#include <gz/common/Console.hh>
#include <gz/gui/Application.hh>
#include <gz/plugin/Register.hh>
#include <gz/msgs/Factory.hh>
#include <gz/transport/MessageInfo.hh>
#include <gz/transport/Node.hh>
#include <gz/transport/Publisher.hh>

#include "TopicViewer.hh"

#define NAME_KEY "name"
#define TYPE_KEY "type"
#define TOPIC_KEY "topic"
#define PATH_KEY "path"
#define PLOT_KEY "plottable"

#define NAME_ROLE 51
#define TYPE_ROLE 52
#define TOPIC_ROLE 53
#define PATH_ROLE 54
#define PLOT_ROLE 55

namespace gz
{
namespace gui
{
namespace plugins
{
  /// \brief Model for the Topics and their Msgs and Fields
  /// a tree model that represents the topics tree with its Msgs
  /// Childeren and each msg node has its own fileds/msgs childeren
  class TopicsModel : public QStandardItemModel
  {
    /// \brief roles and names of the model
    public: QHash<int, QByteArray> roleNames() const override
    {
      QHash<int, QByteArray> roles;
      roles[NAME_ROLE] = NAME_KEY;
      roles[TYPE_ROLE] = TYPE_KEY;
      roles[TOPIC_ROLE] = TOPIC_KEY;
      roles[PATH_ROLE] = PATH_KEY;
      roles[PLOT_ROLE] = PLOT_KEY;
      return roles;
    }
  };

  class TopicViewerPrivate
  {
    /// \brief Node for Commincation
    public: gz::transport::Node node;

    /// \brief Model to create it from the available topics and messages
    public: TopicsModel *model;

    /// \brief Timer to update the model and keep track of its changes
    public: QTimer *timer;

    /// \brief topic: msgType map to keep track of the model current topics
    public: std::map<std::string, std::string> currentTopics;

    /// \brief Create the fields model
    public: void CreateModel();

    /// \brief add a topic to the model
    /// \param[in] _topic topic name to be displayed
    /// \param[in] _msg topic's msg type
    public: void AddTopic(const std::string &_topic,
                         const std::string &_msg);

    /// \brief add a field/msg child to that parent item
    /// \param[in] _parentItem a parent for the added field/msg
    /// \param[in] _msgName the displayed name of the field/msg
    /// \param[in] _msgType field/msg type
    public: void AddField(QStandardItem *_parentItem,
                       const std::string &_msgName,
                       const std::string &_msgType);

    /// \brief factory method for creating an item
    /// \param[in] _name the display name
    /// \param[in] _type type of the field of the item
    /// \param[in] _path a set of concatenate strings of parent msgs
    /// names that lead to that field, starting from the most parent
    /// ex : if we have [Collision]msg contains [pose]msg contains [position]
    /// msg contains [x,y,z] fields, so the path of x = "pose-position-x"
    /// \param[in] _topic the name of the most parent item
    /// \return the created Item
    public: QStandardItem *FactoryItem(const std::string &_name,
                                      const std::string &_type,
                                      const std::string &_path = "",
                                      const std::string &_topic = "");

    /// \brief set the topic role name of the item with the most
    /// topic parent of that field item
    /// \param[in] _item item ref to set its topic
    public: void SetItemTopic(QStandardItem *_item);

    /// \brief set the path/ID of the givin item starting from
    /// the most topic parent to the field itself
    /// \param[in] _item item ref to set its path
    public: void SetItemPath(QStandardItem *_item);

    /// \brief get the topic name of selected item
    /// \param[in] _item ref to the item to get its parent topic
    public: std::string TopicName(const QStandardItem *_item) const;

    /// \brief full path starting from topic name till the msg name
    /// \param[in] _index index of the QStanadardItem
    /// \return string with all elements separated by '/'
    public: std::string ItemPath(const QStandardItem *_item) const;

    /// \brief check if the type is supported in the plotting types
    /// \param[in] _type the msg type to check if it is supported
    public: bool IsPlotable(
            const google::protobuf::FieldDescriptor::Type &_type);

    /// \brief supported types for plotting
    public: std::vector<google::protobuf::FieldDescriptor::Type> plotableTypes;
  };
}
}
}

using namespace gz;
using namespace gui;
using namespace plugins;

TopicViewer::TopicViewer() : Plugin(), dataPtr(new TopicViewerPrivate)
{
  using namespace google::protobuf;
  this->dataPtr->plotableTypes.push_back(FieldDescriptor::Type::TYPE_DOUBLE);
  this->dataPtr->plotableTypes.push_back(FieldDescriptor::Type::TYPE_FLOAT);
  this->dataPtr->plotableTypes.push_back(FieldDescriptor::Type::TYPE_INT32);
  this->dataPtr->plotableTypes.push_back(FieldDescriptor::Type::TYPE_INT64);
  this->dataPtr->plotableTypes.push_back(FieldDescriptor::Type::TYPE_UINT32);
  this->dataPtr->plotableTypes.push_back(FieldDescriptor::Type::TYPE_UINT64);
  this->dataPtr->plotableTypes.push_back(FieldDescriptor::Type::TYPE_BOOL);

  this->dataPtr->CreateModel();

  gui::App()->Engine()->rootContext()->setContextProperty(
                "TopicsModel", this->dataPtr->model);

  this->dataPtr->timer = new QTimer();
  connect(this->dataPtr->timer, SIGNAL(timeout()), this, SLOT(UpdateModel()));
  this->dataPtr->timer->start(1000);
}

//////////////////////////////////////////////////
TopicViewer::~TopicViewer()
{
}

//////////////////////////////////////////////////
void TopicViewer::LoadConfig(const tinyxml2::XMLElement *)
{
  if (this->title.empty())
    this->title = "Topic Viewer";
}

//////////////////////////////////////////////////
QStandardItemModel *TopicViewer::Model()
{
  return reinterpret_cast<QStandardItemModel *>(this->dataPtr->model);
}

//////////////////////////////////////////////////
void TopicViewerPrivate::CreateModel()
{
  this->model = new TopicsModel();

  std::vector<std::string> topics;
  this->node.TopicList(topics);

  for (unsigned int i = 0; i < topics.size(); ++i)
  {
    std::vector<transport::MessagePublisher> infoMsgs;
    this->node.TopicInfo(topics[i], infoMsgs);
    std::string msgType = infoMsgs[0].MsgTypeName();
    this->AddTopic(topics[i], msgType);
  }
}

//////////////////////////////////////////////////
void TopicViewerPrivate::AddTopic(const std::string &_topic,
                           const std::string &_msg)
{
  QStandardItem *topicItem = this->FactoryItem(_topic, _msg);
  topicItem->setWhatsThis("Topic");
  QStandardItem *parent = this->model->invisibleRootItem();
  parent->appendRow(topicItem);

  this->AddField(topicItem , _msg, _msg);

  // store the topics to keep track of them
  this->currentTopics[_topic] = _msg;
}

//////////////////////////////////////////////////
void TopicViewerPrivate::AddField(QStandardItem *_parentItem,
                           const std::string &_msgName,
                           const std::string &_msgType)
{
  QStandardItem *msgItem;

  // check if it is a topic, to skip the extra level of the topic Msg
  if (_parentItem->whatsThis() == "Topic")
  {
    msgItem = _parentItem;
    // make it different, so next iteration will make a new msg item
    msgItem->setWhatsThis("Msg");
  }
  else
  {
    msgItem = this->FactoryItem(_msgName, _msgType);
    _parentItem->appendRow(msgItem);
  }

  auto msg = msgs::Factory::New(_msgType);
  if (!msg)
  {
      gzwarn << "Null Msg: " << _msgType << std::endl;
      return;
  }

  auto msgDescriptor = msg->GetDescriptor();
  if (!msgDescriptor)
  {
    gzwarn << "Null Descriptor of Msg: " << _msgType << std::endl;
    return;
  }

  for (int i = 0 ; i < msgDescriptor->field_count(); ++i)
  {
    auto msgField = msgDescriptor->field(i);

    if (msgField->is_repeated())
      continue;

    auto messageType = msgField->message_type();

    if (messageType)
      this->AddField(msgItem, msgField->name(), messageType->name());

    else
    {
      auto msgFieldItem = this->FactoryItem(msgField->name(),
                                            msgField->type_name());
      msgItem->appendRow(msgFieldItem);

      this->SetItemPath(msgFieldItem);
      this->SetItemTopic(msgFieldItem);

      // to make the plottable items draggable
      if (this->IsPlotable(msgField->type()))
        msgFieldItem->setData(QVariant(true), PLOT_ROLE);
    }
  }
}

//////////////////////////////////////////////////
QStandardItem *TopicViewerPrivate::FactoryItem(const std::string &_name,
                                               const std::string &_type,
                                               const std::string &_path,
                                               const std::string &_topic)
{
  QString name = QString::fromStdString(_name);
  QString type = QString::fromStdString(_type);
  QString path = QString::fromStdString(_path);
  QString topic = QString::fromStdString(_topic);

  QStandardItem *item = new QStandardItem(name);

  item->setData(QVariant(name), NAME_ROLE);
  item->setData(QVariant(type), TYPE_ROLE);
  item->setData(QVariant(path), PATH_ROLE);
  item->setData(QVariant(topic), TOPIC_ROLE);
  item->setData(QVariant(false), PLOT_ROLE);

  return item;
}

//////////////////////////////////////////////////
void TopicViewerPrivate::SetItemTopic(QStandardItem *_item)
{
  std::string topic = this->TopicName(_item);
  QVariant Topic(QString::fromStdString(topic));
  _item->setData(Topic, TOPIC_ROLE);
}

//////////////////////////////////////////////////
void TopicViewerPrivate::SetItemPath(QStandardItem *_item)
{
  std::string path = this->ItemPath(_item);
  QVariant Path(QString::fromStdString(path));
  _item->setData(Path, PATH_ROLE);
}

//////////////////////////////////////////////////
std::string TopicViewerPrivate::TopicName(const QStandardItem *_item) const
{
  QStandardItem *parent = _item->parent();

  // get the next parent until you reach the first level parent
  while (parent)
  {
    _item = parent;
    parent = parent->parent();
  }

  return _item->data(NAME_ROLE).toString().toStdString();
}

//////////////////////////////////////////////////
std::string TopicViewerPrivate::ItemPath(const QStandardItem *_item) const
{
  std::deque<std::string> path;
  while (_item)
  {
    path.push_front(_item->data(NAME_ROLE).toString().toStdString());
    _item = _item->parent();
  }

  if (path.size())
    path.erase(path.begin());

  // convert to string
  std::string pathString;

  for (unsigned int i = 0; i < path.size()-1; ++i)
    pathString += path[i] + "-";

  if (path.size())
    pathString += path[path.size()-1];

  return pathString;
}

/////////////////////////////////////////////////
bool TopicViewerPrivate::IsPlotable(
    const google::protobuf::FieldDescriptor::Type &_type)
{
  return std::find(this->plotableTypes.begin(), this->plotableTypes.end(),
                   _type) != this->plotableTypes.end();
}

/////////////////////////////////////////////////
void TopicViewer::UpdateModel()
{
  // get the current topics in the network
  std::vector<std::string> topics;
  this->dataPtr->node.TopicList(topics);

  // initialize the topics with the old topics & remove every matched topic
  // when you finish advertised topics the remaining topics will be removed
  std::map<std::string, std::string> topicsToRemove =
          this->dataPtr->currentTopics;

  for (unsigned int i = 0; i < topics.size(); ++i)
  {
    // get the msg type
    std::vector<transport::MessagePublisher> infoMsgs;
    this->dataPtr->node.TopicInfo(topics[i], infoMsgs);
    std::string msgType = infoMsgs[0].MsgTypeName();

    // skip the matched topics
    if (this->dataPtr->currentTopics.count(topics[i]) &&
            this->dataPtr->currentTopics[topics[i]] == msgType)
    {
      topicsToRemove.erase(topics[i]);
      continue;
    }

    // new topic
    this->dataPtr->AddTopic(topics[i], msgType);
  }

  // remove the topics that don't exist in the network
  for (auto topic : topicsToRemove)
  {
    auto root = this->dataPtr->model->invisibleRootItem();

    // search for the topic in the model
    for (int i = 0; i < root->rowCount(); ++i)
    {
      auto child = root->child(i);

      if (child->data(NAME_ROLE).toString().toStdString() == topic.first &&
              child->data(TYPE_ROLE).toString().toStdString() == topic.second)
      {
        // remove from model
        root->removeRow(i);
        // remove from topics as it is a dangling topic
        this->dataPtr->currentTopics.erase(topic.first);
        break;
      }
    }
  }
}


// Register this plugin
GZ_ADD_PLUGIN(TopicViewer,
              gui::Plugin)
