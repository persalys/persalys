// mainWindow.hxx

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "otgui/StudyTreeView.hxx"
#include "otgui/qpyconsole.h"

#include <QMainWindow>
#include <QMdiArea>
#include <QMenu>

namespace OTGUI {
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

  QMdiArea * getMdiArea() const;
  void launchInitialMessageBox();

public slots:
  void showSubWindow(QMdiSubWindow * win);
  void showSubWindow(QStandardItem * item);
  void checkIfWindowResultExists(AnalysisItem * item);

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