//                                               -*- C++ -*-
/**
 *  @brief QScrollArea for the Sobol indices
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
#ifndef OTGUI_SOBOLRESULTWINDOWWIDGET_HXX
#define OTGUI_SOBOLRESULTWINDOWWIDGET_HXX

#include "otgui/SobolResult.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/CustomStandardItemModel.hxx"

#include <QScrollArea>

namespace OTGUI {
class OTGUI_API SobolResultWindowWidget : public QScrollArea
{
  Q_OBJECT

public:
  SobolResultWindowWidget(const SobolResult& result, const QString& warningMessage="");

  QVector<PlotWidget*> getPlotWidgets();

protected:
  void buildInterface();

public slots:
  void updateIndicesPlot(int section, Qt::SortOrder order);
  void setCurrentIndex(int);
signals:
  void currentPlotChanged(int);

private:
  SobolResult result_;
  int currentIndex_;
  QVector<CustomStandardItemModel*> listTableModels_;
  QVector<PlotWidget*> listPlotWidgets_;
  QString warningMessage_;
};
}
#endif