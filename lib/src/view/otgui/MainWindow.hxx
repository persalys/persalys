// mainWindow.hxx

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "otgui/StudyTreeView.hxx"
#include "otgui/qpyconsole.h"
#include "otgui/OTguiMenuBar.hxx"
#include "otgui/OTguiToolBar.hxx"
#include "otgui/OTguiStatusBar.hxx"

#include <QMainWindow>
#include <QMdiArea>
#include <QDockWidget>

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
  void checkIfWindowResultExists(ObserverItem * item);
  void showGraphConfigurationTabWidget(QWidget*);
  void hideGraphConfigurationTabWidget(QWidget*);

protected:
  void buildInterface();
  void buildConnections();

private:
  StudyTreeView * studyTree_;
  QDockWidget * configurationDock_;
  QMdiArea * mdiArea_;
  OTguiMenuBar * menuBar_;
  OTguiToolBar * toolBar_;
  OTguiStatusBar * statusBar_;
  QPyConsole * console_;
};
}
#endif