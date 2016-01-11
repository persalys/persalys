#include "otgui/OTguiMenuBar.hxx"

#include <QAction>

namespace OTGUI {

OTguiMenuBar::OTguiMenuBar()
  : QMenuBar()
{
  buildActions();
}


void OTguiMenuBar::buildActions()
{
  QMenu * menu = new QMenu(tr("&Menu"));
  QList<QAction* > actions;

  QAction * action = new QAction(QIcon(":/images/document-new.png"), tr("&New OTStudy"), this);
  action->setStatusTip(tr("Create a new OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(createNewOTStudy()));
  actions.append(action);

  action = new QAction(QIcon(":/images/document-import.png"), tr("&Load OTStudy"), this);
  action->setStatusTip(tr("Load an OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(loadOTStudy()));
  actions.append(action);

  action = new QAction(QIcon(":/images/window-close.png"), tr("E&xit"), this);
  connect(action, SIGNAL(triggered()), this, SIGNAL(closeWindow()));
  actions.append(action);

  menu->addActions(actions);
  addMenu(menu);
}
}