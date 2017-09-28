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

#ifndef IGNITION_GUI_MESSAGEWIDGET_HH_
#define IGNITION_GUI_MESSAGEWIDGET_HH_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <ignition/math/Vector3.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/System.hh"

namespace google
{
  namespace protobuf
  {
    class Message;
    class Reflection;
    class FieldDescriptor;
  }
}

namespace ignition
{
  namespace gui
  {
    class MessageWidgetPrivate;

    /// \brief A widget for density properties.
    class IGNITION_GUI_VISIBLE DensityWidget : public PropertyWidget
    {
      Q_OBJECT

      /// \brief Constructor
      public: DensityWidget();

      /// \brief Updates the widget's density value.
      /// \param[in] _density New density value.
      public: void SetDensity(const double _density);

      /// \brief Accessor for the widget's density value.
      /// \return The density value.
      public: double Density() const;

      /// \brief Callback when the density combo box is changed.
      /// \param[in] _text New density type in string.
      private slots: void OnComboBoxChanged(const QString &_text);

      /// \brief Callback when the density spin box is changed.
      /// \param[in] _text New density value in string.
      private slots: void OnSpinBoxChanged(const QString &_text);

      /// \brief Signal emitted when density has changed.
      /// \param[in] _value Density value.
      Q_SIGNALS: void DensityValueChanged(const double &_value);

      /// \brief A combo box for density according to material.
      public: QComboBox *comboBox;

      /// \brief A spin box for density value.
      public: QDoubleSpinBox *spinBox;

      /// \brief Current density value.
      private: double density;
    };

    /// \brief A widget generated from a google protobuf message.
    class IGNITION_GUI_VISIBLE MessageWidget : public QWidget
    {
      Q_OBJECT

      /// \brief Constructor
      public: MessageWidget();

      /// \brief Destructor
      public: ~MessageWidget();

      /// \brief Load from a google protobuf message.
      /// \param[in] _msg Message to load from.
      public: void Load(const google::protobuf::Message *_msg);

      /// \brief Get the updated message.
      /// \return Updated message.
      public: google::protobuf::Message *Msg();

      /// \brief Signal that a property widget's value has changed.
      /// \param[in] _name Scoped name of widget.
      /// \param[in] _value New value.
      signals: void ValueChanged(const std::string &_name, const QVariant _value);

      /// \brief Set whether a child widget should be visible.
      /// \param[in] _name Name of the child widget.
      /// \param[in] _visible True to set the widget to be visible.
      public: void SetWidgetVisible(const std::string &_name, bool _visible);

      /// \brief Get whether a child widget is visible.
      /// \param[in] _name Name of the child widget.
      /// \return True if the widget is visible.
      public: bool WidgetVisible(const std::string &_name) const;

      /// \brief Set whether a child widget should be read-only.
      /// \param[in] _name Name of the child widget.
      /// \param[in] _visible True to set the widget to be read-only.
      public: void SetWidgetReadOnly(const std::string &_name, bool _readOnly);

      /// \brief Get whether a child widget is read-only.
      /// \param[in] _name Name of the child widget.
      /// \return True if the widget is read-only.
      public: bool WidgetReadOnly(const std::string &_name) const;

      /// \brief Update the widgets from a message.
      /// \param[in] _msg Message used for updating the widgets.
      public: void UpdateFromMsg(const google::protobuf::Message *_msg);

      /// \brief Set a value of a property widget.
      /// \param[in] _name Name of the property widget.
      /// \param[in] _value Value to set to.
      /// \return True if the value is set successfully.
      public: bool SetPropertyValue(const std::string &_name,
                                    const QVariant _value);

      /// \brief Set a density value to a child widget.
      /// \param[in] _name Name of the child widget.
      /// \param[in] _value Density value to set to.
      /// \return True if the value is set successfully.
      public: bool SetDensityWidgetValue(const std::string &_name,
          const double _value);

      /// \brief Get value from a property widget.
      /// \param[in] _name Name of the property widget.
      /// \return Value as QVariant.
      public: QVariant PropertyValue(const std::string &_name) const;

      /// \brief Get a density value from a child widget.
      /// \param[in] _name Name of the child widget.
      /// \return Density value.
      public: double DensityWidgetValue(const std::string &_name) const;

      /// \brief Create a widget for setting a density value.
      /// \param[in] _key A key that is used as a label for the widget.
      /// \param[in] _level Level of the widget in the tree.
      /// \return The newly created widget.
      public: PropertyWidget *CreateDensityWidget(const std::string &_key,
          const int _level = 0);

      /// \brief Register a child widget as a child of this widget, so it can
      /// be updated. Note that the widget is not automatically added to a
      /// layout.
      /// \param[in] _name Unique name to indentify the child within this widget
      /// \param[in] _child Child widget to be added. It doesn't need to be a
      /// PropertyWidget.
      /// \return True if child successfully added.
      public: bool AddPropertyWidget(const std::string &_name,
          PropertyWidget *_child);

      /// \brief Insert a layout into the config widget's layout at a specific
      /// position.
      /// \param[in] _layout The layout to be inserted.
      /// \param[in] _pos The position to insert at, 0 being the top.
      public: void InsertLayout(QLayout *_layout, int _pos);

      /// \brief Get a config child widget by its name.
      /// \param[in] _name Scoped name of the child widget.
      /// \return The child widget with the given name or nullptr if it wasn't
      /// found.
      public: PropertyWidget *PropertyWidgetByName(
          const std::string &_name) const;

      /// \brief Get the number of child widgets.
      /// \return The number of child widgets.
      public: unsigned int PropertyWidgetCount() const;

      /// \brief Get a style sheet in string format, to be applied to a child
      /// config widget with setStyleSheet.
      /// \param[in] _type Type of style sheet, such as "warning", "active",
      /// "normal".
      /// \param[in] _level Level of widget in the tree.
      /// \return Style sheet as string. Returns an empty string if _type is
      /// unknown.
      public: static QString StyleSheet(const std::string &_type,
          const int _level = 0);

      /// \brief Parse the input message and either create widgets for
      /// configuring fields of the message, or update the widgets with values
      /// from the message.
      /// \param[in] _msg Message.
      /// \param[in] _update True to parse only fields that are specified in
      /// the message rather than all the available fields in the message
      /// \param[in] _name Name used when creating new widgets.
      /// \param[in] _level Level of the widget in the tree.
      /// return Updated widget.
      private: QWidget *Parse(google::protobuf::Message *_msg,
          bool _update = false, const std::string &_name = "",
          const int _level = 0);

      /// \brief Parse a vector3 message.
      /// param[in] _msg Input vector3d message.
      /// return Parsed vector.
      private: math::Vector3d ParseVector3d(
          const google::protobuf::Message *_msg) const;

      /// \brief Update the message field using values from the widgets.
      /// \param[in] _msg Message to be updated.
      /// \param[in] _name Name of parent widget.
      private: void UpdateMsg(google::protobuf::Message *_msg,
          const std::string &_name = "");

      /// \brief Update a vector3d message.
      /// \param[in] _msg Vector3d message to be updated.
      /// \param[in] _value ignition math Vector3d used for updating the
      /// message.
      private: void UpdateVector3dMsg(google::protobuf::Message *_msg,
          const math::Vector3d &_value);

      /// \brief Update a child widget with a density value.
      /// \param[in] _widget Pointer to the child widget.
      /// \param[in] _value Density value.
      /// \return True if the update completed successfully.
      private: bool UpdateDensityWidget(PropertyWidget *_widget,
          const double _value);

      /// \brief Received item selection user input.
      /// \param[in] _item Item selected.
      /// \param[in] _column Column index.
      private slots: void OnItemSelection(QTreeWidgetItem *_item,
          const int _column);

      /// \brief Signal emitted when density value changes.
      /// \param[in] _value Density value.
      Q_SIGNALS: void DensityValueChanged(const double &_value);

      /// \brief Signal emitted when mass value changes.
      /// \param[in] _value Mass value.
      Q_SIGNALS: void MassValueChanged(const double &_value);

      /// \brief Callback when density value changes in child widget.
      /// \param[in] _value Density value.
      private slots: void OnDensityValueChanged(const double _value);

      /// \brief Callback when mass value changes in child widget.
      /// \param[in] _value Mass value.
      private slots: void OnMassValueChanged(const double _value);

      /// \brief Qt event filter currently used to filter mouse wheel events.
      /// \param[in] _obj Object that is watched by the event filter.
      /// \param[in] _event Qt event.
      /// \return True if the event is handled.
      private: bool eventFilter(QObject *_obj, QEvent *_event);

      /// \internal
      /// \brief Pointer to private data.
      private: std::unique_ptr<MessageWidgetPrivate> dataPtr;
    };
  }
}
#endif
