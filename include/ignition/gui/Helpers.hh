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
#ifndef IGNITION_GUI_HELPERS_HH_
#define IGNITION_GUI_HELPERS_HH_

#include <string>
#include <vector>

#include "ignition/gui/qt.h"
#include "ignition/gui/Enums.hh"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    /// \brief Create a human readable key, capitalizing the first letter
    /// and removing characters like "_".
    /// \param[in] _key Non-human-readable key.
    /// \return Human-readable key.
    IGNITION_GUI_VISIBLE
    std::string humanReadable(const std::string &_key);

    /// \brief Returns the unit for a given key. For example, the key "mass"
    /// returns "kg".
    /// \param[in] _key The key.
    /// \param[in] _type In case the key may have more than one type, the type
    /// must be given too. For example, a prismatic joint will have different
    /// units from a revolute joint.
    /// \return The unit.
    IGNITION_GUI_VISIBLE
    std::string unitFromKey(const std::string &_key,
                            const std::string &_type = "");

    /// \brief Returns the range for a given key. For example, the key
    /// "transparency" returns min == 0, max == 1.
    /// \param[in] _key The key.
    /// \param[out] _min The minimum value.
    /// \param[out] _max The maximum value.
    IGNITION_GUI_VISIBLE
    void rangeFromKey(const std::string &_key, double &_min, double &_max);

    /// \brief Returns the string type for a given key. For example, the key
    /// "innerxml" has a PLAIN_TEXT type while "name" is a LINE.
    /// \param[in] _key The key.
    /// \return The string type.
    IGNITION_GUI_VISIBLE
    StringType stringTypeFromKey(const std::string &_key);

    /// \brief List of colors used for the background of widgets according to
    /// their level.
    const std::vector<QString> kBgColors(
      {"#FFFFFF", "#DDDDDD", "#BBBBBB", "#999999"});

    /// \brief Red color used for "red" or "x" fields.
    const QString kRedColor = "#d42b2b";

    /// \brief Green color used for "green" or "y" fields.
    const QString kGreenColor = "#3bc43b";

    /// \brief Blue color used for "blue" or "z" fields.
    const QString kBlueColor = "#0d0df2";
  }
}
#endif
