#include "persalys/PVSpreadSheetViewWidget.hxx"

#include <pqSaveDataReaction.h>
#include <pqActiveObjects.h>
#include <pqApplicationCore.h>
#include <vtkSMSourceProxy.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkIdTypeArray.h>
#include <vtkSMSelectionHelper.h>
#include <vtkSMProxyManager.h>
#include <pqSelectionManager.h>

#include <QMenu>

namespace PERSALYS
{
const char PVSpreadSheetViewWidget::PV_VIEW_TYPE[] = "SpreadSheetView";

const char PVSpreadSheetViewWidget::PV_REPRESENTATION_TYPE[] = "SpreadSheetRepresentation";

PVSpreadSheetViewWidget::PVSpreadSheetViewWidget(QWidget *parent, PVServerManagerInterface *smb)
  : PVViewWidget(parent, smb, PV_VIEW_TYPE)
{
  // context menu
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
}


PVSpreadSheetViewWidget::~PVSpreadSheetViewWidget()
{
}


// show the context menu when right clicking
void PVSpreadSheetViewWidget::contextMenu(const QPoint& pos)
{
  // action
  QAction * exportDataAction = new QAction(QIcon(":/images/document-export-table.png"), tr("Export"), this);
  connect(exportDataAction, SIGNAL(triggered(bool)), this, SLOT(exportData()));

  // menu
  QMenu * contextMenu(new QMenu(this));
  contextMenu->addAction(exportDataAction);
  contextMenu->popup(mapToGlobal(pos));
}


void PVSpreadSheetViewWidget::exportData()
{
  pqActiveObjects::instance().setActivePort(getPort());
  pqSaveDataReaction::saveActiveData();
}
}
