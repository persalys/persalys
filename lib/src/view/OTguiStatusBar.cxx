// OTguiStatusBar.cxx

#include "otgui/OTguiStatusBar.hxx"

namespace OTGUI {

OTguiStatusBar::OTguiStatusBar(QWidget *parent)
  : QStatusBar(parent)
{
  messageLabel_ = new QLabel;
  addWidget(messageLabel_);
}


void OTguiStatusBar::showErrorMessage(const QString & message)
{
  messageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(message).arg("</font>"));
}


void OTguiStatusBar::showMessage(const QString & message)
{
  messageLabel_->setText(message);
}


void OTguiStatusBar::clearMessage()
{
  messageLabel_->setText("");
}
}