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

#ifndef IGNITION_GUI_PLUGINS_RESPONDER_HH_
#define IGNITION_GUI_PLUGINS_RESPONDER_HH_

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <google/protobuf/message.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <memory>

#include "ignition/gui/qt.h"
#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class ResponderPrivate;

  /// \brief Advertises a service which responds with a custom message.
  class Responder : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: Responder();

    /// \brief Destructor
    public: virtual ~Responder();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Callback when serve button is pressed.
    public slots: void OnServe();

    /// \brief Callback when stop button is pressed.
    public slots: void OnStop();

    /// \brief Update request text
    /// \param[in] _req Request string
    private: void UpdateRequest(QString _req);

    /// \brief Service callback
    /// \param[in] _req Request protobuf message
    /// \param[out] _res Response protobuf message
    /// \param[out] _success True if successful
    template<typename REQ, typename RES>
    void Callback(const REQ &_req, RES &_res, bool &_success)
    {
      this->UpdateRequest(QString::fromStdString(_req.DebugString()));
      _res.CopyFrom(*this->res);
      _success = true;
    }

    /// \brief Holds user defined response
    private: std::unique_ptr<google::protobuf::Message> res;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<ResponderPrivate> dataPtr;
  };
}
}
}

#endif
