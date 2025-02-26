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
import QtQuick 2.9
import QtCharts 2.2
import QtQuick.Controls 2.2

import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

Rectangle {
  id: main

  /**
    subscribe to a field || register the chart to the subscribed field
  */
  signal subscribe(real Id, string topic, string path);
  /**
    unsubscribe from a field || unregister the chart from the subscribed field
  */
  signal unSubscribe(real Id, string topic, string path);
  /**
    register the chart to the component attribute
  */
  signal componentSubscribe(string entity, string typeId, string type, string attribute, real Id);
  /**
    unregister the chart from the component attribute
  */
  signal componentUnSubscribe(string entity, string typeId, string attribute, real Id);
  /**
    chart is clicked to swap the chart mode
    from small chart to the main chart in multi charts mode
    Id: chartID
  */
  signal clicked(real Id);

  /**
    Points Limitation: max points of each series
    When points exceed that limit, some points from begining are deleted
  */
  property int maxPoints: 10000
  /**
    Chart ID
  */
  property int chartID: -1
  /**
    True if the chart is a small chart in the multi charts mode
  */
  property bool multiChartsMode: false

  /**
    add point to a field graph
    _fieldID field key or path
    _x x coordinate of the point
    _y y coordinate of the point
  */
  function appendPoint(_fieldID, _x, _y)
  {
    chart.appendPoint(_fieldID, _x, _y);
  }
  /**
    set the chart opacity
    _opacity opacity value
  */
  function setChartOpacity(_opacity)
  {
    chart.opacity = _opacity;
  }
  /**
    move chart to the right
  */
  function moveChart()
  {
    chart.scrollRight(chart.width * shiftAmount.value);
  }
  /**
    change the PlotArea size to fill the chart or the reverse case
  */
  function fillPlotInOrOut()
  {
    if (multiChartsMode)
    {
      chart.legend.visible = false
      chart.margins.top = 0
      chart.margins.bottom = 0
      chart.margins.right = 0
      chart.margins.left = 0
    }
    else
    {
      chart.legend.visible = true
      chart.margins.top = 20
      chart.margins.bottom = 20
      chart.margins.right = 20
      chart.margins.left = 20
    }
  }

  /**
    get the chart object
  */
  function getChart()
  {
    return chart;
  }

  /**
    fix OpenGL Disappear problem when the plugin is docked
  */
  function fixOpenGL()
  {
    lineSeries.useOpenGL = false;
    lineSeries.useOpenGL = true;
  }


  color: "transparent"

  // =============== Fields info Rectangle ================
  Rectangle {
    id: infoRect

    /**
      Handle Dropping Plot Data (Fields or Components)
      text dropped text of the dragged item
    */
    function onDrop(text)
    {
      // topic and path is separated with ','
      if (text.search(",") === -1)
        return;

      // check if the dropped item is component
      if (infoRect.isComponentDrop(text))
      {
        var textList = text.split(",");
        var entity = textList[1];
        var typeId = textList[2];
        var type = textList[3];
        var attribute = textList[4];
        var typeName = textList[5];

        var componentID = entity + "," + typeId + "," + attribute;
        var displayText = entity + "," + typeName + "," + attribute;

        componentSubscribe(entity, typeId, type, attribute, chartID);

        // if the field is already attached
        if (componentID in chart.serieses)
          return;

        chart.addSeries(componentID, displayText);
        infoRect.addComponent(entity, typeId, type, attribute, typeName, displayText);
      }
      // the dropped item is a field
      else
      {
        var topic_path = text.split(",");
        var topic = topic_path[0];
        var path = topic_path[1];

        // Field Full Path ID
        var ID = topic + "-" + path;

        // attach the chart to the subscribed field
        subscribe(chartID, topic, path);

        // if the field is already attached
        if (ID in chart.serieses)
          return;

        // add axis series to plot the field
        chart.addSeries(ID, "");

        // add field info component
        infoRect.addField(ID, topic, path);
      }
      guideText.visible = false;
    }

    /**
      add field to the fields layout
      ID Chart ID
      topic topic name
      path field key or path
    */
    function addField(ID, topic, path)
    {
      var field = fieldInfo.createObject(row);
      field.width = 150;
      field.height = Qt.binding( function() {return infoRect.height * 0.8} );
      field.y = Qt.binding( function()
        {
          if (infoRect.height)
            return (infoRect.height - field.height)/2;
          else
            return 0;
        }
      );

      // update field data
      field.topic = topic;
      field.path = path;
      field.type = "Field"
    }
    /**
      add component to the chart
      entity entity ID
      typeId type ID
      type type of the component attribute (Pose3d, Vector3d .. etc)
    */
    function addComponent(entity, typeId, type, attribute, typeName, displayText)
    {
      var _component = fieldInfo.createObject(row);
      _component.width = 150;
      _component.height = Qt.binding( function() {return infoRect.height * 0.8} );
      _component.y = Qt.binding( function()
        {
          if (infoRect.height)
            return (infoRect.height - _component.height)/2;
          else
            return 0;
        }
      );

      _component.entity = entity;
      _component.typeId = typeId;
      _component.componentType = type;
      _component.attribute = attribute;
      _component.typeName = typeName;
      _component.displayText = displayText;

      _component.type = "Component";
    }

    /**
      True if the dropped text has the component format
      dropText the text dropped in the field info rect
    */
    function isComponentDrop(dropText)
    {
      var textList = dropText.split(",");
      if (textList.length < 6)
        return false;
      if (textList[0] !== "Component")
        return false;

      return true;
    }

    width: parent.width
    height: (multiChartsMode) ? 0 : 50
    color: (Material.theme == Material.Light) ? Material.color(Material.Grey,Material.Shade200)
                                              : Material.color(Material.BlueGrey, Material.Shade800)

    Text {
      id: guideText
      text: qsTr("Drag & Drop Plottable Fields | Components")
      anchors.centerIn: parent
      color: (Material.theme == Material.Light) ? "gray" : "white"
      opacity: 0.3
      visible: (multiChartsMode) ? false : true
    }

    // make it scrolable
    ScrollView {
      anchors.fill: parent
      ScrollBar.horizontal.policy: ScrollBar.AsNeeded
      ScrollBar.vertical.policy: ScrollBar.AlwaysOff
      clip: true
      // Horizontal Layout for the fields
      Row {
        anchors.fill: parent
        id:row
        spacing: 10
      }
    }

    DropArea {
      anchors.fill: parent
      onDropped:
      {
        var text = drop.getDataAsString("text/plain");
        infoRect.onDrop(text);
      }
    }
  }

  // ================ Field / Component ====================
  Component {
    id: fieldInfo
    Rectangle {
      id: component

      /**
        Field or Component
      */
      property string type: ""

      /**
        field data:
        topic name
        path field key
      */
      property string topic: ""
      property string path: ""

      /**
        component data:
        entity entity ID
        typeId type ID
        type type of the component attribute (Pose3d, Vector3d .. etc)
        attribute (X, Y, Z, Roll, ... etc)
      */
      property string entity: ""
      property string typeId: ""
      property string componentType: ""
      property string attribute: ""
      property string typeName: ""
      property string componentId: entity + "," + typeId + "," + attribute;
      property string displayText: ""

      /**
        set the field name text
      */
      function setText(text) {
          fieldname.text = text;
      }
      signal unsubscribe(string topic, string path);

      radius: width/4
      Rectangle {
        height: parent.height
        width: parent.width
        radius: width/4
        color: Material.accentColor
        clip: true

        MouseArea {
          id : fieldInfoMouse
          anchors.fill: parent
          hoverEnabled: true
          onEntered: enterAnimation.start();
          onExited: exitAnimation.start();
        }

        Text {
          id: fieldname
          text: (component.type === "Field") ? component.topic + "/"+ component.path :
                (component.type === "Component") ? component.entity + "," + component.typeName
                                                   + "," + component.attribute : ""
          color: "white"
          elide: Text.ElideRight
          width: parent.width * 0.9
          anchors.verticalCenter: parent.verticalCenter
          leftPadding: 20;
        }

        ToolTip {
          delay: 1000
          timeout: 2000
          text: (component.type === "Field" ) ? component.topic + "-"+ component.path :
                (component.type === "Component") ? "entity: " + component.entity + "\n" +
                                                    "typeId: " + component.typeId + "\n" +
                                                    "typeName: " + component.typeName + "\n" +
                                                    "dataType: " + component.componentType + "\n" +
                                                    "attribute: " + component.attribute : ""
          visible: fieldInfoMouse.containsMouse
          y: fieldInfoMouse.mouseY
          x: fieldInfoMouse.mouseX
          enter: null
          exit: null
        }
      }

      Rectangle {
        id: exitBtn
        radius: width / 2
        height: parent.height * 0.8;
        width: height
        color: "red"
        opacity: 0
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: parent.width * 0.05
        Text { anchors.centerIn: parent; text: "x"; color: "white"}

        MouseArea {
          anchors.fill: parent
          onClicked: {
            exitAnimation.start();

            // unSubscribe from the transport / Component
            if (component.type === "Field")
              main.unSubscribe(main.chartID, component.topic, component.path);

            else if (component.type === "Component")
              main.componentUnSubscribe(component.entity, component.typeId,
                                          component.attribute, main.chartID)


            // delete the series points and deattache it from the chart
            if (component.type === "Field")
              chart.deleteSeries(component.topic + "-" + component.path)

            else if (component.type === "Component")
              chart.deleteSeries(component.componentId);

            // delete the field info component
            component.destroy();
          }
        }
        NumberAnimation {
          id: enterAnimation
          target: exitBtn; property: "opacity"; duration: 100
          easing.type: Easing.InOutQuad; from: 0; to: 1;
        }
        NumberAnimation {
          id: exitAnimation
          target: exitBtn; property: "opacity"; duration: 100;
          easing.type: Easing.InOutQuad; from: 0.85; to: 0;
        }
      }
    }
  }

  // ================== Chart ============================
  ChartView {
    id : chart
    /**
      all serieses, field path is the key, series is the value
    */
    property var serieses: ({})
    /**
      colors to give the fields different colors
    */
    property var colors: (Material.theme == Material.Light) ?
                          ["red", "#004c6d", "#FF5D8C","#903f5c", "#7ec92a", "#a88f31", "#ff7c43" , "#62efff"] :
                          ["#ffa600", "#7aa6c2", "#f04c6d", "#bc5090", "#a88f31", "#7a5195", "#ffbcc3", "#7ec92a"];
    /**
      current index of colors array
    */
    property int indexColor: 0

    /**
      get sereieses
      return: map of serieses <series id, series object>
    */
    function getAllSerieses()
    {
      return serieses;
    }

    /**
      update the text that shows the hover point value (x,y) on the mouse cursor
    */
    function updateHoverText()
    {
      if (!hoverText.visible)
        return;

      var axisWidth = xAxis.max - xAxis.min;
      var axisHeight = yAxis.max - yAxis.min;
      var xPos = xAxis.min + ( (chartMouse.mouseX - chart.plotArea.x) / chart.plotArea.width ) * axisWidth;
      var yPos = yAxis.max - ( (chartMouse.mouseY - chart.plotArea.y) / chart.plotArea.height) * axisHeight;
      hoverText.text =  "(" + xPos.toFixed(2).toString() + ", " + yPos.toFixed(2).toString() + ")";
      hoverText.x = chartMouse.mouseX + 12;
      hoverText.y = chartMouse.mouseY;
    }

    /**
      add new series
      ID key of the series: path of the field of the series
    */
    function addSeries(ID, seriesDisplayText) {
      var seriesName = (seriesDisplayText) ? seriesDisplayText : ID
      var newSeries = createSeries(ChartView.SeriesTypeLine, seriesName, xAxis, yAxis);
      newSeries.useOpenGL = true;
      newSeries.width = 2;
      newSeries.color = chart.colors[chart.indexColor % chart.colors.length]
      serieses[ID] = newSeries;

      chart.indexColor = (chart.indexColor + 1)  % chart.colors.length;
    }

    /**
      delete a field series by its ID
      ID field path
    */
    function deleteSeries(ID) {
      // remove the points of the series from the chart
      removeSeries(serieses[ID]);
      // remove the series key from the serieses map
      delete serieses[ID];
    }

    /**
      add point to a specific TextField
      _fieldID field ID or Path
      _x x of the point
      _y y of the point
    */
    function appendPoint(_fieldID, _x, _y)
    {
      if (!chart.serieses[_fieldID])
        return;

      // if this is the first point (if the chart is empty):
      // set the min/max according to that point's coordinates
      // note: count == 2: because chart has 1 series by default to show plotting grid
      if (chart.count === 2 && chart.serieses[_fieldID].count === 0)
      {
        xAxis.min = _x;
        xAxis.max = _x + 10;
        chart.serieses[_fieldID].append(_x, _y);
        return;
      }

      // expand the chart boundries if needed
      if (xAxis.max  < _x)
      {
        xAxis.max = _x;
        chart.scrollRight(chart.width * 0.0012);
      }

      if (yAxis.max  < _y)
        yAxis.max = _y ;
      if (yAxis.min > _y)
        yAxis.min = _y ;
      if (xAxis.min > _x)
        xAxis.min = _x ;

      // add the point
      chart.serieses[_fieldID].append(_x, _y);

      // delete the oldest point to limit the points size
      if (chart.serieses[_fieldID].count > maxPoints)
          chart.serieses[_fieldID].removePoints(0,1)

      chart.updateHoverText();
    }

    width: parent.width
    anchors.bottom: parent.bottom
    anchors.top: infoRect.bottom

    // animations
    antialiasing: true
    opacity: 1
    backgroundRoundness: 10
    animationOptions: ChartView.NoAnimation

    theme: (Material.theme == Material.Light) ? ChartView.ChartThemeLight: ChartView.ChartThemeDark

    Text {
      id:hoverText
      visible: (chartMouse.flag && !multiChartsMode && chartMouse.containsMouse) ? true : false
      color: (Material.theme == Material.Light) ? "black" : Material.color(Material.Grey,Material.Shade200)
    }

    MouseArea {
      id:chartMouse
      /**
        flag to show the hover text
        True if the user clicked on the CheckBox
      */
      property bool flag: (hoverCheckBox.checkState === Qt.Checked) ? true : false
      /**
        xHold is the value of x when the user press and hold the mouse to move the plot view
      */
      property double xHold: 0
      /**
        xHold is the value of y when the user press and hold the mouse to move the plot view
      */
      property double yHold: 0

      anchors.fill:parent
      hoverEnabled: true
      cursorShape: (multiChartsMode) ? Qt.PointingHandCursor : Qt.ArrowCursor

      onEntered: {
        if (multiChartsMode)
          chart.opacity = 0.7;
      }
      onExited: {
        if (multiChartsMode)
          chart.opacity = 1;
      }
      onPressed: {
        xHold = mouseX;
        yHold = mouseY;
      }

      /**
        Drag/Move the plot view
      */
      onPositionChanged: {
        if (multiChartsMode)
          return

        if (pressed)
        {
          chart.scrollLeft(mouseX - xHold)
          chart.scrollUp(mouseY - yHold)

          xHold = mouseX
          yHold = mouseY
        }
        else
          chart.updateHoverText();

      }

      onClicked: {
        main.clicked(chartID);
      }

      /**
        zoom shift amount
      */
      property double shift: 15

      /**
        Zoom
      */
      onWheel:{
        if (multiChartsMode)
          return

        // the center of the plot
        var centerX = chart.plotArea.x + chart.plotArea.width/2
        var centerY = chart.plotArea.y + chart.plotArea.height/2

        // the percentage of the mouseX = how it moves far away from the plot center
        // ex: if the the plot width = 100 & mouseX = 75, so it moves the 50% away from the center (75-50)/50
        var factorX = (wheel.x - centerX) / (chart.plotArea.width/2); // %
        // same for y but with mouseY, centerY and Height
        var factorY = (wheel.y - centerY) / (chart.plotArea.height/2); // %


        var zoomType;
        if( wheel.angleDelta.y > 0)
          // zoomIn
          zoomType = 1;
        else
          // zoomOut
          zoomType = -1;


        // plot size (width & height) will always increase/decrese by 2*shift
        // (imagine the size is centered with shift distance at both sides of width (same of height) )

        // the location of zooming is determine by changing the x,y (top left corner) of the zoom rect
        // x,y increase/decrease
        var rect = Qt.rect(chart.plotArea.x + (factorX + 1) * shift * zoomType,
                          chart.plotArea.y + (factorY + 1) * shift * zoomType,
                          chart.plotArea.width  - 2 * shift * zoomType,
                          chart.plotArea.height - 2 * shift * zoomType
                          );

        chart.zoomIn(rect);
      }
    }

    DropArea {
      anchors.fill: parent
      onDropped:
      {
        var text = drop.getDataAsString("text/plain");
        infoRect.onDrop(text);
      }
    }

    ValueAxis {
      id : yAxis
      min: 0;
      max: 3;
      tickCount: 9
    }

    ValueAxis {
      id : xAxis
      min: 0
      max: 3
      tickCount: 9
    }

    // to just show the plot at begining
    LineSeries {
      id: lineSeries
      axisX: xAxis
      axisY: yAxis
      visible: false
      useOpenGL: true
    }

    Text {
      id: plotName
      text: "Plot " + chartID.toString()
      font.pointSize: 15
      anchors.left: parent.left
      anchors.top: parent.top
      anchors.margins: 20
      color: Material.color(Material.Grey, Material.Shade500)
      visible: (multiChartsMode) ? false : true
    }

    Text {
      id: hoverName
      anchors.centerIn: parent
      visible: (multiChartsMode && chartMouse.containsMouse)
      text: plotName.text
      color: plotName.color
      font.pointSize: plotName.font.pointSize
    }
  }

  CheckBox {
    id: hoverCheckBox;
    visible: (main.multiChartsMode) ? false : true
    checkState: Qt.Unchecked
    anchors.right: chart.right
    anchors.top: chart.top
    anchors.margins: 20
    text: "hover"
  }
}
