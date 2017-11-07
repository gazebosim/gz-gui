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
#ifndef IGNITION_GUI_VARIABLEPILLCONTAINER_HH_
#define IGNITION_GUI_VARIABLEPILLCONTAINER_HH_

#include <memory>
#include <string>

#include "ignition/gui/VariablePill.hh"
#include "ignition/gui/System.hh"

namespace ignition
{
namespace gui
{
  // Forward declare private data class
  class VariablePillContainerPrivate;

  /// \brief A container for holding variable pills
  ///
  /// The container consists of a label and a field where pills and multi-pills
  /// can be dragged to.
  class IGNITION_GUI_VISIBLE VariablePillContainer : public QWidget
  {
    Q_OBJECT

    /// \brief Constructor.
    /// \param[in] _parent Pointer to a parent widget.
    public: explicit VariablePillContainer(QWidget *_parent = nullptr);

    /// \brief Destructor.
    public: virtual ~VariablePillContainer();

    /// \brief Set the label text for this variable pill container.
    /// \param[in] _text Text to set the label to.
    public: void SetText(const std::string &_text);

    /// \brief Get the variable pill container's label text.
    /// \return Container label.
    public: std::string Text() const;

    /// \brief Set the maximum number of variable pills this container can
    /// hold.
    /// \param[in] _max Maximum number of variable pills. -1 means unlimited.
    public: void SetMaxSize(const int _max);

    /// \brief Get the maximum number of variable pills this container can
    /// hold.
    /// \return Maximum number of variable pills. -1 means unlimited.
    public: int MaxSize() const;

    /// \brief Set the label text for a variable pill in this container.
    /// \param[in] _id Unique id of the variable pill.
    /// \param[in] _text Text to set the variable pill label to.
    public: void SetVariablePillLabel(const unsigned int _id,
                                      const std::string &_text);

    /// \brief Create a new variable pill and add it either to the container or
    /// to a multi-variable pill in the container.
    ///
    /// This calls AddVariablePill(VariablePill *)
    ///
    /// \param[in] _name Name for the new variable pill.
    /// \param[in] _targetId If adding to a multi-variable pill, this is the
    /// pill's id. Leave as EmptyVariable so it is added to the container.
    public: unsigned int AddVariablePill(const std::string &_name,
                    const unsigned int _targetId = VariablePill::EmptyVariable);

    /// \brief Add an existing variable pill to the container or one of the
    /// multi-variable pills inside it.
    /// \param[in] _variable Variable pill to be added.
    /// \param[in] _targetId If adding to a multi-variable pill, this is the
    /// pill's id. Leave as EmptyVariable so it is added to the container.
    public: void AddVariablePill(VariablePill *_variable,
                    const unsigned int _targetId = VariablePill::EmptyVariable);

    /// \brief Remove a variable pill from the container.
    /// \param[in] _variable Variable pill to remove.
    public: void RemoveVariablePill(VariablePill *_variable);

    /// \brief Remove a variable pill from the container.
    /// \param[in] _id Unique id of the variable pill to remove.
    public: void RemoveVariablePill(const unsigned int _id);

    /// \brief Get the number of child variable pills.
    /// \return Number of child variable pills.
    public: unsigned int VariablePillCount() const;

    /// \brief Get a variable pill by id.
    /// \param[in] _id Variable pill id.
    /// \return Variable pill with the specified id.
    public: gui::VariablePill *VariablePill(const unsigned int _id) const;

    /// \brief Set the given variable pill as the currently selected one and
    /// deselects any other variables which may be currently selected.
    /// \param[in] _variable Variable pill which should be selected.
    public: void SetSelected(gui::VariablePill *_variable);

    /// \brief Used to accept drag enter events.
    /// \param[in] _evt The drag event.
    protected: void dragEnterEvent(QDragEnterEvent *_evt) override;

    /// \brief Used to accept drop events.
    /// \param[in] _evt The drop event.
    protected: void dropEvent(QDropEvent *_evt) override;

    /// \brief Qt callback when a key is pressed.
    /// \param[in] _event Qt key event.
    protected: virtual void keyPressEvent(QKeyEvent *_event) override;

    /// \brief Qt callback when the mouse is released.
    /// \param[in] _event Qt mouse event.
    protected: void mouseReleaseEvent(QMouseEvent *_event) override;

    /// \brief Helper function to check whether the drag action is valid.
    /// \param[in] _evt The drag event.
    /// \return True if the drag action is valid.
    private: bool IsDragValid(const QDropEvent *_evt) const;

    /// \brief Qt signal emitted when a variable is added to the container
    /// \param[in] _id Unique id of the added variable pill.
    /// \param[in] _name Name of variable pill added.
    /// \param[in] _targetId Unique id of the target variable pill that this
    /// variable is added to. VariablePill::EmptyVariable if it is added to the
    /// container and not a multi-variable pill.
    signals: void VariableAdded(const unsigned int _id,
                                const std::string &_name,
                                const unsigned int _targetId);

    /// \brief Qt signal emitted when a variable is removed from the container.
    /// \param[in] _id Unique id of the variable pill which was removed.
    /// \param[in] _targetId Unique id of the variable pill from which the
    /// previous variable was removed from. VariablePill::EmptyVariable if it
    /// was removed directly from the container and not a multi-variable pill.
    signals: void VariableRemoved(const unsigned int _id,
                                  const unsigned int _targetId);

    /// \brief Qt signal emitted when a variable is moved into the container.
    /// \param[in] _id Unique id of the variable pill.
    /// \param[in] _targetId Unique id of the target variable pill that this
    /// variable has moved to. VariablePill::EmptyVariable if it moved to the
    /// container and not a multi-variable pill.
    signals: void VariableMoved(const unsigned int _id,
                                const unsigned int _targetId);

    /// \brief Qt signal emitted when a variable label has changed.
    /// \param[in] _id Unique id of the variable pill.
    /// \param[in] _label New variable label.
    signals: void VariableLabelChanged(const unsigned int _id,
                                       const std::string &_label);

    /// \brief Qt Callback when a variable has been added to another variable.
    /// \param[in] _id Unique id of the added variable.
    /// \param[in] _label Name of the added variable.
    private slots: void OnAddVariable(const unsigned int _id,
                                      const std::string &_label);

    /// \brief Qt Callback when a variable has been removed.
    /// \param[_id] _id Unique id of the removed variable.
    private slots: void OnRemoveVariable(const unsigned int _id);

    /// \brief Qt Callback when a variable has moved into another variable.
    /// \param[_id] _id Unique id of the variable that has moved.
    private slots: void OnMoveVariable(const unsigned int _id);

    /// \brief Qt Callback when a variable label has changed.
    /// \param[_id] _label New variable label.
    private slots: void OnSetVariableLabel(const std::string &_label);

    /// \internal
    /// \brief Private data pointer.
    private: std::unique_ptr<VariablePillContainerPrivate> dataPtr;
  };
}
}
#endif
