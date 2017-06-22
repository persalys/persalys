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
#include "otgui/ResultWindow.hxx"

#include <QGridLayout>

using namespace OT;

namespace OTGUI {
  
ResultWindow::ResultWindow(AnalysisItem * item)
  : OTguiSubWindow(item)
  , parametersWidget_(0)
{
  connect(this, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)), this, SLOT(showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates)));
}


void ResultWindow::setParameters(const Analysis& analysis, const QString& title)
{
  QStringList paramNames;
  QStringList paramValues;
  AnalysisItem::GetAnalysisParameters(analysis, paramNames, paramValues);

  if (!paramNames.isEmpty() && paramNames.size() == paramValues.size())
  {
    parametersWidget_ = new QWidget;
    QGridLayout * parametersWidgetLayout = new QGridLayout(parametersWidget_);
    parametersWidgetLayout->addWidget(new ParametersWidget(title, paramNames, paramValues));
  }
}


void ResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates)
{
  // to overwrite
}
}