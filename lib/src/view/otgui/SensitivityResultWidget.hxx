//                                               -*- C++ -*-
/**
 *  @brief QWidget for sensitivity analysis result
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#ifndef OTGUI_SENSITIVITYRESULTWIDGET_HXX
#define OTGUI_SENSITIVITYRESULTWIDGET_HXX

#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/PlotWidget.hxx"

#include <openturns/Point.hxx>

#include <QWidget>

namespace OTGUI
{
class OTGUI_API SensitivityResultWidget : public QWidget
{
  Q_OBJECT

public:
  enum Type {Sobol, SRC};

  SensitivityResultWidget(const OT::Point& firstIndices,
                          const OT::Point& totalIndices,
                          const OT::Description& inputNames,
                          const OT::String& outputName,
                          const Type type,
                          QWidget * parent = 0);

public slots:
  void updateIndicesPlot(int, Qt::SortOrder);

private:
  PlotWidget * plot_;
  CustomStandardItemModel * indicesTableModel_;
};
}
#endif
