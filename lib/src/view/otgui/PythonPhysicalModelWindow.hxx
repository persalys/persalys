//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a python physical model
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
#ifndef OTGUI_PYTHONPHYSICALMODELWINDOW_HXX
#define OTGUI_PYTHONPHYSICALMODELWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/PhysicalModelItem.hxx"
#include "otgui/CodeModel.hxx"

#include <QTableView>

namespace OTGUI {

class OTGUI_API CodeView : public QTableView
{
  Q_OBJECT
public:
  CodeView(QWidget * parent = 0);
protected:
  virtual bool event(QEvent * event);
};

class OTGUI_API PythonPhysicalModelWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
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