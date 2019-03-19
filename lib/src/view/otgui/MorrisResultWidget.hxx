//                                               -*- C++ -*-
/**
 *  @brief QWidget for Morris analysis result
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifndef OTGUI_MORRISRESULTWIDGET_HXX
#define OTGUI_MORRISRESULTWIDGET_HXX

#include "otgui/MorrisResult.hxx"

#include <QWidget>

#include <qwt_plot_marker.h>

namespace OTGUI
{

class OTGUI_API MorrisResultWidget : public QWidget
{
  Q_OBJECT

public:
  MorrisResultWidget(MorrisResult &result, const int outIndex, QWidget *parent = 0);

protected:
  void buildInterface();
  void updateLabels();

public slots:
  void updateNoEffectBoundary(const double);
  void updateSelectedPointsFromTable();
  void updateSelectedPointsFromMuSigma();
  void updateSelectedPointsFromMuMu();
signals:
  void plotItemsChanged();
  void resetTableModel();
  void noEffectBoundaryChanged(const QPointF&);

private:
  MorrisResult& result_;
  OT::UnsignedInteger outpuIndex_;
  QVector<QwtPlotMarker*> markersMuSigma_;
  QVector<QwtPlotMarker*> markersMuMu_;
  QVector<QwtPlotMarker*> fakeSelectedMarkers_;
  QVector<QwtPlotMarker*> fakeUnselectedMarkers_;
};
}
#endif
