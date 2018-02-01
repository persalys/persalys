//                                               -*- C++ -*-
/**
 *  @brief QWidget for plot matrices
 *
 *  Copyright 2015-2018 EDF-Phimeca
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

#include <openturns/Sample.hxx>

#include <QTableWidget>
#include <QLabel>
#include "otgui/OTGuiprivate.hxx"

namespace OTGUI
{
class OTGUI_API PlotMatrixWidget : public QWidget
{
  Q_OBJECT

public:
  PlotMatrixWidget(const OT::Sample & sample1, const OT::Sample & sample2, QWidget * parent = 0);

  QString getTitle() const;
  void setTitle(const QString & title);
  QStringList getInputNames() const;
  QStringList getOutputNames() const;
  void setInputNames(const QStringList inputNames);
  void setOutputNames(const QStringList outputNames);
  QStringList getColumnsNames() const;
  QStringList getRowsNames() const;
  QImage getMatrixImage();

public slots:
  void exportPlot();
  void setColumnsToDisplay(QStringList columns);
  void setRowsToDisplay(QStringList rows);

private:
  QTableWidget * tableWidget_;
  QList<QImage> matrixImages_;
  QImage matrixImage_;
  QStringList inputNames_;
  QStringList outputNames_;
  QStringList columnsNames_;
  QStringList rowsNames_;
  QLabel * matrixTitleLabel_;
  int nbColumnsToDisplay_;
  int nbRowsToDisplay_;
};
}
#endif
