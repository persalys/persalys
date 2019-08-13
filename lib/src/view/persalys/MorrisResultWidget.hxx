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
#ifndef PERSALYS_MORRISRESULTWIDGET_HXX
#define PERSALYS_MORRISRESULTWIDGET_HXX

#include "persalys/MorrisResult.hxx"

#include "persalys/MorrisPlot.hxx"

namespace PERSALYS
{

class PERSALYS_API MorrisResultWidget : public QWidget
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
  void updateSelectedMarkerRequested(const QString &text);
  void updateUnselectedMarkerRequested(const QString &text);

private:
  MorrisResult& result_;
  OT::UnsignedInteger outputIndex_;
  QVector<PlotMarker*> markersMuSigma_;
  QVector<PlotMarker*> markersMuMu_;
};
}
#endif
