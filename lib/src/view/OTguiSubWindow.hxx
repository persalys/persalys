// OTguiSubWindow.hxx

#ifndef OTGUISUBWINDOW_H
#define OTGUISUBWINDOW_H

#include <QStandardItem>
#include <QMdiSubWindow>

namespace OTGUI {
class OTguiSubWindow : public QMdiSubWindow
{
  Q_OBJECT

public:
  OTguiSubWindow(QStandardItem * item);

  QStandardItem * getItem()const;

private:
  QStandardItem * item_;
};
}
#endif