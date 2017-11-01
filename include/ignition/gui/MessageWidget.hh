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

#include <memory>
#include <string>

#include "ignition/gui/qt.h"
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
    class PropertyWidget;

    /// \brief Generate a widget with property fields based on a Google
    /// Protobuf message.
    ///
    /// The fields are generated based on a message which is passed in the
    /// constructor.
    ///
    /// Updates to the widget's fields can be done by calling `UpdateFromMsg`
    /// as long as the same message type is passed.
    ///
    /// The widget can also fill a message according to the current values of
    /// its widgets input by the user.
    ///
    class IGNITION_GUI_VISIBLE MessageWidget : public QWidget
    {
      Q_OBJECT

      /// \brief Constructor.
      /// \param[in] _msg Message to load from.
      /// \sa UpdateFromMsg
      public: explicit MessageWidget(const google::protobuf::Message *_msg);

      /// \brief Destructor
      public: ~MessageWidget();

      /// \brief Get a message with the widget's current contents. The message
      /// will be of the same type as the one used on the constructor.
      /// \return Updated message.
      /// \sa UpdateFromMsg
      public: google::protobuf::Message *Msg() const;

      /// \brief Update the widgets with values from a message.
      /// \param[in] _msg Message used for updating the widgets.
      /// \return True if successful. This may fail for example, if a message
      /// type differs from the message used to construct the widget.
      /// \sa Msg
      public: bool UpdateFromMsg(const google::protobuf::Message *_msg);

      /// \brief Signal that a property widget's value has changed.
      /// \param[in] _name Scoped name of widget.
      /// \param[in] _value New value.
      signals: void ValueChanged(const std::string &_name,
                                 const QVariant _value);

      /// \brief Set whether a property widget should be visible.
      ///
      /// * If hiding a collapsible, all its children will be hidden.
      /// * If showing a collapsible, all its children will be shown unless
      ///   they've been explicitly hidden.
      /// * If hiding a collapsed property, it won't show when the containing
      ///   collapsible is expanded.
      /// * If showing a collapsed property, it won't show until the containing
      ///   collapsible is expanded.
      ///
      /// \param[in] _name Name of the property widget. For repeated fields,
      /// omit the "::number" which would refer to a specific widget.
      /// \param[in] _visible True to set the widget to be visible.
      /// \return True if the value is set successfully.
      /// \sa PropertyVisible
      public: bool SetPropertyVisible(const std::string &_name,
                                      const bool _visible);

      /// \brief Get whether a property widget is visible.
      /// \param[in] _name Name of the property widget.
      /// \return False if widget is not visible or doesn't exist.
      /// \sa SetPropertyVisible
      public: bool PropertyVisible(const std::string &_name) const;

      /// \brief Set whether all property widgets should be read-only. This
      /// disables all child widgets.
      /// \param[in] _readOnly True for read-only, false for read-write.
      /// \return True if the value is set successfully.
      /// \sa ReadOnly
      public: bool SetReadOnly(const bool _readOnly);

      /// \brief Get whether all property widgets are read-only.
      /// \return False if at least one widget is not read-only.
      /// \sa SetReadOnly
      public: bool ReadOnly() const;

      /// \brief Set whether a property widget should be read-only.
      /// \param[in] _name Name of the property widget. For repeated fields,
      /// omit the "::number" which would refer to a specific widget.
      /// \param[in] _readOnly True to set the widget to be read-only.
      /// \return True if the value is set successfully.
      /// \sa PropertyReadOnly
      public: bool SetPropertyReadOnly(const std::string &_name,
                                     const bool _readOnly);

      /// \brief Get whether a property widget is read-only.
      /// \param[in] _name Name of the property widget.
      /// \return True if the widget is read-only.
      /// \sa SetPropertyReadOnly
      public: bool PropertyReadOnly(const std::string &_name) const;

      /// \brief Set a value of a property widget.
      /// \param[in] _name Name of the property widget.
      /// \param[in] _value Value to set to.
      /// \return True if the value is set successfully.
      /// \sa PropertyValue
      public: bool SetPropertyValue(const std::string &_name,
                                    const QVariant _value);

      /// \brief Get value from a property widget.
      /// \param[in] _name Name of the property widget.
      /// \return Value as QVariant.
      /// \sa SetPropertyValue
      public: QVariant PropertyValue(const std::string &_name) const;

      /// \brief Get a property widget by its scoped name.
      /// \param[in] _name Scoped name of the property widget.
      /// \return The widget with the given name or nullptr if it wasn't found.
      public: PropertyWidget *PropertyWidgetByName(
          const std::string &_name) const;

      /// \brief Get the number of property widgets.
      /// \return The number of property widgets.
      public: unsigned int PropertyWidgetCount() const;

      /// \brief Performs the following:
      /// * Register the widget so that it can be referred by its scoped name
      /// * Forwards its signals to the message widget
      /// * Places the widget in the layout with the correct indentation
      /// \param[in] _scopedName Unique name to indentify the property within
      /// this widget. Nested message names are scoped using `::`, for example:
      /// `pose::position::x`.
      /// \param[in] _property Widget to be added.
      /// \param[in] _parent Parent widget to add the property to.
      /// \return True if property successfully added.
      private: bool AddPropertyWidget(const std::string &_scopedName,
          PropertyWidget *_property, QWidget *_parent);

      /// \brief Performs the following:
      /// * Register the widget so that it can be referred by its scoped name
      /// * Forwards its signals to the message widget
      /// * Places the widget in the layout with the correct indentation
      /// \param[in] _scopedName Unique name to indentify the property within
      /// this widget. Nested message names are scoped using `::`, for example:
      /// `pose::position::x`.
      /// \param[in] _property Widget to be added.
      /// \param[in] _parent Parent widget to add the property to.
      /// \return True if property successfully added.
      private: bool RemovePropertyWidget(const std::string &_scopedName);

      /// \brief Parse the input message and either create widgets for
      /// configuring fields of the message, or update existing widgets with
      /// values from the message. This is called recursively to parse nested
      /// messages.
      /// \param[in] _msg Message.
      /// \param[in] _scopedName Scoped name for new widgets, if this is the
      /// top-level message, the scoped name is an empty string.
      /// \param[in] _parent Pointer to parent of generated widgets, this is
      /// either the message widget or a collapsible widget.
      /// return True if successful.
      private: bool Parse(const google::protobuf::Message *_msg,
          const std::string &_scopedName, QWidget *_parent);

      /// \brief Update the given message using values from the widgets. This is
      /// called recursively to update nested messages.
      /// \param[in] _msg Message to be updated.
      /// \param[in] _parentScopedName Scoped name of parent widget, empty if
      /// this is the top level message.
      /// \return True if successful
      private: bool FillMsg(google::protobuf::Message *_msg,
          const std::string &_parentScopedName = "") const;

      /// \internal
      /// \brief Pointer to private data.
      private: std::unique_ptr<MessageWidgetPrivate> dataPtr;
    };
  }
}
#endif
