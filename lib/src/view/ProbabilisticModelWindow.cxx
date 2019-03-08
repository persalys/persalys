//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a probabilistic model
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/ProbabilisticModelWindow.hxx"

#include "otgui/DependenciesWidget.hxx"

using namespace OT;

namespace OTGUI
{

ProbabilisticModelWindow::ProbabilisticModelWindow(ProbabilisticModelItem * item, QWidget * parent)
  : SubWindow(item, parent)
  , marginalsWidget_(new MarginalsWidget(item, this))
  , dependenciesWidget_(new DependenciesWidget(item, this))
{
  buildInterface();
}


void ProbabilisticModelWindow::buildInterface()
{
  QVBoxLayout * mainWidgetLayout = new QVBoxLayout(this);

  mainWidgetLayout->addWidget(new TitleLabel(tr("Probabilistic model"), "user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#probamodel"));

  QTabWidget * rootTab = new QTabWidget;

  // First Tab: marginals
  rootTab->addTab(marginalsWidget_, tr("Marginals"));

  // Second Tab: dependencies
  rootTab->addTab(dependenciesWidget_, tr("Dependence"));
  connect(marginalsWidget_, SIGNAL(updateDependenciesRequested()), dependenciesWidget_, SLOT(updateWidgets()));

  mainWidgetLayout->addWidget(rootTab, 1);
}
}
