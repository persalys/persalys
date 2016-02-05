// OTguiSubWindow.cxx

#include "otgui/OTguiSubWindow.hxx"

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
}