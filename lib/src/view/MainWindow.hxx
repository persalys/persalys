// mainWindow.hxx


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "StudyTreeView.hxx"
#include "qpyconsole.h"

#include <QMainWindow>
#include <QMdiArea>
#include <QMenu>

namespace OTGUI {
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

  virtual ~MainWindow();

  QMdiArea * getMdiArea() const;

public slots:
  void showSubWindow(QMdiSubWindow * win);
  void showSubWindow(QStandardItem * item);

protected:
  void buildInterface();
  void buildActions();

private:
  StudyTreeView * studyTree_;
  QMdiArea * mdiArea_;
  QMenu * menu_;
  QPyConsole * console_;
};
}
#endif