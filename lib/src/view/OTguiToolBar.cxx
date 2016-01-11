#include "otgui/OTguiToolBar.hxx"

#include <QAction>

namespace OTGUI {

OTguiToolBar::OTguiToolBar()
  : QToolBar()
{
  buildActions();
}


void OTguiToolBar::buildActions()
{
  QList<QAction* > actions;

  QAction * action = new QAction(QIcon(":/images/document-new22x22.png"), tr("&New OTStudy"), this);
  action->setStatusTip(tr("Create a new OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(createNewOTStudy()));
  actions.append(action);

  action = new QAction(QIcon(":/images/document-import22x22.png"), tr("&Load OTStudy"), this);
  action->setStatusTip(tr("Load an OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(loadOTStudy()));
  actions.append(action);

  addActions(actions);
}
}