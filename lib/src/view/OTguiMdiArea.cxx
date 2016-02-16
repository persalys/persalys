//                                               -*- C++ -*-
/**
 *  @brief QMdiArea
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
#include "otgui/OTguiMdiArea.hxx"

#include "otgui/OTguiSubWindow.hxx"

#include <QAction>

namespace OTGUI {

OTguiMdiArea::OTguiMdiArea()
  : QMdiArea()
{
}


void OTguiMdiArea::showSubWindow(QMdiSubWindow * win)
{
  addSubWindow(win);
  win->showMaximized();
}


void OTguiMdiArea::showSubWindow(QStandardItem * item)
{
  for (int i=0; i<subWindowList().size(); ++i)
  {
    OTguiSubWindow * win = static_cast<OTguiSubWindow*>(subWindowList().at(i));
    if (win->getItem() == item)
    {
      win->widget()->showMaximized();
      setActiveSubWindow(win);
//       statusBar_->showErrorMessage(win->getErrorMessage());
    }
  }
}


void OTguiMdiArea::checkIfWindowResultExists(QStandardItem * item)
{
  for (int i=0; i<subWindowList().size(); ++i)
  {
    OTguiSubWindow * win = static_cast<OTguiSubWindow*>(subWindowList().at(i));
    if (win->getItem() == item)
    {
      removeSubWindow(win);
      delete win;
    }
  }
}
}