//                                               -*- C++ -*-
/**
 *  @brief QwtPlot for Morris plot
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_MORRISPLOT_HXX
#define PERSALYS_MORRISPLOT_HXX

#include "persalys/PlotWidget.hxx"

#include <qwt_plot_marker.h>

namespace PERSALYS
{

class FakePlotMarker : public QwtPlotMarker
{
public:
  FakePlotMarker(PlotWidget *plotWidget, const QPen pen)
  {
    setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, pen, QSize(5, 5)));
    setItemAttribute(QwtPlotItem::Legend, true);
    setVisible(false);
    attach(plotWidget);
  }
};


class PlotMarker : public QwtPlotMarker
{
public:
  PlotMarker(PlotWidget *plotWidget, const QPen &pen, const QPointF &pt, const QwtText &label)
  {
    setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, pen, QSize(5, 5)));
    setValue(pt);
    setLabel(label);
    setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
    attach(plotWidget);
  }

  void updateSymbolColor(const QPen &pen)
  {
    setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, pen, QSize(5, 5)));
  }
};


class PERSALYS_PLOT_API MorrisPlot : public PlotWidget
{
  Q_OBJECT

public:
  MorrisPlot(const QString &plotTypeName, const QPointF& initialMarkersCoord, QWidget *parent = 0);

public slots:
  void selectPoints(const QRectF&);
  void updateVerticalMarkerValue(const QPointF&);
  void updateFakeSelectedMarkerTitle(const QString &title);
  void updateFakeUnSelectedMarkerTitle(const QString &title);

signals:
  void selectedPointsChanged();
  void selectedPointsChanged(const OT::Indices& ind);

private:
  QwtPlotMarker *  verticalMarker_;
  FakePlotMarker * fakeSelectedMarker_;
  FakePlotMarker * fakeUnselectedMarker_;
};
}
#endif
