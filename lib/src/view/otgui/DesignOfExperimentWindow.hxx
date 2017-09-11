//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the designs of experiments
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
#ifndef OTGUI_DESIGNOFEXPERIMENTWINDOW_HXX
#define OTGUI_DESIGNOFEXPERIMENTWINDOW_HXX

#include "DataAnalysisWindow.hxx"

namespace OTGUI {
class OTGUI_API DesignOfExperimentWindow : public DataAnalysisWindow
{
  Q_OBJECT

public:
  DesignOfExperimentWindow(AnalysisItem * item, QWidget * parent=0);

protected:
  virtual void initialize(AnalysisItem* item);
  virtual void fillListWidget();
  virtual void fillTabWidget();
  virtual void addSummaryTab();
#ifdef OTGUI_HAVE_PARAVIEW
  virtual void addParaviewWidgetsTabs();
#endif
  virtual void addTableTab();

public slots:
  virtual void updateVariablesListVisibility(int indexTab);
private:
  OT::Sample notEvaluatedInputSample_;
};
}
#endif
