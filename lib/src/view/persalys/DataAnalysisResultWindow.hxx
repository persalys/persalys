//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of data analysis
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_DATAANALYSISRESULTWINDOW_HXX
#define PERSALYS_DATAANALYSISRESULTWINDOW_HXX

#include "persalys/DataAnalysisWindow.hxx"

namespace PERSALYS
{
class PERSALYS_VIEW_API DataAnalysisResultWindow : public DataAnalysisWindow
{
  Q_OBJECT

public:
  DataAnalysisResultWindow(AnalysisItem * item, QWidget * parent = 0);

protected:
  void initialize(AnalysisItem* item);
};
}
#endif
