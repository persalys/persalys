//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a symbolic physical model
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
#include "otgui/SymbolicPhysicalModelWindow.hxx"

#include "otgui/PhysicalModelWindowWidget.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI
{

SymbolicPhysicalModelWindow::SymbolicPhysicalModelWindow(PhysicalModelDefinitionItem * item, QWidget * parent)
  : OTguiSubWindow(item, parent)
{
  setWindowTitle(tr("Symbolic physical model"));

  // Widgets
  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  PhysicalModelWindowWidget * widget = new PhysicalModelWindowWidget(item);
  connect(widget, SIGNAL(errorMessageChanged(QString)), this, SLOT(setTemporaryErrorMessage(QString)));
  mainLayout->addWidget(widget);

  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  mainLayout->addWidget(errorMessageLabel_);

  ////////////////
  setWidget(mainWidget);
}
}
