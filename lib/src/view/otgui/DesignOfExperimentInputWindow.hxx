//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the input sample of the designs of experiments
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
#ifndef OTGUI_DESIGNOFEXPERIMENTINPUTWINDOW_HXX
#define OTGUI_DESIGNOFEXPERIMENTINPUTWINDOW_HXX

#include "OTguiSubWindow.hxx"
#include "otgui/DesignOfExperimentDefinitionItem.hxx"

namespace OTGUI {
class OTGUI_API DesignOfExperimentInputWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  DesignOfExperimentInputWindow(DesignOfExperimentDefinitionItem * item, QWidget * parent=0);

protected:
  void buildInterface();

private:
  OT::Sample originalInputSample_;
};
}
#endif
