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

    /// \brief A widget generated from a google protobuf message.
    class IGNITION_GUI_VISIBLE MessageWidget : public QWidget
    {
      Q_OBJECT

      /// \brief Generate a widget with property fields based on a google
      /// protobuf message.
      ///
      /// Updates to the widget's fields can be done by calling `UpdateFromMsg`
      /// as long as the same message type is passed.
      ///
      /// \param[in] _msg Message to load from.
      /// \sa UpdateFromMsg
      public: MessageWidget(const google::protobuf::Message *_msg);

      /// \brief Destructor
      public: ~MessageWidget();

      /// \brief Get a message with the widget's current contents. The message
      /// will be of the same type as the one used on the constructor.
      /// \return Updated message.
      public: google::protobuf::Message *Msg();

      /// \brief Update the widgets from a message.
      /// \param[in] _msg Message used for updating the widgets.
      /// \return True if succesfull. This may fail for example, if a message
      /// type differs from the message used to construct the widget.
      public: bool UpdateFromMsg(const google::protobuf::Message *_msg);

      /// \brief Signal that a property widget's value has changed.
      /// \param[in] _name Scoped name of widget.
      /// \param[in] _value New value.
      signals: void ValueChanged(const std::string &_name,
                                 const QVariant _value);

      /// \brief Set whether a property widget should be visible.
      /// \param[in] _name Name of the property widget.
      /// \param[in] _visible True to set the widget to be visible.
      public: void SetWidgetVisible(const std::string &_name,
                                    const bool _visible);

      /// \brief Get whether a property widget is visible.
      /// \param[in] _name Name of the property widget.
      /// \return True if the widget is visible.
      public: bool WidgetVisible(const std::string &_name) const;

      /// \brief Set whether all property widgets should be read-only.
      /// \param[in] _readOnly True for read-only.
      public: void SetReadOnly(const bool _readOnly);

      /// \brief Get whether all property widgets are read-only.
      /// \return false if at least one widget is not read-only.
      public: bool ReadOnly() const;

      /// \brief Set whether a property widget should be read-only.
      /// \param[in] _name Name of the property widget.
      /// \param[in] _readOnly True to set the widget to be read-only.
      public: void SetWidgetReadOnly(const std::string &_name,
                                     const bool _readOnly);

      /// \brief Get whether a property widget is read-only.
      /// \param[in] _name Name of the property widget.
      /// \return True if the widget is read-only.
      public: bool WidgetReadOnly(const std::string &_name) const;

      /// \brief Set a value of a property widget.
      /// \param[in] _name Name of the property widget.
      /// \param[in] _value Value to set to.
      /// \return True if the value is set successfully.
      public: bool SetPropertyValue(const std::string &_name,
                                    const QVariant _value);

      /// \brief Get value from a property widget.
      /// \param[in] _name Name of the property widget.
      /// \return Value as QVariant.
      public: QVariant PropertyValue(const std::string &_name) const;

      /// \brief Get a config property widget by its name.
      /// \param[in] _name Scoped name of the property widget.
      /// \return The widget with the given name or nullptr if it wasn't found.
      public: PropertyWidget *PropertyWidgetByName(
          const std::string &_name) const;

      /// \brief Get the number of property widgets.
      /// \return The number of property widgets.
      public: unsigned int PropertyWidgetCount() const;

      /// \brief Register a property widget as a property of this widget, so it
      /// can be updated. Note that the widget is not automatically added to a
      /// layout.
      /// \param[in] _name Unique name to indentify the property within this
      /// widget
      /// \param[in] _property Child widget to be added. It doesn't need to be a
      /// PropertyWidget.
      /// \return True if property successfully added.
      private: bool AddPropertyWidget(const std::string &_name,
          PropertyWidget *_property);

      /// \brief Parse the input message and either create widgets for
      /// configuring fields of the message, or update the widgets with values
      /// from the message. This is called recursively to parse nested messages.
      /// \param[in] _msg Message.
      /// \param[in] _scopedName Scoped name for new widgets.
      /// \param[in] _parent Pointer to parent of generated widgets.
      /// return True if successful.
      private: bool Parse(google::protobuf::Message *_msg,
          const std::string &_name, QWidget *_parent);

      /// \brief Update the message field using values from the widgets.
      /// \param[in] _msg Message to be updated.
      /// \param[in] _name Optional name of a property field. By default, takes
      /// the whole message.
      /// \return True if successful
      private: bool FillMsg(google::protobuf::Message *_msg,
          const std::string &_name = "");

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
