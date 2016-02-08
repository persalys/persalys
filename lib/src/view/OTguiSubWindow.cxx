// OTguiSubWindow.cxx

#include "otgui/OTguiSubWindow.hxx"

#include <QEventLoop>
#include <QTimer>

namespace OTGUI {

OTguiSubWindow::OTguiSubWindow(QStandardItem * item)
 : QMdiSubWindow()
 , item_(item)
 , errorMessageLabel_(0)
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
  errorMessage_ = message;
  if (errorMessageLabel_)
    errorMessageLabel_->setText(message);
  emit errorMessageChanged(message);
}


void OTguiSubWindow::setTemporaryErrorMessage(QString message)
{
  setErrorMessage(QString("%1%2%3").arg("<font color=red>").arg(message).arg("</font>"));
  QEventLoop eventLoop;
  QTimer::singleShot(3000, &eventLoop, SLOT(quit()));
  eventLoop.exec();
  setErrorMessage("");
}
}