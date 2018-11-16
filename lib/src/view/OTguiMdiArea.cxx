//                                               -*- C++ -*-
/**
 *  @brief QMdiArea
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
#include "otgui/OTguiMdiArea.hxx"

#include <QDebug>

namespace OTGUI
{

OTguiMdiArea::OTguiMdiArea(QWidget *parent)
  : QMdiArea(parent)
  , welcomeWindow_(0)
{
}


void OTguiMdiArea::addWelcomeWindow(WelcomeWindow * win)
{
  welcomeWindow_ = win;
  QMdiArea::addSubWindow(win);
  win->showMaximized();
}


void OTguiMdiArea::addSubWindow(OTguiSubWindow * win)
{
  if (!win)
    return;

  // close the welcome window
  if (welcomeWindow_)
    welcomeWindow_->close();

  // add subwindow and show it
  QMdiArea::addSubWindow(win);
  win->widget()->showMaximized();

  // connections
  connect(win, SIGNAL(showWindowRequested()), this, SLOT(changeActiveSubWindow()));
  connect(win, SIGNAL(removeWindowRequested()), this, SLOT(removeSubWindow()));
}


void OTguiMdiArea::changeActiveSubWindow()
{
  OTguiSubWindow * win = qobject_cast<OTguiSubWindow*>(sender());

  if (!win)
  {
    qDebug() << "OTguiMdiArea::changeActiveSubWindow : win NULL\n";
    return;
  }
  win->widget()->showMaximized();
  setActiveSubWindow(win);
}


void OTguiMdiArea::removeSubWindow(OTguiSubWindow* win)
{
  QMdiArea::removeSubWindow(win);
  win->deleteLater();

  // if no more subwindows: open the welcome window
  if (subWindowList().count() < 2 && welcomeWindow_)
  {
    welcomeWindow_->widget()->showMaximized();
    setActiveSubWindow(welcomeWindow_);
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
