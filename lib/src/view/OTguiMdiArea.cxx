//                                               -*- C++ -*-
/**
 *  @brief QMdiArea
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
  if (!subWindowList().count())
    emit mdiAreaEmpty(false);
  addSubWindow(win);
  win->showMaximized();
}


void OTguiMdiArea::showSubWindow(QStandardItem * item)
{
  for (int i=0; i<subWindowList().size(); ++i)
  {
    OTguiSubWindow * win = static_cast<OTguiSubWindow*>(subWindowList().at(i));
    if (win->getItem()->data(Qt::UserRole).toString() == "OTStudy")
    {
      win->close();
    }
    if (win->getItem() == item)
    {
      win->widget()->showMaximized();
      setActiveSubWindow(win);
      win->setErrorMessage(win->getErrorMessage());
    }
  }
}


void OTguiMdiArea::removeSubWindow(QStandardItem * item)
{
  for (int i=0; i<subWindowList().size(); ++i)
  {
    OTguiSubWindow * win = static_cast<OTguiSubWindow*>(subWindowList().at(i));
    if (win->getItem() == item)
    {
      QMdiArea::removeSubWindow(win);
      win->deleteLater();
      break;
    }
  }
  if (!subWindowList().count())
    emit mdiAreaEmpty(true);
}
}