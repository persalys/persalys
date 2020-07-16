//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the input sample of the designs of experiments
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
#ifndef PERSALYS_DESIGNOFEXPERIMENTINPUTWINDOW_HXX
#define PERSALYS_DESIGNOFEXPERIMENTINPUTWINDOW_HXX

#include "persalys/DataAnalysisWindow.hxx"
#include "persalys/AnalysisItem.hxx"

namespace PERSALYS
{
class PERSALYS_VIEW_API DesignOfExperimentInputWindow : public DataAnalysisWindow
{
  Q_OBJECT

public:
  DesignOfExperimentInputWindow(AnalysisItem * item, QWidget * parent = 0);

public slots:
  virtual void updateVariablesListVisibility(int indexTab);
};
}
#endif
