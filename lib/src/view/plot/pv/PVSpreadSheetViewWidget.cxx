#include "persalys/PVSpreadSheetViewWidget.hxx"

#include "persalys/ExportableTableView.hxx"
#include "persalys/SampleTableModel.hxx"

#include <pqSpreadSheetViewDecorator.h>
#include <pqSpreadSheetViewModel.h>
#include <pqActiveObjects.h>
#include <pqSpreadSheetView.h>
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
  connect(exportDataAction, SIGNAL(triggered(bool)), this, SIGNAL(exportDataRequested()));

  QAction * copyDataAction = new QAction(QIcon(":/images/document-export.png"), tr("Copy"), this);
  connect(copyDataAction, SIGNAL(triggered(bool)), this, SIGNAL(copyDataRequested()));

  // menu
  QMenu * contextMenu(new QMenu(this));
  contextMenu->addAction(exportDataAction);
  contextMenu->addAction(copyDataAction);
  contextMenu->popup(mapToGlobal(pos));
}


QWidget * PVSpreadSheetViewWidget::GetSpreadSheetViewWidget(PVSpreadSheetViewWidget *pvWidget, const OT::Sample &sample, Item *item, const OT::Description& errorDesc)
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
  SampleTableModel * tableModel = new SampleTableModel(sample, tableView);
  tableView->setModel(tableModel);
  connect(exportButton, SIGNAL(clicked()), tableView, SLOT(exportData()));
  connect(pvWidget, SIGNAL(exportDataRequested()), tableView, SLOT(exportData()));


  mainLayout->addLayout(hLayout);

  // - table
  if(errorDesc.getSize())
    pvWidget->setData(sample, errorDesc);
  else
    pvWidget->setData(sample);

  if (item)
    connect(item, SIGNAL(dataExportRequested()), tableView, SLOT(exportData()));
  mainLayout->addWidget(pvWidget);

  // add decorator
  pqSpreadSheetView * view = qobject_cast<pqSpreadSheetView*>(pvWidget->getView());
  if (view)
  {
    pqSpreadSheetViewDecorator * decorator = new pqSpreadSheetViewDecorator(view);
    // hide decorator header
    // see header location at: pqSpreadSheetViewDecorator::pqSpreadSheetViewDecorator
    // ParaView-v5.10.1/Qt/ApplicationComponents/pqSpreadSheetViewDecorator.cxx:361
    QVBoxLayout* decoratorLayout = qobject_cast<QVBoxLayout*>(view->widget()->layout());
    if(decoratorLayout)
    {
      decoratorLayout->itemAt(0)->widget()->setVisible(false);
    }
    else
      throw OT::InternalException(HERE) << "Cannot find header layout from PVSpreadSheetViewWidget";
    connect(pvWidget, SIGNAL(copyDataRequested()), decorator, SLOT(copyToClipboard()));
  }

  return mainWidget;
}
}
