//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define the diagram of the data model
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
#ifndef OTGUI_DATAMODELDIAGRAMWINDOW_HXX
#define OTGUI_DATAMODELDIAGRAMWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/DataModelDiagramItem.hxx"

namespace OTGUI
{
class OTGUI_API DataModelDiagramWindow : public OTguiSubWindow
{
  Q_OBJECT

public :
  DataModelDiagramWindow(DataModelDiagramItem * item, QWidget * parent = 0);
};
}
#endif
