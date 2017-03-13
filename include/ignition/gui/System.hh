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
#ifndef IGNITION_GUI_SYSTEM_HH_
#define IGNITION_GUI_SYSTEM_HH_

#if defined(__GNUC__)
#define IGNITION_GUI_DEPRECATED(version) __attribute__((deprecated))
#define IGNITION_GUI_FORCEINLINE __attribute__((always_inline))
#elif defined(_WIN32)
#define IGNITION_GUI_DEPRECATED(version) ()
#define IGNITION_GUI_FORCEINLINE __forceinline
#else
#define IGNITION_GUI_DEPRECATED(version) ()
#define IGNITION_GUI_FORCEINLINE
#endif

/// \def IGNITION_GUI_VISIBLE
/// Use to represent "symbol visible" if supported

/// \def IGNITION_GUI_HIDDEN
/// Use to represent "symbol hidden" if supported
#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define IGNITION_GUI_VISIBLE __attribute__ ((dllexport))
    #else
      #define IGNITION_GUI_VISIBLE __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define IGNITION_GUI_VISIBLE __attribute__ ((dllimport))
    #else
      #define IGNITION_GUI_VISIBLE __declspec(dllimport)
    #endif
  #endif
  #define IGNITION_GUI_HIDDEN
#else
  #if __GNUC__ >= 4
    #define IGNITION_GUI_VISIBLE __attribute__ ((visibility ("default")))
    #define IGNITION_GUI_HIDDEN  __attribute__ ((visibility ("hidden")))
  #else
    #define IGNITION_GUI_VISIBLE
    #define IGNITION_GUI_HIDDEN
  #endif
#endif

#endif
