#include "persalys/PVSpreadSheetViewWidget.hxx"

#include "persalys/ExportableTableView.hxx"
#include "persalys/SampleTableModel.hxx"

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
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

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


QWidget * PVSpreadSheetViewWidget::GetSpreadSheetViewWidget(PVSpreadSheetViewWidget *pvWidget, const OT::Sample &sample, Item *item)
{
  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  QHBoxLayout * hLayout = new QHBoxLayout;
  // - sample size
  QLabel * sizeLabel = new QLabel(tr("Size") + " : " + QString::number(sample.getSize()));
  hLayout->addWidget(sizeLabel);
  hLayout->addStretch();
  // - export button
  QPushButton * exportButton = new QPushButton(QIcon(":/images/document-export-table.png"), tr("Export"));
  hLayout->addWidget(exportButton);

  ExportableTableView * tableView = new ExportableTableView;
  connect(item, SIGNAL(dataExportRequested()), tableView, SLOT(exportData()));
  SampleTableModel * tableModel = new SampleTableModel(sample, tableView);
  tableView->setModel(tableModel);
  connect(exportButton, SIGNAL(clicked()), tableView, SLOT(exportData()));

  //connect(exportButton, SIGNAL(clicked()), pvWidget, SLOT(exportData()));
  mainLayout->addLayout(hLayout);

  // - table
  pvWidget->setData(sample);
  if (item)
    connect(item, SIGNAL(dataExportRequested()), pvWidget, SLOT(exportData()));
  mainLayout->addWidget(pvWidget);

  return mainWidget;
}
}
