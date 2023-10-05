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
#ifndef GZ_GUI_DRAGDROPMODEL_HH_
#define GZ_GUI_DRAGDROPMODEL_HH_

#include "gz/gui/Export.hh"
#include "gz/gui/qt.h"

namespace gz::gui
{
/// \brief Customized item model so that we can pass along an URI query as
/// MIME information during a drag-drop.
class GZ_GUI_VISIBLE DragDropModel : public QStandardItemModel
{
  /// \brief Overloaded from Qt. Custom MIME data function.
  /// \param[in] _indexes List of selected items.
  /// \return Mime data for the selected items.
  public: QMimeData *mimeData(const QModelIndexList &_indexes) const;
};
}  // namespace gz::gui
#endif  // GZ_GUI_DRAGDROPMODEL_HH_
