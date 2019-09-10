//                                               -*- C++ -*-
/**
 *  @brief QwtPlot for box plot
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
#ifndef PERSALYS_BOXPLOT_HXX
#define PERSALYS_BOXPLOT_HXX

#include "persalys/PlotWidget.hxx"
#include "persalys/DataAnalysisResult.hxx"

namespace PERSALYS
{

class PERSALYS_API BoxPlot : public PlotWidget
{
  Q_OBJECT

public:
  BoxPlot(const QStringList &variableNames, QWidget *parent = 0);

  void addBoxPlot(const DataAnalysisResult &result, const OT::UnsignedInteger index);

protected:
  void plotCurve(const int index, double * x, double * y, int size, const QPen pen = QPen(Qt::black, 2),
                 QwtPlotCurve::CurveStyle style = QwtPlotCurve::Lines, QwtSymbol* symbol = 0);
  void updateVariableOrder(const QStringList &variableNames, const QList<int> &indices);

public slots:
  void setVariablesToShow(const QStringList&);

private:
  QStringList variableNames_;
  int boxesCounter_;
};
}
#endif
