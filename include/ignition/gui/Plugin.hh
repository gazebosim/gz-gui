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
#ifndef IGNITION_GUI_PLUGIN_HH_
#define IGNITION_GUI_PLUGIN_HH_

#ifdef _WIN32
  // Ensure that Winsock2.h is included before Windows.h, which can get
  // pulled in by anybody (e.g., Boost).
  // This was put here because all the plugins are going to use it
  // This doesn't guarantee something else won't cause it,
  // but this saves putting this in every plugin
#include <Winsock2.h>
#endif

#ifndef _WIN32
  #include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <dlfcn.h>

#include <iostream>
#include <list>
#include <memory>
#include <string>

#include "ignition/gui/qt.h"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    /// \addtogroup gazebo_common Common
    /// \{

    /// \brief A class which all plugins must inherit from
    template<class T>
    class PluginT
    {
      /// \brief plugin pointer type definition
      public: typedef std::shared_ptr<T> TPtr;

      /// \brief Constructor
      public: PluginT()
              {
              }

      /// \brief Destructor
      public: virtual ~PluginT()
              {
                // dlclose has been disabled due to segfaults in the test suite
                // This workaround is detailed in #1026 and #1066. After the test
                // or gazebo execution the plugin is not loaded in memory anymore
                // \todo Figure out the right thing to do.

                // dlclose(this->dlHandle);
              }

      /// \brief Get the full path to the plugin shared library.
      /// \return File name.
      public: std::string Filename() const
              {
                return this->filename;
              }

      /// \brief Get the user given plugin name.
      /// \return Handle name.
      public: std::string Handle() const
              {
                return this->handleName;
              }

      /// \brief a class method that creates a plugin from a file name.
      /// It locates the shared library and loads it dynamically.
      /// \param[in] _filename The path to the shared library. This can be the
      /// full path, or a path relative to one of the paths specified in an
      /// environment variable.
      /// \param[in] _name Short name of the plugin
      /// \return Shared Pointer to this class type
      public: static TPtr Create(const std::string &_filename,
                  const std::string &_name)
              {
                TPtr result;
                std::string filename(_filename);

  #ifdef __APPLE__
                // This is a hack to work around issue #800,
                // error loading plugin libraries with different extensions
                {
                  size_t soSuffix = filename.rfind(".so");
                  if (soSuffix != std::string::npos)
                  {
                    const std::string macSuffix(".dylib");
                    filename.replace(soSuffix, macSuffix.length(), macSuffix);
                  }
                }
  #elif _WIN32
                // Corresponding windows hack
                {
                  // replace .so with .dll
                  size_t soSuffix = filename.rfind(".so");
                  if (soSuffix != std::string::npos)
                  {
                    const std::string winSuffix(".dll");
                    filename.replace(soSuffix, winSuffix.length(), winSuffix);
                  }
                  // remove the lib prefix
                  filename.erase(0, 3);
                }
  #endif  // ifdef __APPLE__

                // TODO: Get paths from env variable
                std::list<std::string> pluginPaths;
                pluginPaths.push_back("/home/louise/.igngui/plugins");

                struct stat st;
                for (auto iter : pluginPaths)
                {
                  auto fullname = iter + std::string("/") + filename;
                  if (stat(fullname.c_str(), &st) == 0)
                  {
                    filename = fullname;
                    break;
                  }
                }

                fptr_union_t registerFunc;
                std::string registerName = "RegisterPlugin";

                void *dlHandle = dlopen(filename.c_str(), RTLD_LAZY|RTLD_GLOBAL);
                if (!dlHandle)
                {
                  std::cerr << "Failed to load plugin [" << filename << "]: "
                    << dlerror() << std::endl;
                  return result;
                }

                registerFunc.ptr = dlsym(dlHandle, registerName.c_str());

                if (!registerFunc.ptr)
                {
                  std::cerr << "Failed to resolve " << registerName
                        << ": " << dlerror();
                  return result;
                }

                // Register the new controller.
                result.reset(registerFunc.func());
                result->dlHandle = dlHandle;

                result->handleName = _name;
                result->filename = filename;

                return result;
              }

      /// \brief Full path to the shared library file
      protected: std::string filename;

      /// \brief User given name.
      protected: std::string handleName;

      /// \brief Pointer to shared library registration function definition
      private: typedef union
               {
                 T *(*func)();
                 void *ptr;
               } fptr_union_t;

      /// \brief Handle used for closing the dynamic library.
      private: void *dlHandle = nullptr;
    };

    /// \brief Gui plugin
    class IGNITION_GUI_VISIBLE GUIPlugin
        : public QWidget, public PluginT<GUIPlugin>
    {
      public: GUIPlugin() : QWidget(nullptr)
              {}

      /// \brief Load function
      ///
      /// Called when a plugin is first created.
      /// This function should not be blocking.
      public: virtual void Load() {}

      // \brief must be defined to support style sheets
      public: virtual void paintEvent(QPaintEvent *)
      {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
      }
    };

    /// \}

    /// \brief Plugin registration function for gui plugin. Part of the
    /// shared object interface. This function is called when loading the shared
    /// library to add the plugin to the registered list.
    /// \return the name of the registered plugin
    #define IGN_REGISTER_GUI_PLUGIN(classname) \
      extern "C" IGNITION_GUI_VISIBLE ignition::gui::GUIPlugin *RegisterPlugin(); \
      ignition::gui::GUIPlugin *RegisterPlugin() \
      {\
        return new classname();\
      }
  }
}

#endif
