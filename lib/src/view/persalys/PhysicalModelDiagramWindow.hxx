//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define the diagram of the physical model
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifndef PERSALYS_PHYSICALMODELDIAGRAMWINDOW_HXX
#define PERSALYS_PHYSICALMODELDIAGRAMWINDOW_HXX

#include "persalys/SubWindow.hxx"
#include "persalys/PhysicalModelDiagramItem.hxx"

namespace PERSALYS
{
class PERSALYS_API PhysicalModelDiagramWindow : public SubWindow
{
  Q_OBJECT

public :
  PhysicalModelDiagramWindow(PhysicalModelDiagramItem * item, QWidget * parent = 0);
};
}
#endif
