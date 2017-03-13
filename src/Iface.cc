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

#include <iostream>
#include <signal.h>

#include "ignition/gui/qt.h"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/Iface.hh"

// These are needed by QT. They need to stay valid during the entire
// lifetime of the application, and argc > 0 and argv must contain one valid
// character string
int g_argc = 1;
char **g_argv;

using namespace ignition;
using namespace gui;

QApplication *g_app;
MainWindow *g_main_win = nullptr;

//////////////////////////////////////////////////
void signal_handler(int)
{
  ignition::gui::stop();
}

/////////////////////////////////////////////////
bool ignition::gui::load()
{
  std::cout << "Load Iface" << std::endl;
  g_argv = new char*[g_argc];
  for (int i = 0; i < g_argc; i++)
  {
    g_argv[i] = new char[strlen("ignition")];
    snprintf(g_argv[i], strlen("ignition"), "ignition");
  }

  g_app = new QApplication(g_argc, g_argv);

  g_main_win = new MainWindow();

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::run(int /*_argc*/, char ** /*_argv*/)
{
  std::cout << "Run Iface" << std::endl;
  if (!ignition::gui::load())
    return false;

#ifndef _WIN32
  // Now that we're about to run, install a signal handler to allow for
  // graceful shutdown on Ctrl-C.
  struct sigaction sigact;
  sigact.sa_flags = 0;
  sigact.sa_handler = signal_handler;
  if (sigemptyset(&sigact.sa_mask) != 0)
    std::cerr << "sigemptyset failed while setting up for SIGINT" << std::endl;
  if (sigaction(SIGINT, &sigact, NULL))
  {
    std::cerr << "signal(2) failed while setting up for SIGINT" << std::endl;
    return false;
  }
#endif

  g_app->exec();

  fflush(stdout);

  delete g_main_win;
  return true;
}

/////////////////////////////////////////////////
void ignition::gui::stop()
{
  g_app->quit();
}

