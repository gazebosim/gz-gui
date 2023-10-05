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

#include <sstream>
#include <gz/common/Console.hh>
#include <gz/common/StringUtils.hh>
#include <gz/transport/Node.hh>
#include <gz/transport/MessageInfo.hh>
#include <gz/transport/Publisher.hh>

#include "gz/gui/PlottingInterface.hh"
#include "gz/gui/Application.hh"

#define DEFAULT_TIME (INT_MIN)
// 1/60 Period like the GuiSystem frequency (60Hz)
#define MAX_PERIOD_DIFF (0.0166666667)

namespace gz::gui
{
class PlotDataPrivate
{
  /// \brief Value of that field
  public: double value;

  /// \brief arrival time (header time)
  public: double time = DEFAULT_TIME;

  /// \brief Registered Charts to that field
  public: std::set<int> charts;
};


class TopicPrivate
{
  /// \brief Check the plotable types and get data from reflection
  /// \param[in] _msg Message to get data from
  /// \param[in] _field Field within the message to get
  /// \return Plottable value as double, zero if not plottable
  public: double FieldData(const google::protobuf::Message &_msg,
                           const google::protobuf::FieldDescriptor *_field);

  /// \brief Topic name
  public: std::string name;

  /// \brief Default Plotting time
  public: std::shared_ptr<double> plottingTime;

  /// \brief Previous header time to limit the frequency of publishing
  public: double lastHeaderTime = 0;

  /// \brief Plotting fields to update its values
  public: std::map<std::string, gz::gui::PlotData*> fields;
};

class TransportPrivate
{
  /// \brief Node for Commincation
  public: gz::transport::Node node;

  /// \brief subscribed topics
  public: std::map<std::string, gz::gui::Topic*> topics;
};

class PlottingIfacePrivate
{
  /// \brief Responsible for transport messages and topics
  public: Transport transport;

  /// \brief Plotting time pointer to give access to topics to read it
  public: std::shared_ptr<double> plottingTimeRef = std::make_shared<double>();

  /// \brief timeout to update the plot with the timer
  public: int timeout;

  /// \brief timer to update the plotting each time step
  public: QTimer timer;
};

//////////////////////////////////////////////////////
PlotData::PlotData() :
    dataPtr(std::make_unique<PlotDataPrivate>())
{
  this->dataPtr->value = 0;
}

//////////////////////////////////////////////////////
PlotData::~PlotData()
{
}

//////////////////////////////////////////////////////
void PlotData::SetValue(const double _value)
{
  this->dataPtr->value = _value;
}

//////////////////////////////////////////////////////
double PlotData::Value() const
{
  return this->dataPtr->value;
}

//////////////////////////////////////////////////////
void PlotData::SetTime(const double _time)
{
  this->dataPtr->time = _time;
}

//////////////////////////////////////////////////////
double PlotData::Time() const
{
  return this->dataPtr->time;
}

//////////////////////////////////////////////////////
void PlotData::AddChart(int _chart)
{
  this->dataPtr->charts.insert(_chart);
}

//////////////////////////////////////////////////////
void PlotData::RemoveChart(int _chart)
{
  auto chartIt = this->dataPtr->charts.find(_chart);
  if (chartIt != this->dataPtr->charts.end())
    this->dataPtr->charts.erase(chartIt);
}

//////////////////////////////////////////////////////
int PlotData::ChartCount() const
{
  return this->dataPtr->charts.size();
}

//////////////////////////////////////////////////////
std::set<int> &PlotData::Charts()
{
  return this->dataPtr->charts;
}

//////////////////////////////////////////////////////
Topic::Topic(const std::string &_name) : QObject(),
    dataPtr(std::make_unique<TopicPrivate>())
{
  this->dataPtr->name = _name;
}

//////////////////////////////////////////////////////
Topic::~Topic()
{
  for (auto field : this->dataPtr->fields)
    delete field.second;
}

//////////////////////////////////////////////////////
std::string &Topic::Name() const
{
  return this->dataPtr->name;
}

//////////////////////////////////////////////////////
void Topic::Register(const std::string &_fieldPath, int _chart)
{
  // if a new field create a new field and register the chart
  if (this->dataPtr->fields.count(_fieldPath) == 0)
    this->dataPtr->fields[_fieldPath] = new PlotData();

  this->dataPtr->fields[_fieldPath]->AddChart(_chart);
}

//////////////////////////////////////////////////////
void Topic::UnRegister(const std::string &_fieldPath, int _chart)
{
  this->dataPtr->fields[_fieldPath]->RemoveChart(_chart);

  // if no one registers to the field, remove it
  if (!this->dataPtr->fields[_fieldPath]->ChartCount())
    this->dataPtr->fields.erase(_fieldPath);
}

//////////////////////////////////////////////////////
int Topic::FieldCount() const
{
  return this->dataPtr->fields.size();
}

//////////////////////////////////////////////////////
std::map<std::string, PlotData*> &Topic::Fields()
{
  return this->dataPtr->fields;
}

//////////////////////////////////////////////////////
void Topic::Callback(const google::protobuf::Message &_msg)
{
  // check for header time
  double headerTime;
  if (!this->HasHeader(_msg, headerTime))
  {
    if (!this->dataPtr->plottingTime)
        return;

    headerTime = DEFAULT_TIME;

    if (*this->dataPtr->plottingTime - this->dataPtr->lastHeaderTime
          < MAX_PERIOD_DIFF)
      return;

    this->dataPtr->lastHeaderTime = *this->dataPtr->plottingTime;
  }
  else
  {
    if (headerTime - this->dataPtr->lastHeaderTime < MAX_PERIOD_DIFF)
        return;

    this->dataPtr->lastHeaderTime = headerTime;
  }

  // loop over the registered fields and update them
  for (auto fieldIt : this->dataPtr->fields)
  {
    auto msgDescriptor = _msg.GetDescriptor();
    auto ref = _msg.GetReflection();

    google::protobuf::Message *valueMsg = nullptr;

    auto fieldFullPath = gz::common::Split(fieldIt.first, '-');
    int pathSize = fieldFullPath.size();

    // loop until you reach the last field in the path
    for (int i = 0; i < pathSize-1 ; i++)
    {
      std::string fieldName = fieldFullPath[i];

      auto field = msgDescriptor->FindFieldByName(fieldName);

      msgDescriptor = field->message_type();

      if (valueMsg)
      {
        valueMsg = ref->MutableMessage
                (const_cast<google::protobuf::Message *>(valueMsg), field);
      }
      else
      {
        valueMsg = ref->MutableMessage
                (const_cast<google::protobuf::Message *>(&_msg), field);
      }

      if (valueMsg)
        ref = valueMsg->GetReflection();
      else
      {
        gzwarn << "Invalid topic msg" << std::endl;
        return;
      }
    }

    std::string fieldName = fieldFullPath[pathSize-1];
    double data;

    if (valueMsg)
    {
      auto field = valueMsg->GetDescriptor()->FindFieldByName(fieldName);
      data = this->dataPtr->FieldData(*valueMsg, field);
    }
    else
    {
      auto field = msgDescriptor->FindFieldByName(fieldName);
      data = this->dataPtr->FieldData(_msg, field);
    }

    if (!fieldIt.second)
      continue;

    // Field Arrival Time
    fieldIt.second->SetTime(headerTime);

    // Field Value
    fieldIt.second->SetValue(data);

    // Update Field Charts UI
    this->UpdateGui(fieldIt.first);
  }
}

//////////////////////////////////////////////////////
bool Topic::HasHeader(const google::protobuf::Message &_msg,
                      double &_headerTime)
{
  auto ref = _msg.GetReflection();
  auto header = _msg.GetDescriptor()->FindFieldByName("header");
  auto found = ref->HasField(_msg, header);

  if (!found)
    return false;

  auto stamp = header->message_type()->FindFieldByName("stamp");

  if (!stamp)
    return false;

  auto headerMsg = ref->MutableMessage
          (const_cast<google::protobuf::Message *>(&_msg), header);

  if (!headerMsg)
    return false;

  ref = headerMsg->GetReflection();

  auto stampMsg = ref->MutableMessage
          (const_cast<google::protobuf::Message *>(headerMsg), stamp);

  if (!stampMsg)
    return false;

  auto secField = stamp->message_type()->FindFieldByName("sec");
  auto nsecField = stamp->message_type()->FindFieldByName("nsec");

  auto sec = this->dataPtr->FieldData(*stampMsg, secField);
  auto nsec = this->dataPtr->FieldData(*stampMsg, nsecField);

  _headerTime = sec + nsec * std::pow(10, -9);

  return true;
}

//////////////////////////////////////////////////////
void Topic::UpdateGui(const std::string &_field)
{
  auto field = this->dataPtr->fields[_field];

  auto x = field->Time();
  auto y = field->Value();

  QString fieldFullPath = QString::fromStdString
          (this->dataPtr->name + "-" + _field);

  auto charts = field->Charts();

  for (auto const &chart : charts)
    emit plot(chart, fieldFullPath, x, y);
}

//////////////////////////////////////////////////////
void Topic::SetPlottingTimeRef(const std::shared_ptr<double> &_timeRef)
{
  if (!this->dataPtr->plottingTime)
    this->dataPtr->plottingTime = _timeRef;
}

//////////////////////////////////////////////////////
double TopicPrivate::FieldData(const google::protobuf::Message &_msg,
                               const google::protobuf::FieldDescriptor *_field)
{
  using namespace google::protobuf;
  auto ref = _msg.GetReflection();
  auto type = _field->type();

  if (type == FieldDescriptor::Type::TYPE_DOUBLE)
    return ref->GetDouble(_msg, _field);
  else if (type == FieldDescriptor::Type::TYPE_FLOAT)
    return ref->GetFloat(_msg, _field);
  else if (type == FieldDescriptor::Type::TYPE_INT32)
    return ref->GetInt32(_msg, _field);
  else if (type == FieldDescriptor::Type::TYPE_INT64)
    return ref->GetInt64(_msg, _field);
  else if (type == FieldDescriptor::Type::TYPE_BOOL)
    return ref->GetBool(_msg, _field);
  else if (type == FieldDescriptor::Type::TYPE_UINT32)
    return ref->GetUInt32(_msg, _field);
  else if (type == FieldDescriptor::Type::TYPE_UINT64)
    return ref->GetUInt64(_msg, _field);
  else
  {
    gzwarn << "Non Plotting Type" << std::endl;
    return 0;
  }
}

////////////////////////////////////////////
Transport::Transport() : dataPtr(std::make_unique<TransportPrivate>())
{
}

////////////////////////////////////////////
Transport::~Transport()
{
  // unsubscribe from all topics in the transport
  for (auto topic : this->dataPtr->topics)
    this->dataPtr->node.Unsubscribe(topic.first);
}

////////////////////////////////////////////
void Transport::Unsubscribe(const std::string &_topic,
                            const std::string &_fieldPath,
                            int _chart)
{
  if (this->dataPtr->topics.count(_topic))
  {
    this->dataPtr->topics[_topic]->UnRegister(_fieldPath, _chart);

    // if there is no registered fields, unsubscribe from the topic
    if (this->dataPtr->topics[_topic]->FieldCount() == 0)
    {
      this->dataPtr->node.Unsubscribe(_topic);
      this->dataPtr->topics.erase(_topic);
    }
  }
}

////////////////////////////////////////////
void Transport::Subscribe(const std::string &_topic,
                          const std::string &_fieldPath,
                          int _chart, const std::shared_ptr<double> &_time)
{
  // new topic
  if (this->dataPtr->topics.count(_topic) == 0)
  {
    auto topicHandler = new Topic(_topic);
    this->dataPtr->topics[_topic] = topicHandler;

    topicHandler->Register(_fieldPath, _chart);
    this->dataPtr->node.Subscribe(_topic, &Topic::Callback, topicHandler);

    topicHandler->SetPlottingTimeRef(_time);

    connect(topicHandler, SIGNAL(plot(int, QString, double, double)),
            this, SLOT(onPlot(int, QString, double, double)));
  }
  // already exist topic
  else
  {
    this->dataPtr->topics[_topic]->Register(_fieldPath, _chart);
    this->dataPtr->node.Subscribe(_topic, &Topic::Callback,
                                  this->dataPtr->topics[_topic]);
  }
}

//////////////////////////////////////////////////////
const std::map<std::string, Topic*> &Transport::Topics()
{
  return this->dataPtr->topics;
}

//////////////////////////////////////////////////////
void Transport::onPlot(int _chart, QString _fieldID, double _x, double _y)
{
  emit this->plot(_chart, _fieldID, _x, _y);
}

//////////////////////////////////////////////////////
void Transport::UnsubscribeOutdatedTopics()
{
  // get all topics in the transport
  std::vector<std::string> topics;
  this->dataPtr->node.TopicList(topics);

  for (auto topic : this->dataPtr->topics)
  {
    // check if the topic exist
    if (std::find(topics.begin(), topics.end(), topic.first) == topics.end())
    {
      this->dataPtr->node.Unsubscribe(topic.first);
      delete topic.second;
      this->dataPtr->topics.erase(topic.first);
    }
  }
}

//////////////////////////////////////////////////////
PlottingInterface::PlottingInterface() : QObject(),
    dataPtr(std::make_unique<PlottingIfacePrivate>())
{
  connect(&this->dataPtr->transport,
          SIGNAL(plot(int, QString, double, double)), this,
          SLOT(onPlot(int, QString, double, double)));

  this->dataPtr->timeout = 1;
  this->InitTimer();

  App()->Engine()->rootContext()->setContextProperty("PlottingIface", this);
}

//////////////////////////////////////////////////////
PlottingInterface::~PlottingInterface()
{
}

//////////////////////////////////////////////////////
void PlottingInterface::unsubscribe(int _chart,
                                    QString _topic,
                                    QString _fieldPath)
{
  this->dataPtr->transport.Unsubscribe(_topic.toStdString(),
                                       _fieldPath.toStdString(),
                                       _chart);
}

//////////////////////////////////////////////////////
float PlottingInterface::Timeout() const
{
  return this->dataPtr->timer.interval();
}

//////////////////////////////////////////////////////
void PlottingInterface::onComponentSubscribe(QString _entity, QString _typeId,
                                             QString _type, QString _attribute,
                                             int _chart)
{
  // convert the strings into
  uint64_t entity, typeId;
  std::istringstream issEntity(_entity.toStdString());
  issEntity >> entity;
  std::istringstream issTypeId(_typeId.toStdString());
  issTypeId >> typeId;

  emit this->ComponentSubscribe(entity, typeId, _type.toStdString(),
                                _attribute.toStdString(), _chart);
}

//////////////////////////////////////////////////////
void PlottingInterface::onComponentUnSubscribe(QString _entity, QString _typeId,
                                               QString _attribute, int _chart)
{
  // convert the strings into
  uint64_t entity, typeId;
  std::istringstream issEntity(_entity.toStdString());
  issEntity >> entity;
  std::istringstream issTypeId(_typeId.toStdString());
  issTypeId >> typeId;

  emit this->ComponentUnSubscribe(entity, typeId,
                                  _attribute.toStdString(), _chart);
}

//////////////////////////////////////////////////////
void PlottingInterface::subscribe(int _chart,
                                  QString _topic,
                                  QString _fieldPath)
{
  this->dataPtr->transport.Subscribe(_topic.toStdString(),
                                     _fieldPath.toStdString(),
                                     _chart, this->dataPtr->plottingTimeRef);
}

////////////////////////////////////////////
void PlottingInterface::InitTimer()
{
  this->dataPtr->timer.setInterval(this->dataPtr->timeout);
  connect(&this->dataPtr->timer, SIGNAL(timeout()), this, SLOT(UpdateTime()));
  this->dataPtr->timer.start();
}

//////////////////////////////////////////////////////
void PlottingInterface::onPlot(int _chart, QString _fieldID,
                               double _x, double _y)
{
  // if _x == -1, then the msg has not header time
  // so update x with the default plotting time that is handled by a timer
  if (static_cast<int>(_x) == DEFAULT_TIME)
      _x = *this->dataPtr->plottingTimeRef;

  emit this->plot(_chart, _fieldID, _x, _y);
}

//////////////////////////////////////////////////////
void PlottingInterface::UpdateTime()
{
  *this->dataPtr->plottingTimeRef += this->dataPtr->timeout * 0.001;
}

//////////////////////////////////////////////////////
std::string PlottingInterface::FilePath(QString _path, std::string _name,
                                        std::string _extention)
{
  if (_extention != "csv" && _extention != "pdf")
    return "";

  if (_path.toStdString().size() < 8)
  {
    gzwarn << "Couldn't parse file path" << std::endl;
    return "";
  }
  else
    // remove "file://" at the begin of the path
    _path.remove(0, 7);

  std::replace(_name.begin(), _name.end(), '/', '_');
  std::replace(_name.begin(), _name.end(), '-', '_');
  std::replace(_name.begin(), _name.end(), ',', '_');

  return _path.toStdString() + "/" + "\'" + _name + "." + _extention + "\'";
}

//////////////////////////////////////////////////////
bool PlottingInterface::exportCSV(QString _path, int _chart,
                                  QMap< QString, QVariant> _serieses)
{
  std::string plotName = "Plot" + std::to_string(_chart);

  std::ofstream file;

  QMap<QString, QVariant>::const_iterator series = _serieses.constBegin();
  while (series != _serieses.constEnd())
  {
    auto key = series.key().toStdString();

    // check if it is a component
    auto seriesKeys = gz::common::Split(key, ',');
    if (seriesKeys.size() == 3)
    {
      // convert from string to uint64_t
      uint64_t typeId;
      std::string typeIdString = seriesKeys[1];
      std::istringstream issTypeId(typeIdString);
      issTypeId >> typeId;

      // replace the typeId num with the type name
      auto typeName = emit ComponentName(typeId);
      seriesKeys[1] = typeName;

      // make the new series key
      key = seriesKeys[0] + "_" + seriesKeys[1] + "_" + seriesKeys[2];
    }
    // if Field
    else
      std::replace(key.begin(), key.end(), '-', '/');

    auto name = plotName +  "_" + key;

    auto filePath = this->FilePath(_path , name, "csv");

    if (!filePath.size())
    {
        gzwarn << "[Couldn't parse file: " << filePath << "]" << std::endl;
        return false;
    }

    file.open(filePath);
    if (!file.is_open())
        gzwarn << "[Couldn't open file: " << filePath << "]" << std::endl;

    file << "time, " << key << std::endl;

    auto points = series.value().toList();
    for (int j = 0 ; j < points.size(); j++)
    {
        auto point = points.at(j).toPointF();
        file << point.x() << ", " << point.y() << std::endl;
    }

    file.close();
    ++series;
  }
  return true;
}
}  // namespace gz::gui
