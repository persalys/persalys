//                                               -*- C++ -*-
/**
 *  @brief Base class QMdiSubWindow for the results
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
#ifndef OTGUI_RESULTWINDOW_HXX
#define OTGUI_RESULTWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/AnalysisItem.hxx"
#include "otgui/ParametersWidget.hxx"

namespace OTGUI {
class OTGUI_API ResultWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  ResultWindow(AnalysisItem * item);

protected:
  virtual void setParameters(const Analysis & analysis);

public slots:
  virtual void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);

protected:
  QWidget * parametersWidget_;
};
}
#endif