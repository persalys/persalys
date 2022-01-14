//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for metamodels
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
#include "persalys/MetaModelWindow.hxx"

#include "persalys/PhysicalModelWindowWidget.hxx"

#include <QVBoxLayout>

namespace PERSALYS
{

MetaModelWindow::MetaModelWindow(PhysicalModelItem * item, QWidget * parent)
  : SubWindow(item, parent)
  , physicalModel_(item->getPhysicalModel())
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(new TitleLabel(tr("Metamodel")));

  widgetLayout->addWidget(new PhysicalModelWindowWidget(item));
}
}
