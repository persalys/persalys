//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of kriging
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#ifndef PERSALYS_KRIGINGRESULTWINDOW_HXX
#define PERSALYS_KRIGINGRESULTWINDOW_HXX

#include "persalys/ResultWindow.hxx"
#include "persalys/KrigingAnalysisResult.hxx"

namespace PERSALYS
{
class PERSALYS_VIEW_API KrigingResultWindow : public ResultWindow
{
  Q_OBJECT

public:
  KrigingResultWindow(AnalysisItem* item, QWidget * parent = 0);

protected:
  void buildInterface();

private:
  KrigingAnalysisResult result_;
  bool optimizeParameters_;
  QString errorMessage_;
};
}
#endif
