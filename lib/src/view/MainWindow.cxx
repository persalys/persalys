#include "otgui/MainWindow.hxx"

#include "otgui/PhysicalModelWindow.hxx"

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
  buildActions();
  connect(studyTree_, SIGNAL(showWindow(QMdiSubWindow *)), this, SLOT(showSubWindow(QMdiSubWindow *)));
  connect(studyTree_, SIGNAL(itemSelected(QStandardItem*)), this, SLOT(showSubWindow(QStandardItem *)));
  connect(studyTree_, SIGNAL(checkIfWindowResultExists(AnalysisItem *)), this, SLOT(checkIfWindowResultExists(AnalysisItem *)));
  setWindowTitle("OTGui");
}


void MainWindow::buildInterface()
{
  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  QSplitter * mainSplitter = new QSplitter(Qt::Horizontal);
  mainSplitter->addWidget(studyTree_);
  mainSplitter->addWidget(mdiArea_);
  mainLayout->addWidget(mainSplitter);

  // Python Console
  console_ = new QPyConsole(this);
  connect(studyTree_, SIGNAL(loadPythonScript(const QString &)), console_, SLOT(loadScript(const QString &)));

  QDockWidget * consoleDock = new QDockWidget;
  consoleDock->setWidget(console_);
  consoleDock->setFeatures(QDockWidget::DockWidgetClosable);
  addDockWidget(Qt::BottomDockWidgetArea, consoleDock);
  //TODO:
//   connect(consoleDock, SIGNAL(visibilityChanged(bool)), this,SLOT(consoleVisibilityChanged()));

  setCentralWidget(mainWidget);

  // Menus
  menu_ = menuBar()->addMenu(tr("&Menu"));
}


void MainWindow::buildActions()
{
  menu_->addAction(tr("&New OTStudy"), studyTree_, SLOT(createNewOTStudy()));
  menu_->addAction(tr("&Load OTStudy"), studyTree_, SLOT(loadOTStudy()));
  menu_->addAction(tr("E&xit"), this, SLOT(close()), QKeySequence::Quit);
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
  win->show();
}


void MainWindow::showSubWindow(QStandardItem * item)
{
  QList<QMdiSubWindow *> listSubWindow =  mdiArea_->subWindowList();

  for (int i = 0; i < listSubWindow.size(); ++i)
  {
    OTguiSubWindow * win = static_cast<OTguiSubWindow*>(listSubWindow.at(i));
    if ( win->getItem() == item)
    {
      win->widget()->show();
      mdiArea_->setActiveSubWindow(win);
    }
  }

}


void MainWindow::checkIfWindowResultExists(AnalysisItem* item)
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


}