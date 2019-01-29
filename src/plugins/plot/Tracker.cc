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

#include <ignition/math/Helpers.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/plugins/plot/Tracker.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

/////////////////////////////////////////////////
Tracker::Tracker(QWidget *_canvas)
  : QwtPlotPicker(_canvas)
{
  this->setTrackerMode(QwtPlotPicker::AlwaysOn);
  this->setRubberBand(QwtPicker::VLineRubberBand);
  this->setStateMachine(new QwtPickerTrackerMachine());
}


/////////////////////////////////////////////////
void Tracker::updateDisplay()
{
  // this updates default rubberband and tracker text
  QwtPicker::updateDisplay();
}

/////////////////////////////////////////////////
void Tracker::widgetMousePressEvent(QMouseEvent *_e)
{
  this->setRubberBand(QwtPicker::NoRubberBand);
  QwtPicker::widgetMousePressEvent(_e);
}

/////////////////////////////////////////////////
void Tracker::widgetMouseReleaseEvent(QMouseEvent *_e)
{
  this->setRubberBand(QwtPicker::VLineRubberBand);
  QwtPicker::widgetMouseReleaseEvent(_e);
}

/////////////////////////////////////////////////
QwtText Tracker::trackerTextF(const QPointF &_pos) const
{
  // format hover text
  QwtText tracker;
  tracker.setColor(Qt::white);
  QColor c("#f0f0f0");
  c.setAlpha(200);
  tracker.setBackgroundBrush(c);
  tracker.setRenderFlags(Qt::AlignLeft);

  QString info;
  const QwtPlotItemList curves =
      this->plot()->itemList(QwtPlotItem::Rtti_PlotCurve);
  for (int i = 0; i < curves.size(); ++i)
  {
    const QString curveInfo = this->CurveInfoAt(
        static_cast<const QwtPlotCurve *>(curves[i]), _pos);

    if (!curveInfo.isEmpty())
    {
      if (!info.isEmpty())
        info += "<br>";
      info += curveInfo;
    }
  }
  tracker.setText(info);
  return tracker;
}

/////////////////////////////////////////////////
QString Tracker::CurveInfoAt(const QwtPlotCurve *curve,
    const QPointF &_pos) const
{
  const QLineF line = this->CurveLineAt(curve, _pos.x());
  if (line.isNull())
    return QString::null;

  // interpolate
  const double y = line.pointAt(
      (_pos.x() - line.p1().x()) / line.dx()).y();

  // return value string in curve color
  QString info("<font color=""%1"">(%2, %3)</font>");
  return info.arg(curve->pen().color().name()).arg(_pos.x()).arg(y);
}

/////////////////////////////////////////////////
QLineF Tracker::CurveLineAt(const QwtPlotCurve *_curve,
    const double _x) const
{
  // line segment of curve at x
  QLineF line;

  if (_curve->dataSize() >= 2)
  {
    const QRectF br = _curve->boundingRect();
    if ((br.width() > 0) && (_x >= br.left()) && (_x <= br.right()))
    {
      // get closest index in the array of points
      int index = this->UpperSampleIndex(*_curve->data(), _x);
      double cmp = _curve->sample(_curve->dataSize() - 1).x();

      if (index == -1 && ignition::math::equal(_x, cmp))
      {
        // the last sample is excluded from UpperSampleIndex
        index = _curve->dataSize() - 1;
      }

      if (index > 0)
      {
        line.setP1(_curve->sample(index - 1));
        line.setP2(_curve->sample(index));
      }
    }
  }
  return line;
}

/////////////////////////////////////////////////
int Tracker::UpperSampleIndex(const QwtSeriesData<QPointF> &_series,
    const double _value) const
{
  // binary search to find index in series with the closest value
  const int indexMax = _series.size() - 1;
  if (indexMax < 0 || _value >= _series.sample(indexMax).x())
    return -1;

  int indexMin = 0;
  int n = indexMax;
  while (n > 0)
  {
    const int half = n >> 1;
    const int indexMid = indexMin + half;
    if (_value < _series.sample(indexMid).x())
    {
      n = half;
    }
    else
    {
      indexMin = indexMid + 1;
      n -= half + 1;
    }
  }
  return indexMin;
}
