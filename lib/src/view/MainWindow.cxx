#include "otgui/MainWindow.hxx"

#include "otgui/PhysicalModelWindow.hxx"
#include "otgui/GraphConfigurationWidget.hxx"

#include <QSplitter>
#include <QMenuBar>
#include <QDockWidget>
#include <QMessageBox>

namespace OTGUI {

MainWindow::MainWindow()
  : QMainWindow()
  , studyTree_(new StudyTreeView(this))
  , mdiArea_(new QMdiArea(this))
{
  buildInterface();
  buildConnections();
  setWindowTitle("OTGui");
  setWindowIcon(QIcon(":/images/OT_icon32x32.png"));
}


void MainWindow::buildInterface()
{
  // main widget
  QSplitter * mainSplitter = new QSplitter(Qt::Horizontal);

  // left side of the mainSplitter
  QSplitter * leftSideSplitter = new QSplitter(Qt::Vertical);
  leftSideSplitter->addWidget(studyTree_);
  leftSideSplitter->setStretchFactor(0, 8);

  configurationDock_ = new QDockWidget(tr("Graphic configuration"));
  configurationDock_->setFeatures(QDockWidget::NoDockWidgetFeatures);

  leftSideSplitter->addWidget(configurationDock_);
  configurationDock_->close();
  mainSplitter->addWidget(leftSideSplitter);
  leftSideSplitter->setStretchFactor(1, 1);

  // right side of the mainSplitter
  QSplitter * rightSideSplitter = new QSplitter(Qt::Vertical);
  rightSideSplitter->addWidget(mdiArea_);

  // Python Console
  console_ = new QPyConsole(this);
  QDockWidget * consoleDock = new QDockWidget(tr("Python Console"));
  consoleDock->setWidget(console_);
  consoleDock->setFeatures(QDockWidget::DockWidgetClosable);
  rightSideSplitter->addWidget(consoleDock);
  rightSideSplitter->setStretchFactor(0, 3);
  //TODO:
//   connect(consoleDock, SIGNAL(visibilityChanged(bool)), this,SLOT(consoleVisibilityChanged()));
  rightSideSplitter->setStretchFactor(1, 1);
  mainSplitter->addWidget(rightSideSplitter);

  setCentralWidget(mainSplitter);

  // menu bar
  menuBar_ = new OTguiMenuBar;
  setMenuBar(menuBar_);

  // tool bar
  toolBar_ = new OTguiToolBar;
  addToolBar(toolBar_);

  // status status bar
  statusBar_ = new OTguiStatusBar;
  setStatusBar(statusBar_);
}


void MainWindow::buildConnections()
{
  // signals from the study tree to MainWindow
  connect(studyTree_, SIGNAL(showWindow(QMdiSubWindow *)), this, SLOT(showSubWindow(QMdiSubWindow *)));
  connect(studyTree_, SIGNAL(itemSelected(QStandardItem*)), this, SLOT(showSubWindow(QStandardItem *)));
  connect(studyTree_, SIGNAL(checkIfWindowResultExists(ObserverItem *)), this, SLOT(checkIfWindowResultExists(ObserverItem *)));
  connect(studyTree_, SIGNAL(graphWindowActivated(QWidget*)), this, SLOT(showGraphConfigurationTabWidget(QWidget*)));
  connect(studyTree_, SIGNAL(graphWindowDeactivated(QWidget*)), this, SLOT(hideGraphConfigurationTabWidget(QWidget*)));
  connect(studyTree_, SIGNAL(errorMessageEmitted(QString)), statusBar_, SLOT(showErrorMessage(QString)));

  // signal from studyTree_ to console_
  connect(studyTree_, SIGNAL(loadPythonScript(const QString &)), console_, SLOT(loadScript(const QString &)));

  // signals from the menu bar to studyTree_ and MainWindow
  connect(menuBar_, SIGNAL(createNewOTStudy()), studyTree_, SLOT(createNewOTStudy()));
  connect(menuBar_, SIGNAL(loadOTStudy()), studyTree_, SLOT(loadOTStudy()));
  connect(menuBar_, SIGNAL(closeWindow()), this, SLOT(close()));

  // signals from the tool bar to studyTree_
  connect(toolBar_, SIGNAL(createNewOTStudy()), studyTree_, SLOT(createNewOTStudy()));
  connect(toolBar_, SIGNAL(loadOTStudy()), studyTree_, SLOT(loadOTStudy()));
}


void MainWindow::launchInitialMessageBox()
{
  QMessageBox messageBox(this);
  messageBox.setText("Please, select required action by pressing the corresponding button below.");
  QPushButton * newOTStudyButton = messageBox.addButton(tr("New"), QMessageBox::ActionRole);
// TODO:   QPushButton * openOTStudyButton = messageBox.addButton(tr("Open..."), QMessageBox::ActionRole);
  QPushButton * loadScriptButton = messageBox.addButton(tr("Load Script..."), QMessageBox::ActionRole);
  messageBox.setStandardButtons(QMessageBox::Cancel);
  messageBox.setDefaultButton(newOTStudyButton);
  messageBox.exec();

  if (messageBox.clickedButton() == newOTStudyButton)
    studyTree_->createNewOTStudy();
  else if (messageBox.clickedButton() == loadScriptButton)
    studyTree_->loadOTStudy();
}


QMdiArea * MainWindow::getMdiArea() const
{
  return mdiArea_;
}


void MainWindow::showSubWindow(QMdiSubWindow * win)
{
  mdiArea_->addSubWindow(win);
  win->showMaximized();
}


void MainWindow::showSubWindow(QStandardItem * item)
{
  QList<QMdiSubWindow *> listSubWindow =  mdiArea_->subWindowList();
  for (int i = 0; i < listSubWindow.size(); ++i)
  {
    OTguiSubWindow * win = static_cast<OTguiSubWindow*>(listSubWindow.at(i));
    if (win->getItem() == item)
    {
      win->widget()->showMaximized();
      mdiArea_->setActiveSubWindow(win);
      statusBar_->showErrorMessage(win->getErrorMessage());
    }
  }
}


void MainWindow::checkIfWindowResultExists(ObserverItem* item)
{
  QList<QMdiSubWindow *> listSubWindow =  mdiArea_->subWindowList();

  for (int i = 0; i < listSubWindow.size(); ++i)
  {
    OTguiSubWindow * win = static_cast<OTguiSubWindow*>(listSubWindow.at(i));
    if ( win->getItem() == item)
    {
      mdiArea_->removeSubWindow(win);
      delete win;
    }
  }
}


void MainWindow::showGraphConfigurationTabWidget(QWidget * graph)
{
  configurationDock_->setWidget(graph);
  configurationDock_->show();
}


void MainWindow::hideGraphConfigurationTabWidget(QWidget * graph)
{
  configurationDock_->close();
}
}