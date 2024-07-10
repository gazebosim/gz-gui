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
import QtQuick
import QtCharts
import QtQuick.Controls
import QtQuick.Controls.Styles
import QtQuick.Controls.Material
import QtQuick.Layouts
import Qt.labs.platform
import "qrc:/gz/gui/qml"

Rectangle
{
  id : main
  /**
  charts map < chart id, chart object>
  */
  property var charts: ({})
  /**
  id of the main chart of the view (the one with biggest size)
  */
  property int mainChartID: 1

  /**
  variable to create thd IDs of the charts
  */
  property int idIncrementor: 0

  /**
  True if the user has many charts (more than 2)
  False if it has one chart or 2
  used to customize some UI features based on the mode
  */
  property bool multiChartsMode: false
  /**
  export window
  */
  property var window: null

  /**
  add new chart to the view
  */
  function addChart()
  {
    main.idIncrementor++;

    var chartComponent = Qt.createComponent("Chart.qml");
    var chartObject;
    // if the mode is many charts that are organized in horizontal layout
    if (multiChartsMode)
    {
      // add it to the horizontal layout
      chartObject = chartComponent.createObject(rowChartsLayout);
      chartObject.width = 200;
      chartObject.height = Qt.binding( function() {return rowCharts.height * 0.9} );
      chartObject.y = Qt.binding( function() {return (rowCharts.height - chartObject.height)/2} );
      chartObject.multiChartsMode = true;
      chartObject.fillPlotInOrOut()
    }
    else
    {
      // if normal mode, add the chart to the normal vertical layout
      chartObject = chartComponent.createObject(chartsLayout);
      chartObject.height = Qt.binding( function() {return chartsLayout.height / chartsLayout.heightFactor});
      chartObject.width = Qt.binding( function() {return chartsLayout.width});
      // to change the height of each vertical chart
      chartsLayout.heightFactor ++ ;
    }

    // Chart ID
    chartObject.chartID = main.idIncrementor;
    charts[idIncrementor] = chartObject;
    // Signals and Slots
    chartObject.subscribe.connect(main.onSubscribe);
    chartObject.unSubscribe.connect(main.onUnSubscribe);
    chartObject.componentSubscribe.connect(main.onComponentSubscribe);
    chartObject.componentUnSubscribe.connect(main.onComponentUnSubscribe);
    chartObject.clicked.connect(main.onClicked);
  }

  /**
  reallocate charts position, called at moving to multi charts mode
  */
  function reallocateChart(_chart)
  {
    // new attributes to fit in the horizontal layout
    _chart.width = 200;
    _chart.height = Qt.binding( function() {return rowCharts.height * 0.9} );
    _chart.y = Qt.binding( function() {return (rowCharts.height - _chart.height)/2} );
    // enable that to enable the some UI features in the chart
    _chart.multiChartsMode = true;
    _chart.fillPlotInOrOut();

    // change the layout from the vertical layout to the top horizontal one
    _chart.parent = rowChartsLayout;
  }

  function onSubscribe(_Id, _topic, _path)
  {
    PlottingIface.subscribe(_Id, _topic, _path);
  }
  function onUnSubscribe(_Id, _topic, _path)
  {
    PlottingIface.unsubscribe(_Id, _topic, _path);
  }
  function onComponentSubscribe(entity, typeId, type, attribute, Id)
  {
    PlottingIface.onComponentSubscribe(entity, typeId, type, attribute, Id);
  }
  function onComponentUnSubscribe(entity, typeId, attribute, Id)
  {
    PlottingIface.onComponentUnSubscribe(entity, typeId, attribute, Id);
  }

  /**
  on chart onClicked:
  change the main chart to be the clicked chart
  and swap it with the prev main chart
  Id: id of the clicked chart
  */
  function onClicked(Id)
  {
    // if many charts mode & the selected chart is in the horizontal layout
    if (multiChartsMode && charts[Id].multiChartsMode)
    {
      // ======= main chart =========
      // change the main charts properties to fit in the horizontal layout
      charts[mainChartID].width = 200;
      charts[mainChartID].height = Qt.binding( function() {return rowCharts.height * 0.9} );
      charts[mainChartID].y = (rowCharts.height - charts[mainChartID].height)/2;
      charts[mainChartID].multiChartsMode = true;
      charts[mainChartID].fillPlotInOrOut();

      // swap the main chart with the position of the clicked chart
      charts[mainChartID].parent = rowChartsLayout;

      // ======= swapped chart =======
      charts[Id].parent = chartsLayout;
      charts[Id].x = charts[mainChartID].x;
      charts[Id].y = 0;
      charts[Id].height = Qt.binding( function() {return chartsLayout.height / chartsLayout.heightFactor});
      charts[Id].width = Qt.binding( function() {return chartsLayout.width});
      charts[Id].multiChartsMode = false;
      charts[Id].fillPlotInOrOut();
      charts[Id].setChartOpacity(1);

      mainChartID = Id;
    }
  }

  /**
    fix the OpenGL disappear problem when the plugin is docked
  */
  function fixOpenGL()
  {
    Object.keys(main.charts).forEach(function(key) {
      main.charts[key].fixOpenGL();
    });
  }

  /**
  plot point to a chart
  _chart: chart id
  _fieldID: field path or id
  _x: x coordinates of the point
  _y: y coordinates of the point
  */
  function handlePlot(_chart, _fieldID, _x, _y)
  {
    charts[_chart].appendPoint(_fieldID, _x, _y);
  }

  Connections {
    target: PlottingIface
    onPlot : handlePlot(_chart, _fieldID, _x, _y);
  }


  Layout.minimumWidth: 600
  Layout.minimumHeight: 600
  anchors.fill: parent
  color: (Material.theme == Material.Light) ? Material.color(Material.Grey,Material.Shade100) : Material.background

  // when the chart is docked fix the OpenGL Disappear problem
  onWidthChanged: {
    main.fixOpenGL();
  }

  // Horizonal Layout to hold multi charts (small charts)
  Rectangle {
    id: rowCharts
    width: parent.width
    height: (multiChartsMode) ? 150 : 0
    color: (Material.theme == Material.Light) ? Material.color(Material.Grey,Material.Shade200)
                        : Material.color(Material.BlueGrey, Material.Shade800)
    // Make it Scrolable
    ScrollView {
      anchors.fill: parent
      ScrollBar.horizontal.policy: ScrollBar.AsNeeded
      ScrollBar.vertical.policy: ScrollBar.AlwaysOff
      clip: true
      // Horizontal Layout for the Charts
      Row {
        id:rowChartsLayout
        anchors.fill: parent
        spacing: 10
      }
    }
  }

  // Vertical Layout to hold Main Charts (1 or 2 charts)
  Column {
    id : chartsLayout

    /**
    factor to customize the height of the Charts based on their number
    */
    property int heightFactor: 0

    width: parent.width
    anchors.topMargin: 10
    anchors.top: rowCharts.bottom
    anchors.bottom: parent.bottom
  }

  Rectangle {
    id : addBtn

    anchors.right: openExport.left
    anchors.top: parent.top
    anchors.margins: 15

    width: 40
    height: 40
    radius: width/2
    color: Material.accentColor
    Text {
      text:  "+"
      font.weight: Font.Bold
      font.pixelSize: parent.width/2
      color: "white"
      anchors.centerIn: parent
    }

    MouseArea {
      id: mouseAddBtn
      anchors.fill: parent
      hoverEnabled: true
      onEntered: { addBtn.opacity = 0.8; cursorShape = Qt.PointingHandCursor; }
      onExited: { addBtn.opacity = 1; cursorShape =  Qt.ArrowCursor; }

      onClicked: {
        if (Object.keys(charts).length == 2)
        {
          multiChartsMode = true;
          var firstChart = true;
          for (var i = 0; i < chartsLayout.children.length; i++)
          {
            // skip the first one
            if (firstChart)
            {
              firstChart = false;
              // make that chart has a full size
              chartsLayout.heightFactor = 1
              continue;
            }
            var chart = chartsLayout.children[i];
            reallocateChart(chart);
          }
        } // end if

        addChart();
      }
    }

    ToolTip.text: "Add Chart"
    ToolTip.delay: 500
    ToolTip.timeout: 1000
    ToolTip.visible: mouseAddBtn.containsMouse
  }

  Component.onCompleted: {
    addChart();
  }

  ToolButton {
    id: openExport
    width: 40;
    height: 40;
    anchors.right: parent.right
    anchors.top: parent.top
    anchors.margins: 15
    onHoveredChanged: (opacity === 1) ? opacity = 0.8 : opacity = 1;

    background: Rectangle{
      id: background
      anchors.fill: parent
      radius: width/2 // circle

      color: "transparent"
      border.width: 1
      border.color: Material.color(Material.Grey, Material.Shade500)
    }

    Image {
      id: exportIcon
      /**
      size factor
      */
      property double factor: 0.65
      width: background.width * factor;
      height: background.height * factor;
      anchors.centerIn: background
      sourceSize.width: background.width * factor;
      sourceSize.height: background.height * factor;
      source: "images/export_icon.png"
    }

    ToolTip.text: "Export";
    ToolTip.visible: openExport.hovered
    ToolTip.delay: 500
    ToolTip.timeout: 1000

    onClicked: {
      if (main.window)
        main.window.destroy()

      main.window = exportWindow.createObject();
      main.window.setCharts(main.charts);
      main.window.setBackgroundColor(main.color);
      main.window.setPrimaryColor(Material.primaryColor)
      main.window.show();
    }
  }

  Component {
    id : exportWindow
    ApplicationWindow {
      id: exportApp

      /**
      flag indicated if the chart is in small mode
      */
      property bool isSmallChart: false
      /**
      charts objects that is loaded in the export window
      */
      property var charts: []
      /**
      index of the selected chart
      */
      property int index: 0

      /**
      charts images that is displayed in the export window
      */
      property var chartImages: []

      /**
      add chart copy/image to the export window
      */
      function addChartCopy(result)
      {
        var imageObject = chartCopy.createObject(grid);
        imageObject.setSource(result.url);

        imageObject.setChartName("Plot" + charts[index].chartID.toString());
        imageObject.chartIndex = index;

        chartImages.push(imageObject);

        index++;
        continueCopy();
      }

      /**
      set/add all the charts in the PlottingInterface to the export window
      */
      function setCharts(_charts)
      {
        // clear
        charts = []
        chartImages = []

        Object.keys(_charts).forEach(function(key) {
          charts.push(_charts[key]);
        });

        continueCopy();
      }

      /**
      continue the process of the copy
      Grabbing image takes time and we should continue after the image callback
      */
      function continueCopy()
      {
        if (index < charts.length)
          charts[index].getChart().grabToImage(addChartCopy);
      }

      /**
      export all selected charts in the export window to that path
      */
      function exportCSV(path)
      {
      for (var i = 0; i < chartImages.length; i++)
      {
        if (!chartImages[i].isSelected())
          continue;

        var chart = charts[chartImages[i].chartIndex];
        var serieses = chart.getChart().getAllSerieses();
        var chart_id = chart.chartID;

        if (Object.keys(serieses).length === 0)
          continue;


        // convert Serieses to Map of {series_name : points list}
        // slots in cpp accepts QMap<QString, QVariant>
        // QVariant could be a list instead of series object

        var chartSerieses = {};
        var seriesArray = [];
        Object.keys(serieses).forEach(function(key) {

          seriesArray = []

          // convert Series to QList<QPointF>
          for (var j =0; j < serieses[key].count; j++)
            seriesArray.push(serieses[key].at(j));

          // add series
          chartSerieses[key] = seriesArray;
        });

        return PlottingIface.exportCSV(path, chart_id, chartSerieses);
      }
      }

      /**
      set the background color of the export widget
      */
      function setBackgroundColor(_color)
      {
      exportWidget.color = _color;
      }

      /**
      set color of the export button
      */
      function setPrimaryColor(_color)
      {
      exportBtn.color = _color;
      }

      width: 1000; height: 600
      title: "Plotting Export"

      Rectangle {
        color: (Material.theme === Material.Light) ? Material.color(Material.Grey, Material.Shade200) :
                              Material.color(Material.Grey, Material.Shade900)
        anchors.fill: parent
        id: exportWidget

        // make it scrolable
        ScrollView {
          width: parent.width
          height: parent.height - exportBtn.height * 1.5
          ScrollBar.vertical.policy: ScrollBar.AlwaysOn
          clip: true

          Grid {
            id: grid
            anchors.fill: parent
            spacing: 10
            padding: 30
            columns: (exportWidget.width)/ 300
          }
        }

        ComboBox {
          id: exportBtn

          property string color: Material.primaryColor

          displayText: "Export to"
          model: ["CSV"]

          background: Rectangle {
            implicitWidth: 120
            implicitHeight: 40
            radius: 10
            color: parent.color;
          }
          anchors.bottom: parent.bottom
          anchors.right: parent.right
          anchors.margins: 10

          onActivated: {
            fileDialog.open();
          }
        }
        Rectangle {
          id: cancelBtn
          color: Material.color(Material.Grey, Material.Shade600);
          implicitWidth: 120
          implicitHeight: 40
          radius: 10
          anchors.bottom: parent.bottom
          anchors.left: parent.left
          anchors.margins: 10
          Text {
            text: "Cancel"
            anchors.centerIn: parent
            color: "white"
          }
          MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.opacity = 0.9; cursorShape: Qt.PointingHandCursor
            onExited: parent.opacity = 1;
            onClicked: exportApp.close()
          }
        }
      }

      FolderDialog {
        id: fileDialog
        title: "Choose a folder"
        visible: false
        options: FolderDialog.ShowDirsOnly

        onAccepted: {
          if (exportBtn.currentText == "CSV")
          {
            var success = exportApp.exportCSV(folder);
            if (success)
              exportApp.close();
          }
        }
        onRejected: fileDialog.close();
      }

      /**
      Image/Copy of the Chart
      */
      Component {
        id: chartCopy
        Column
        {
          /**
          set the image source
          */
          function setSource(url)
          {
            image.source = url;
          }
          /**
          get the source of the grapped image
          */
          function getSource()
          {
            return image.source
          }
          /**
          set displayed name of the chart
          */
          function setChartName(name)
          {
            checkboxText.text = name;
          }
          /**
          check if the chart is selected to export
          True if selected
          */
          function isSelected()
          {
            return checkbox.checked;
          }

          /**
          index of the assosiated chart object in the charts array
          */
          property int chartIndex: 0

          width: 300
          height: 250
          Image {
            id: image
            width: parent.width
            height: parent.height - 50
          }
          CheckBox {
            id: checkbox
            x: image.width / 2 - 50
            checkState: Qt.Unchecked
            Text {
              id: checkboxText
              anchors.left: parent.right
              anchors.verticalCenter: parent.verticalCenter
              color: Material.color(Material.Grey, Material.Shade500)
              leftPadding: 5
            }
          }
        }
      }
    }
  }
}
