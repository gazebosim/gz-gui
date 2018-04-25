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

#ifndef IGNITION_GUI_PROPERTYWIDGET_HH_
#define IGNITION_GUI_PROPERTYWIDGET_HH_

#include <memory>
#include <string>

#include "ignition/gui/qt.h"
#include "ignition/gui/Export.hh"

namespace ignition
{
  namespace gui
  {
    class PropertyWidgetPrivate;

    /// \brief Abstract base class for widgets which hold properties, such as
    /// numbers and strings. Each derived class specializes in a property type.
    /// The value of any derived widget can be retrieved as a QVariant.
    ///
    /// # Usage example
    ///
    /// Let's say we have a derived class to handle type T called TWidget.
    ///
    /// First we must make sure the type is available to QVariant, this is done
    /// by calling Q_DECLARE_METATYPE(T). For convenience, types used by
    /// widgets built into Ignition GUI are added in the QtMetatypes.hh header.
    ///
    /// The value which the widget currently holds can be retrieved as follows:
    ///
    /// T value = tWidget->Value().value<T>();
    ///
    /// The value of the widget can be set as follows:
    ///
    /// tWidget->SetValue(QVariant::fromValue(value));
    ///
    /// You can also listen to the ValueChanged signal, which is emitted when
    /// the widget's value changes.
    ///
    class IGNITION_GUI_VISIBLE PropertyWidget : public QFrame
    {
      Q_OBJECT

      /// \brief Constructor
      public: PropertyWidget();

      /// \brief Destructor
      public: virtual ~PropertyWidget();

      /// \brief Get value from widget.
      /// \return Value of the widget as a QVariant.
      public: virtual QVariant Value() const = 0;

      /// \brief Update widget with a new value.
      /// \param[in] _value Value to set to.
      /// \return True if successful.
      public: virtual bool SetValue(const QVariant _value) = 0;

      /// \brief Set whether this widget is read-only or read-write. By default,
      /// it calls `this->setEnabled(!_readOnly)`, but derived classes can
      /// implement their own.
      /// Once set explicitly, it can only be overriden explicitly.
      /// \param[in] _readOnly True for read-only (disabled)
      /// \param[in] _explicit True if explicitly setting the widget, false if
      /// this is being handed down from a parent.
      public: virtual void SetReadOnly(const bool _readOnly,
                                       const bool _explicit = true);

      /// \brief Get whether this widget is read-only or read-write. By default,
      /// it returns `!this->isEnabled()`, but derived classes can implement
      /// their own.
      /// \return True if read-only (disabled)
      public: virtual bool ReadOnly() const;

      /// \brief Used for drag and drop. Set the URI that contains the Ignition
      /// Transport topic and message field associated to the widget. This is
      /// needed by the widget that receives the drop event, as likely it will
      /// subscribe to this topic to get some data.
      /// \param[in] _uri The URI used for drag and drop.
      /// \sa DragAndDropURI().
      public: void SetDragAndDropURI(const std::string &_uri);

      /// \brief Used for drag and drop. The name of the URI that contains the
      /// Ignition Transport topic and message field associated to the widget.
      /// \return The URI.
      /// \sa SetDragAndDropURI().
      public: std::string DragAndDropURI() const;

      /// \brief Signal that the value has changed.
      /// \param[in] _value New value.
      signals: void ValueChanged(const QVariant _value);

      /// \brief Callback when an internal widget's value has changed.
      protected slots: void OnValueChanged();

      /// \brief Filter events from other Qt objects.
      /// param[in] _object Qt object watched by the event filter.
      /// param[in] _event Qt event to be filtered.
      /// \return True to stop event propagation.
      protected slots: bool eventFilter(QObject *_object,
                                        QEvent *_event) override;

      /// \brief Pointer to private data
      private: std::unique_ptr<PropertyWidgetPrivate> dataPtr;
    };
  }
}
#endif
