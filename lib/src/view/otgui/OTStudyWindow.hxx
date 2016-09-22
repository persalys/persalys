//                                               -*- C++ -*-
/**
 *  @brief OTStudy window
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#ifndef OTGUI_OTSTUDYWINDOW_HXX
#define OTGUI_OTSTUDYWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/OTStudyItem.hxx"

namespace OTGUI {
class OTStudyWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  OTStudyWindow(OTStudyItem * item);

  void buildInterface();

public slots:
signals:
  void createNewDataModel();
  void createNewAnalyticalPhysicalModel();
  void createNewPythonPhysicalModel();
#ifdef OTGUI_HAVE_YACS
  void createNewYACSPhysicalModel();
#endif

private:
};
}
#endif