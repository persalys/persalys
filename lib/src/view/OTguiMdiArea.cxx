//                                               -*- C++ -*-
/**
 *  @brief QMdiArea
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
#include "otgui/OTguiMdiArea.hxx"

#include "otgui/OTguiSubWindow.hxx"

#include <QDebug>

namespace OTGUI
{

OTguiMdiArea::OTguiMdiArea()
  : QMdiArea()
{
}


void OTguiMdiArea::showSubWindow(OTguiSubWindow * win)
{
  if (!win)
    return;
  if (!subWindowList().count())
    emit mdiAreaEmpty(false);
  addSubWindow(win);
  win->showMaximized();
  connect(win, SIGNAL(errorMessageChanged(QString)), this, SIGNAL(errorMessageChanged(QString)));
  connect(win, SIGNAL(removeWindowRequested()), this, SLOT(removeSubWindow()));
}


void OTguiMdiArea::showSubWindow(QStandardItem * item)
{
  if (!item)
    return;
  for (int i = 0; i < subWindowList().size(); ++i)
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
      emit errorMessageChanged(win->getErrorMessage());
    }
  }
}


void OTguiMdiArea::removeSubWindow(OTguiSubWindow* win)
{
  emit errorMessageChanged("");
  QMdiArea::removeSubWindow(win);
  win->deleteLater();

  if (!subWindowList().count())
    emit mdiAreaEmpty(true);
}


void OTguiMdiArea::removeSubWindow(QStandardItem* item)
{
  for (int i = 0; i < subWindowList().size(); ++i)
  {
    OTguiSubWindow * win = static_cast<OTguiSubWindow*>(subWindowList().at(i));
    if (win->getItem() == item)
    {
      removeSubWindow(win);
      return;
    }
  }
}


void OTguiMdiArea::removeSubWindow()
{
  OTguiSubWindow * win = qobject_cast<OTguiSubWindow*>(sender());

  if (!win)
  {
    qDebug() << "OTguiMdiArea::removeSubWindow : win NULL\n";
    return;
  }
  removeSubWindow(win);
}
}