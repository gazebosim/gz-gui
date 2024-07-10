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
import QtQml.Models
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

TreeView {
    objectName: "treeView"
    id:tree
    model: TopicsModel

    Layout.minimumHeight: 400
    Layout.minimumWidth: 300
    anchors.fill: parent

    property int itemHeight: 30;

    // =========== Colors ===========
    property color oddColor: (Material.theme == Material.Light) ?
                              Material.color(Material.Grey, Material.Shade100):
                              Material.color(Material.Grey, Material.Shade800);

    property color evenColor: (Material.theme == Material.Light) ?
                               Material.color(Material.Grey, Material.Shade200):
                               Material.color(Material.Grey, Material.Shade900);

    property color highlightColor: Material.accentColor;


    verticalScrollBarPolicy: Qt.ScrollBarAsNeeded
    horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
    headerVisible: false
    headerDelegate: Rectangle {
        visible: false
    }
    backgroundVisible: false;
    TableViewColumn
    {
        role: "name";
    }

    // =========== Selection ===========
    selection: ItemSelectionModel {
        model: tree.model
    }
    selectionMode: SelectionMode.SingleSelection

    // =========== Delegates ============
    rowDelegate: Rectangle
    {
        id: row
        color: (styleData.selected)? highlightColor :
                                     (styleData.row % 2 == 0) ? evenColor : oddColor
        height: itemHeight;
    }

    itemDelegate: Item {
        id: item

        // for fixing the item position
        // item pos changes randomly when drag happens (with the copy drag)
        anchors.top: parent.top
        anchors.right: parent.right

        Drag.mimeData: { "text/plain" : (model === null) ? "" : model.topic + "," + model.path }

        Drag.dragType: Drag.Automatic
        Drag.supportedActions : Qt.CopyAction
        Drag.active: dragMouse.drag.active
        // a point to drag from
        Drag.hotSpot.x: 0
        Drag.hotSpot.y: itemHeight

        // used by DropArea that accepts the dragged items
        function itemData ()
        {
            return {
                "name": model.name,
                "type": model.type,
                "path": model.path,
                "topic": model.topic
            }
        }

        MouseArea {
            id: dragMouse
            anchors.fill: parent

            // only plottable items are dragable
            drag.target: (model === null) ? null :
                         (model.plottable) ? parent : null

            // get a copy image of the dragged item
            onPressed: parent.grabToImage(function(result) {
                parent.Drag.imageSource = result.url
            })

            onReleased:
            {
                // emit drop event to notify the DropArea (must manually)
                parent.Drag.drop();
            }

            hoverEnabled: true
            propagateComposedEvents: true
            // make the cursor with a drag shape at the plottable items
            cursorShape: (model === null) ?  Qt.ArrowCursor : (model.plottable) ?
                                                Qt.DragCopyCursor : Qt.ArrowCursor

            onClicked: {
                // change the selection of the tree by clearing the prev, select a new one
                tree.selection.select(styleData.index,ItemSelectionModel.ClearAndSelect)

                // set the selection index to the index of the clicked item (must set manually)
                tree.selection.setCurrentIndex(styleData.index,ItemSelectionModel.ClearAndSelect)

                // the currentIndex of the tree.selection is not the same
                // of the tree.currentIndex, so set the tree.currentIndex.
                // this is the way to access it as it is read-only
                tree.__currentRow = styleData.row

                // set the focus to the selected item to receive the keyboard events
                // this is useful to enable navigating with keyboard from the right position
                item.forceActiveFocus();

                tree.expandCollapseMsg(tree.currentIndex);
            }
        }

        Image {
            id: icon
            source: "plottable_icon.svg"
            height: itemHeight * 0.6
            width: itemHeight * 0.6
            y : itemHeight * 0.2
            visible: (model === null) ? false : model.plottable
        }

        Text {
            id : field
            text: (model === null) ? "" : model.name
            color: (Material.theme == Material.Light || styleData.selected) ?
                    Material.color(Material.Grey, Material.Shade800):
                    Material.color(Material.Grey, Material.Shade400);

            font.pointSize: 12
            anchors.leftMargin: 5
            anchors.left: icon.right
            anchors.right: parent.right
            elide: Text.ElideMiddle
            y: icon.y
        }

        ToolTip {
            id: tool_tip
            delay: 200
            timeout: 2000
            text: (model === null) ? "Type ?" : "Type: " + model.type;
            visible: dragMouse.containsMouse
            y: -itemHeight
            x: dragMouse.mouseX
            enter: null
            exit: null
        }
    }

    property int y_pos: 0

    style: TreeViewStyle {
        branchDelegate: Rectangle {
          height: itemHeight
          width: itemHeight
          color: "transparent"
          Image {
            id: branchImage
            fillMode: Image.Pad
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            sourceSize.height: itemHeight * 0.4
            sourceSize.width: itemHeight * 0.4
            source: styleData.isExpanded ? "minus.png" : "plus.png"
          }
          MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            propagateComposedEvents: true
            onClicked: {
              mouse.accepted = true

              // set the current index & current selection and active focus for keyboard
              // the reason for that to make the branch selection just like the item selection
              // and to fix the animation as it ueses item selection's info
              tree.selection.select(styleData.index,ItemSelectionModel.ClearAndSelect)
              tree.selection.setCurrentIndex(styleData.index,ItemSelectionModel.ClearAndSelect)
              tree.__currentRow = styleData.row
              item.forceActiveFocus();

              expandCollapseMsg(styleData.index);
            }
          }
        }
    }

    function expandCollapseMsg(index){
        if (tree.isExpanded(index))
            tree.collapse(index)
        else
            tree.expand(index);
    }

    Transition {
        id: expandTransition
        NumberAnimation {
            property: "y";
            from: (tree.__listView.currentItem) ? tree.__listView.currentItem.y : 0;
            duration: 200;
            easing.type: Easing.OutQuad
        }
    }

    Transition {
        id: displacedTransition
        NumberAnimation {
            property: "y";
            duration: 200;
            easing.type: Easing.OutQuad;
        }
    }

    Component.onCompleted: {
        tree.__listView.add = expandTransition;
        tree.__listView.displaced = displacedTransition;
        tree.__listView.removeDisplaced = displacedTransition;
    }

}
