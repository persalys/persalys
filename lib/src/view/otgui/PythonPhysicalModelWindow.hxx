//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a python physical model
 *
 *  Copyright 2015-2016 EDF
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
#ifndef OTGUI_PYTHONPHYSICALMODELWINDOW_HXX
#define OTGUI_PYTHONPHYSICALMODELWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/PhysicalModelItem.hxx"
#include "otgui/CodeModel.hxx"

#include <QTableView>
#include <QGroupBox>

namespace OTGUI {
  
class PythonPhysicalModelWindow : public OTguiSubWindow
{
  Q_OBJECT

public :
  PythonPhysicalModelWindow(PhysicalModelItem * item);

public slots:
  void updateCodeModel();

private:
  PhysicalModel physicalModel_;
  CodeModel * codeModel_;
  QTableView * codeView_;
};
}
#endif