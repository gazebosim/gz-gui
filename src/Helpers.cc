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
#include <ignition/common/Console.hh>
#include <ignition/math/Helpers.hh>

#include "ignition/gui/Helpers.hh"

/////////////////////////////////////////////////
std::string ignition::gui::humanReadable(const std::string &_key)
{
  std::string humanKey = _key;
  humanKey[0] = std::toupper(humanKey[0]);
  std::replace(humanKey.begin(), humanKey.end(), '_', ' ');
  return humanKey;
}

/////////////////////////////////////////////////
std::string ignition::gui::unitFromKey(const std::string &_key,
    const std::string &_type)
{
  if (_key == "pos" || _key == "length" || _key == "min_depth")
  {
    return "m";
  }

  if (_key == "rot")
    return "rad";

  if (_key == "kp" || _key == "kd")
    return "N/m";

  if (_key == "max_vel")
    return "m/s";

  if (_key == "mass")
    return "kg";

  if (_key == "ixx" || _key == "ixy" || _key == "ixz" ||
      _key == "iyy" || _key == "iyz" || _key == "izz")
  {
    return "kg&middot;m<sup>2</sup>";
  }

  if (_key == "density")
  {
    return "kg/m<sup>3</sup>";
  }

  if (_key == "limit_lower" || _key == "limit_upper")
  {
    if (_type == "PRISMATIC")
      return "m";
    else if (_type != "")
      return "rad";
  }

  if (_key == "limit_effort")
  {
    if (_type == "PRISMATIC")
      return "N";
    else if (_type != "")
      return "Nm";
  }

  if (_key == "limit_velocity" || _key == "velocity")
  {
    if (_type == "PRISMATIC")
      return "m/s";
    else if (_type != "")
      return "rad/s";
  }

  if (_key == "damping")
  {
    if (_type == "PRISMATIC")
      return "Ns/m";
    else if (_type != "")
      return "Ns";
  }

  if (_key == "friction")
  {
    if (_type == "PRISMATIC")
      return "N";
    else if (_type != "")
      return "Nm";
  }

  ignerr << "Failed to get unit for key [" << _key << "] type ["
         << _type << "]" << std::endl;
  return "";
}

/////////////////////////////////////////////////
void ignition::gui::rangeFromKey(const std::string &_key, double &_min,
    double &_max)
{
  // Maximum range by default
  _min = -math::MAX_D;
  _max = math::MAX_D;

  if (_key == "mass" || _key == "ixx" || _key == "ixy" || _key == "ixz" ||
      _key == "iyy" || _key == "iyz" || _key == "izz" || _key == "length" ||
      _key == "min_depth" || _key == "density")
  {
    _min = 0;
  }
  else if (_key == "bounce" || _key == "transparency" ||
      _key == "laser_retro" || _key == "ambient" || _key == "diffuse" ||
      _key == "specular" || _key == "emissive" ||
      _key == "restitution_coefficient")
  {
    _min = 0;
    _max = 1;
  }
  else if (_key == "fdir1" || _key == "xyz")
  {
    _min = -1;
    _max = +1;
  }
}


