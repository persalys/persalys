//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a python physical model
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
#ifndef OTGUI_PYTHONPHYSICALMODELWINDOW_HXX
#define OTGUI_PYTHONPHYSICALMODELWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/PhysicalModelDefinitionItem.hxx"
#include "otgui/PhysicalModelWindowWidget.hxx"
#include "otgui/CodeModel.hxx"

#include <QTableView>

namespace OTGUI
{
class OTGUI_API PythonPhysicalModelWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  PythonPhysicalModelWindow(PhysicalModelDefinitionItem * item, QWidget * parent = 0);

public slots:
  void parallelizationRequested(int);

private:
  PhysicalModel physicalModel_;
};
}
#endif
