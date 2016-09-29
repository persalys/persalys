//                                               -*- C++ -*-
/**
 *  @brief QWidget for plot matrices
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_PLOTMATRIXWIDGET_HXX
#define OTGUI_PLOTMATRIXWIDGET_HXX

#include <openturns/NumericalSample.hxx>

#include <QTableWidget>
#include <QLabel>
#include "otgui/OTGuiprivate.hxx"

namespace OTGUI {
class OTGUI_API PlotMatrixWidget : public QWidget
{
  Q_OBJECT

public:
  PlotMatrixWidget(const OT::NumericalSample & inputSample, const OT::NumericalSample & outputSample, QWidget * parent = 0);

  QString getTitle() const;
  void setTitle(const QString & title);
  QStringList getInputNames() const;
  QStringList getOutputNames() const;
  QImage getMatrixImage();

public slots:
  void exportPlot();
  void setInputsToDisplay(QStringList inputs);
  void setOutputsToDisplay(QStringList outputs);

private:
  QTableWidget * tableWidget_;
  QList<QImage> matrixImages_;
  QImage matrixImage_;
  QStringList inputNames_;
  QStringList outputNames_;
  QLabel * matrixTitleLabel_;
  int nbInputsToDisplay_;
  int nbOutputsToDisplay_;
};
}
#endif