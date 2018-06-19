/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#include <ignition/common/Console.hh>
#include "ignition/gui/Application.hh"

namespace ignition
{
  namespace gui
  {
    class ApplicationPrivate
    {
      /// \brief QML engine
      public: QQmlApplicationEngine *engine{nullptr};
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
Application::Application(int &_argc, char **_argv)
  : QGuiApplication(_argc, _argv), dataPtr(new ApplicationPrivate)
{
  this->dataPtr->engine = new QQmlApplicationEngine();
  igndbg << "Initializing application." << std::endl;
}

/////////////////////////////////////////////////
Application::~Application()
{
  igndbg << "Terminating application." << std::endl;

  if (this->dataPtr->engine)
  {
    delete this->dataPtr->engine;
    this->dataPtr->engine = nullptr;
  }
}

/////////////////////////////////////////////////
QQmlApplicationEngine *Application::Engine() const
{
  return this->dataPtr->engine;
}

/////////////////////////////////////////////////
Application *ignition::gui::App()
{
  return qobject_cast<Application *>(qGuiApp);
}

