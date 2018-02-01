//                                               -*- C++ -*-
/**
 *  @brief Base class QMdiSubWindow for the results
 *
 *  Copyright 2015-2018 EDF-Phimeca
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

#include "otgui/TranslationManager.hxx"

#include <QGridLayout>

using namespace OT;

namespace OTGUI
{

ResultWindow::ResultWindow(OTguiItem * item, QWidget * parent)
  : OTguiSubWindow(item, parent)
  , parametersWidget_(0)
{
}


void ResultWindow::setParameters(const Analysis& analysis, const QString& title)
{
  const Parameters analysisParameters(analysis.getImplementation()->getParameters());
  if (analysisParameters.getSize())
  {
    parametersWidget_ = new QWidget;
    QGridLayout * parametersWidgetLayout = new QGridLayout(parametersWidget_);
    parametersWidgetLayout->addWidget(new ParametersWidget(title, analysisParameters));
  }
}
}
