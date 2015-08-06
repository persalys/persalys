// OTguiSubWindow.cxx


#include "OTguiSubWindow.hxx"

namespace OTGUI {

OTguiSubWindow::OTguiSubWindow(QStandardItem * item)
 : QMdiSubWindow()
 , item_(item)
{
}


OTguiSubWindow::~OTguiSubWindow()
{
}


QStandardItem * OTguiSubWindow::getItem()const
{
  return item_;
}
}