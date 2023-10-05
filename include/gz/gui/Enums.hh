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

#ifndef GZ_GUI_ENUMS_HH_
#define GZ_GUI_ENUMS_HH_

#include "gz/gui/qt.h"

namespace gz::gui
{
/// \brief Data roles
enum DataRole
{
  /// \brief Text which is displayed for the user.
  DISPLAY_NAME = Qt::UserRole + 100,

  /// \brief URI including detailed query. This is the information carried
  /// during a drag-drop operation.
  URI_QUERY,

  /// \brief Data type name, such as "Double" or "Bool", or "model", "link".
  /// Used to specialize display according to type.
  TYPE,

  /// \brief Flag indicating whether an item should be expanded or not.
  TO_EXPAND
};

/// \brief String types
enum class StringType
{
  /// \brief Undefined type
  NONE,

  /// \brief Use line for short strings which usually fit in a single
  /// line.
  LINE,

  /// \brief Use text for longer strings which span multiple lines.
  PLAIN_TEXT
};

/// \brief Number types
enum class NumberType
{
  /// \brief Undefined type
  NONE,

  /// \brief Double
  DOUBLE,

  /// \brief Integer
  INT,

  /// \brief Unsigned integer
  UINT
};

/// \brief File types
enum FileType
{
  /// \brief Comma separated value (CSV)
  CSVFile,

  /// \brief Portable document format (PDF)
  PDFFile
};
}  // namespace gz::gui
#endif  // GZ_GUI_ENUMS_HH_
