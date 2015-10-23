#include "MainWindow.hxx"

#include "PhysicalModelWindow.hxx"

#include <QSplitter>
#include <QMenuBar>
#include <QDockWidget>

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
  connect(studyTree_, SIGNAL(checkIfWindowResultExists(CalculusItem *)), this, SLOT(checkIfWindowResultExists(CalculusItem *)));
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
  menu_->addAction(tr("&New study"), studyTree_, SLOT(createNewStudy()));
  menu_->addAction(tr("&Load study"), studyTree_, SLOT(loadStudy()));
  menu_->addAction(tr("E&xit"), this, SLOT(close()), QKeySequence::Quit);
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


void MainWindow::checkIfWindowResultExists(CalculusItem* item)
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