//                                               -*- C++ -*-
/**
 *  @brief QwtPlot for Morris plot
 *
 *  Copyright 2015-2024 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "persalys/MorrisPlot.hxx"

#include <QMenu>
#include <QMouseEvent>

#include <qwt_plot_picker.h>
#include <qwt_scale_widget.h>
#include <qwt_picker_machine.h>
#include <qwt_legend.h>

using namespace OT;

namespace PERSALYS
{

MorrisPlot::MorrisPlot(const QString &plotTypeName, const QPointF& initialMarkersCoord, QWidget *parent)
  : PlotWidget(plotTypeName, false, parent)
  , verticalMarker_(new QwtPlotMarker)
{
  setContextMenuPolicy(Qt::NoContextMenu);
  insertLegend(new QwtLegend, QwtPlot::RightLegend);
  setAxisTitle(QwtPlot::xBottom, tr("µ*"));

  // vertical marker: no effect boundary
  verticalMarker_->setValue(initialMarkersCoord);
  verticalMarker_->setLineStyle(QwtPlotMarker::VLine);
  verticalMarker_->setTitle(tr("No effect boundary"));
  verticalMarker_->setLinePen(QPen(Qt::darkGreen, 2, Qt::DashLine));
  verticalMarker_->setItemAttribute(QwtPlotItem::Legend, true);
  verticalMarker_->attach(this);

  // mouse right click event filter to change the vertical marker position
  QwtScaleWidget * scaleWidget = axisWidget(QwtPlot::xBottom);
  scaleWidget->installEventFilter(this);
  scaleWidget->setToolTip(tr("Selecting a value at the scale will move the vertical marker."));

  // picker to select points with the right button
  QwtPlotPicker * picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::RectRubberBand, QwtPicker::AlwaysOn, canvas());
  picker->setStateMachine(new QwtPickerDragRectMachine);
  picker->setMousePattern(QwtEventPattern::MouseSelect1, Qt::RightButton);

  connect(picker, SIGNAL(selected(QRectF)), this, SLOT(selectPoints(QRectF)));

  // - two fake markers to add items in the legend
  fakeSelectedMarker_ = new FakePlotMarker(this, QPen(Qt::blue));
  fakeUnselectedMarker_ = new FakePlotMarker(this, QPen(Qt::red));
}


void MorrisPlot::selectPoints(const QRectF& rect)
{
  const double left = std::min(rect.left(), rect.right());
  const double right = std::max(rect.left(), rect.right());
  const double top = std::max(rect.top(), rect.bottom());
  const double bottom = std::min(rect.top(), rect.bottom());

  const QwtPlotItemList& plotItemList = itemList();
  QVector< QPointF > selectedPoints;
  QVector< QwtPlotMarker* > selectedMarkers;

  for (QwtPlotItemIterator it = plotItemList.begin(); it != plotItemList.end(); ++it)
  {
    if ((*it)->rtti() == QwtPlotItem::Rtti_PlotMarker)
    {
      PlotMarker * m = dynamic_cast<PlotMarker *>(*it);
      if (m)
      {
        const double xP = m->xValue();
        const double yP = m->yValue();

        // if the point is in rect
        if (yP <= top && yP >= bottom && xP <= right && xP >= left)
        {
          selectedMarkers.append(m);
          selectedPoints.append(m->value());
        }
      }
    }
  }
  // if there is at least one point in the rectangle
  if (selectedMarkers.size())
  {
    // highlight the selected points
    QwtPlotCurve selectedPointCurve;
    selectedPointCurve.setSymbol(new QwtSymbol(QwtSymbol::Ellipse, Qt::magenta, QPen(Qt::magenta), QSize(7, 7)));
    selectedPointCurve.setStyle(QwtPlotCurve::NoCurve);
    selectedPointCurve.setItemAttribute(QwtPlotItem::Legend, false);

    selectedPointCurve.setSamples(selectedPoints);
    selectedPointCurve.attach(this);

    replot();

    // context menu to choose to de/select the points
    QMenu contextMenu(this);
    // actions of the context menu
    QAction * deselectPointsAction = new QAction(tr("Deselect the points"), &contextMenu);
    QAction * selectPointsAction = new QAction(tr("Select the points"), &contextMenu);
    contextMenu.addAction(deselectPointsAction);
    contextMenu.addAction(selectPointsAction);

    const QPoint pos(transform(QwtPlot::xBottom, left), transform(QwtPlot::yLeft, bottom));
    QAction * action = contextMenu.exec(mapToGlobal(pos));

    // emit signal to the window to update the data
    if (action == deselectPointsAction || action == selectPointsAction)
    {
      const QPen markerPen = (action == deselectPointsAction ? QPen(Qt::red) : QPen(Qt::blue));
      for (int i = 0; i < selectedMarkers.size(); ++i)
        selectedMarkers[i]->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, markerPen, QSize(5, 5)));
      emit selectedPointsChanged();
    }
    selectedPointCurve.detach();

    replot();
  }
}


void MorrisPlot::updateVerticalMarkerValue(const QPointF& markerValue)
{
  verticalMarker_->setValue(markerValue);
}


void MorrisPlot::updateFakeSelectedMarkerTitle(const QString &title)
{
  fakeSelectedMarker_->setTitle(title);
}


void MorrisPlot::updateFakeUnSelectedMarkerTitle(const QString &title)
{
  fakeUnselectedMarker_->setTitle(title);
}
}
