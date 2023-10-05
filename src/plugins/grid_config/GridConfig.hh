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

#ifndef GZ_GUI_PLUGINS_GRIDCONFIG_HH_
#define GZ_GUI_PLUGINS_GRIDCONFIG_HH_

#include <memory>

#include <gz/gui/Plugin.hh>

namespace gz::gui::plugins
{
class GridConfigPrivate;

/// \brief Manages grids in a Gazebo Rendering scene. This plugin can be
/// used for:
/// * Introspecting grids
/// * Editing grids
///
/// ## Configuration
///
/// * \<insert\> : One grid will be inserted at startup for each \<insert\>
///                tag.
///   * \<horizontal_cell_count\> : Number of cells in the horizontal
///                                 direction, defaults to 20.
///   * \<vertical_cell_count\> : Number of cells in the vertical direction,
///                               defaults to 0;
///   * \<cell_length\> : Length of each cell, defaults to 1.
///   * \<pose\> : Grid pose, defaults to the origin.
///   * \<color\> : Grid color, defaults to (0.7, 0.7, 0.7, 1.0)
class GridConfig : public gz::gui::Plugin
{
  Q_OBJECT

  /// \brief Name list
  Q_PROPERTY(
    QStringList nameList
    READ NameList
    WRITE SetNameList
    NOTIFY NameListChanged
  )

  /// \brief Constructor
  public: GridConfig();

  /// \brief Destructor
  public: ~GridConfig() override;

  // Documentation inherited
  public: void LoadConfig(const tinyxml2::XMLElement *) override;

  // Documentation inherited
  protected: bool eventFilter(QObject *_obj, QEvent *_event) override;

  /// \brief Create grids defined at startup
  public: void CreateGrids();

  /// \brief Update grid
  public: void UpdateGrid();

  /// \brief Callback to retrieve existing grid.
  public: void ConnectToGrid();

  /// \brief Refresh list of grids. This is called in the rendering thread.
  public: void RefreshList();

  /// \brief Callback when refresh button is pressed.
  public slots: void OnRefresh();

  /// \brief Callback when a new name is chosen on the combo box.
  /// \param[in] _name Grid name
  public slots: void OnName(const QString &_name);

  /// \brief Get the list of grid names
  /// \return List of grids.
  public slots: QStringList NameList() const;

  /// \brief Set the list of names
  /// \param[in] _nameList List of names
  public slots: void SetNameList(const QStringList &_nameList);

  /// \brief Notify that name list has changed
  signals: void NameListChanged();

  /// \brief Callback to update vertical cell count
  /// \param[in] _cellCount new vertical cell count
  public slots: void UpdateVCellCount(int _cellCount);

  /// \brief Callback to update horizontal cell count
  /// \param[in] _cellCount new horizontal cell count
  public slots: void UpdateHCellCount(int _cellCount);

  /// \brief Callback to update cell length
  /// \param[in] _length new cell length
  public slots: void UpdateCellLength(double _length);

  /// \brief Callback to update grid pose
  /// \param[in] _x, _y, _z cartesion coordinates
  /// \param[in] _roll, _pitch, _yaw principal coordinates
  public slots: void SetPose(double _x, double _y, double _z,
                             double _roll, double _pitch, double _yaw);

  /// \brief Callback to update grid color
  /// \param[in] _r, _g, _b, _a RGB color model with fourth alpha channel
  public slots: void SetColor(double _r, double _g, double _b, double _a);

  /// \brief Callback when checkbox is clicked.
  /// \param[in] _checked indicates show or hide grid
  public slots: void OnShow(bool _checked);

  /// \brief Notify QML that grid values have changed.
  /// \param[in] _hCellCount Horizontal cell count
  /// \param[in] _vCellCount Vertical cell count
  /// \param[in] _cellLength Cell length
  /// \param[in] _pos XYZ Position
  /// \param[in] _rot RPY orientation
  /// \param[in] _color Grid color
  signals: void newParams(
      int _hCellCount,
      int _vCellCount,
      double _cellLength,
      QVector3D _pos,
      QVector3D _rot,
      QColor _color);

  /// \internal
  /// \brief Pointer to private data.
  private: std::unique_ptr<GridConfigPrivate> dataPtr;
};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_GRIDCONFIG_HH_
