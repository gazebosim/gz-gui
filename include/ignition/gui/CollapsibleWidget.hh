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

#ifndef IGNITION_GUI_COLLAPSIBLEWIDGET_HH_
#define IGNITION_GUI_COLLAPSIBLEWIDGET_HH_

#include <memory>
#include <string>

#include "ignition/gui/qt.h"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    class CollapsibleWidgetPrivate;

    /// \brief A widget consisting of a button and some content. The
    /// content is hidden or shown as the button is toggled.
    /// It inherits from property widget, but it doesn't hold a property value.
    class IGNITION_GUI_VISIBLE CollapsibleWidget : public PropertyWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _key Title to be displayed on the button.
      public: explicit CollapsibleWidget(const std::string &_key);

      /// \brief Destructor
      public: virtual ~CollapsibleWidget();

      /// \brief Inherited from PropertyWidget.
      ///
      /// Sets the given value to the first child property widget.
      ///
      /// \param[in] _value New value.
      /// \return True if succesfull, will fail if there are no child
      /// property widgets.
      public: bool SetValue(const QVariant _value) override;

      /// \brief Inherited from PropertyWidget.
      ///
      /// Returns a variant containing the value of the first child property
      /// widget.
      ///
      /// \return The current value of the first child property widget, or an
      /// invalid variant if there are no children.
      public: QVariant Value() const override;

      /// \brief Callback that collapses or expands the contents.
      /// _param[in] _checked True for expanded.
      public slots: void Toggle(const bool _checked);

      /// \brief Whether this is collapsed or not
      /// \return True for expanded.
      public: bool IsExpanded() const;

      /// \brief Append a widget to the collapsible contents. The contents
      /// consist of a vertical layout.
      /// \param[in] _widget Widget to be appended.
      public: void AppendContent(QWidget *_widget);

      /// \brief Get the current number of items in the content.
      /// \return The number of widgets in the content.
      public: unsigned int ContentCount() const;

      // Documentation inherited
      public: virtual void SetReadOnly(const bool _readOnly,
                                       const bool _explicit) override;

      // Documentation inherited
      public: virtual bool ReadOnly() const override;

      /// \brief Notifies when widget has been toggled.
      /// \param[in] _expanded True for expanded, false for collapsed.
      signals: void Toggled(const bool _expanded) const;

      /// \brief Pointer to private data
      private: std::unique_ptr<CollapsibleWidgetPrivate> dataPtr;
    };
  }
}
#endif
