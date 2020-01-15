//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of ApproximationAnalysis
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_APPROXIMATIONRESULTWINDOW_HXX
#define PERSALYS_APPROXIMATIONRESULTWINDOW_HXX

#include "persalys/ResultWindow.hxx"

namespace PERSALYS
{
class PERSALYS_API ApproximationResultWindow : public ResultWindow
{
  Q_OBJECT

public:
  ApproximationResultWindow(AnalysisItem * item, QWidget * parent = 0);
};
}
#endif
