//                                               -*- C++ -*-
/**
 *  @brief Base class for all the windows of otgui
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
#include "otgui/OTguiSubWindow.hxx"

#include <QEventLoop>
#include <QTimer>
#include <QTimeLine>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QOpenGLFunctions_3_2_Core>

namespace OTGUI
{

OTguiSubWindow::OTguiSubWindow(OTguiItem * item, QWidget * parent)
  : QMdiSubWindow(parent)
  , errorMessageLabel_(0)
  , item_(item)
{
  setWindowIcon(QIcon(":/images/OT_icon16x16.png"));

  // connections: signal for mdiArea
  connect(item, SIGNAL(removeWindowRequested()), this, SIGNAL(removeWindowRequested()));
  connect(item, SIGNAL(showWindowRequested()), this, SIGNAL(showWindowRequested()));
}


OTguiSubWindow::~OTguiSubWindow()
{
  emit graphWindowDeactivated();
}


bool OTguiSubWindow::HaveOpenGL32()
{
  char *pNO_GL = getenv("OTGUI_NO_GL");
  if (pNO_GL)
    return false;

  QSurfaceFormat requestedFormat;
  requestedFormat.setVersion(3, 2);
  QOpenGLContext context;
  context.setFormat(requestedFormat);
  if (!context.create())
    return false;

  return context.versionFunctions<QOpenGLFunctions_3_2_Core>();
}


OTguiItem * OTguiSubWindow::getItem() const
{
  return item_;
}


QString OTguiSubWindow::getErrorMessage() const
{
  return errorMessage_;
}


void OTguiSubWindow::setErrorMessage(QString message)
{
  message = QString("<font color=red>%1</font>").arg(message);
  errorMessage_ = message;
  if (errorMessageLabel_)
    errorMessageLabel_->setText(message);
  if (windowState() == Qt::WindowFullScreen || windowState() == Qt::WindowActive || windowState() == (Qt::WindowActive | Qt::WindowMaximized))
    emit errorMessageChanged(message);
}


void OTguiSubWindow::setTemporaryErrorMessage(QString message)
{
  message = QString("<font color=red>%1</font>").arg(message);
  errorMessage_ = message;
  if (errorMessageLabel_)
    errorMessageLabel_->setText(message);
  QTimeLine * time = new QTimeLine(7000, this);
  qtimelineList_.push_back(time);
  connect(time, SIGNAL(stateChanged(QTimeLine::State)), this, SLOT(reInitErrorMessage(QTimeLine::State)));
  time->start();
}


void OTguiSubWindow::reInitErrorMessage(QTimeLine::State state)
{
  if (state == QTimeLine::NotRunning)
  {
    if (qtimelineList_.isEmpty())
      return;
    qtimelineList_.removeFirst();
    // if another QTimeLine started before the end of the previous one: do nothing
    if (qtimelineList_.size())
      return;
    // else initialize error message
    errorMessage_ = "";
    if (errorMessageLabel_)
      errorMessageLabel_->setText("");
  }
}
}
