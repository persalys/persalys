//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of SRCAnalysis
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
#ifndef OTGUI_SRCRESULTWINDOW_HXX
#define OTGUI_SRCRESULTWINDOW_HXX

#include "otgui/ResultWindow.hxx"
#include "otgui/SRCResult.hxx"

#include <QListWidget>

namespace OTGUI {
class OTGUI_API SRCResultWindow : public ResultWindow
{
  Q_OBJECT

public:
  SRCResultWindow(AnalysisItem * item);

protected:
  void buildInterface();

public slots:
  void showHideGraphConfigurationWidget(int indexTab);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);
signals:
  void stateChanged(int);

private:
  SRCResult result_;
  QListWidget * outputsListWidget_;
  QTabWidget * tabWidget_;
  QString warningMessage_;
};
}
#endif
