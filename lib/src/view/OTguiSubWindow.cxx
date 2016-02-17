//                                               -*- C++ -*-
/**
 *  @brief Base class for all the windows of otgui
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
#include "otgui/OTguiSubWindow.hxx"

#include <QEventLoop>
#include <QTimer>

namespace OTGUI {

OTguiSubWindow::OTguiSubWindow(QStandardItem * item)
  : QMdiSubWindow()
  , errorMessageLabel_(0)
  , item_(item)
{
  setWindowIcon(QIcon(":/images/OT_icon16x16.png"));
}


QStandardItem * OTguiSubWindow::getItem()const
{
  return item_;
}


QString OTguiSubWindow::getErrorMessage() const
{
  return errorMessage_;
}


void OTguiSubWindow::setErrorMessage(QString message)
{
  message = QString("%1%2%3").arg("<font color=red>").arg(message).arg("</font>");
  errorMessage_ = message;
  if (errorMessageLabel_)
    errorMessageLabel_->setText(message);
  emit errorMessageChanged(message);
}


void OTguiSubWindow::setTemporaryErrorMessage(QString message)
{
  setErrorMessage(message);
  QEventLoop eventLoop;
  QTimer::singleShot(5000, &eventLoop, SLOT(quit()));
  eventLoop.exec();
  setErrorMessage("");
}
}