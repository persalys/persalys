//                                               -*- C++ -*-
/**
 *  @brief OTStudy
 *
 *  Copyright 2015-2019 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "persalys/MeshWindow.hxx"

#include "persalys/WidgetBoundToDockWidget.hxx"
#include "persalys/GraphConfigurationWidget.hxx"
#include "persalys/ExportableTableView.hxx"
#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/MeshDefinitionWizard.hxx"
#include "persalys/SampleTableModel.hxx"

#ifdef PERSALYS_HAVE_PARAVIEW
#include "persalys/PVServerManagerInterface.hxx"
#include "persalys/PVServerManagerSingleton.hxx"
#include "persalys/PVPlotSettingWidget.hxx"
#include "persalys/PVXYChartViewWidget.hxx"
#include "persalys/PVXYChartSettingWidget.hxx"
#include "persalys/PVSpreadSheetViewWidget.hxx"

#include <pqLinksModel.h>
#include <pqApplicationCore.h>
#endif

#include <QVBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QToolButton>
#include <QSortFilterProxyModel>
#include <QScrollArea>

using namespace OT;

namespace PERSALYS
{

MeshWindow::MeshWindow(MeshItem * item, QWidget * parent)
  : SubWindow(item, parent)
  , meshItem_(item)
{
  connect(item, SIGNAL(meshChanged()), this, SLOT(updateTable()));

  buildInterface();
}


void MeshWindow::buildInterface()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  mainLayout->addWidget(new TitleLabel(tr("Mesh"), "user_manual/graphical_interface/field_model/user_manual_field_model.html#meshdefinition"));

  // Index parameter
  QGroupBox * groupBox = new QGroupBox(tr("Index parameter"));
  QGridLayout * groupBoxLayout = new QGridLayout(groupBox);

  tableView_ = new CopyableTableView;
  tableView_->horizontalHeader()->setStretchLastSection(true);
  tableView_->verticalHeader()->hide();

  // fill in table
  QStringList headerLabels = QStringList() << tr("Name")
                                           << tr("Description")
                                           << tr("Minimum")
                                           << tr("Maximum")
                                           << tr("Number of nodes");
  tableModel_ = new CustomStandardItemModel(1, headerLabels.size(), tableView_);
  tableView_->setModel(tableModel_);

  // fill in table
  tableModel_->setHorizontalHeaderLabels(headerLabels);
  const MeshModel meshModel(meshItem_->getPhysicalModel().getMeshModel());
  const Variable param(meshModel.getIndexParameters()[0]);
  tableModel_->setItem(0, 0, new QStandardItem(QString::fromUtf8(param.getName().c_str())));
  tableModel_->setItem(0, 1, new QStandardItem(QString::fromUtf8(param.getDescription().c_str())));
  tableModel_->setNotEditableItem(0, 2, meshModel.getBounds().getLowerBound()[0]);
  tableModel_->setNotEditableItem(0, 3, meshModel.getBounds().getUpperBound()[0]);
  tableModel_->setNotEditableItem(0, 4, meshModel.getNumberOfNodes()[0]);
  tableModel_->item(0, 2)->setEnabled(false);
  tableModel_->item(0, 3)->setEnabled(false);
  tableModel_->item(0, 4)->setEnabled(false);
  connect(tableModel_, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(updateModel(QStandardItem *)));

  // resize table
  tableView_->resizeWithOptimalHeight();

  groupBoxLayout->addWidget(tableView_, 0, 0);

  // edit button
  QToolButton * editButton = new QToolButton;
  editButton->setText("...");
  connect(editButton, SIGNAL(clicked()), this, SLOT(editMesh()));
  groupBoxLayout->addWidget(editButton, 0, 1, Qt::AlignBottom);

  // isRegular mesh label
  isRegularLabel_ = new QLabel(tr("Regular mesh : %1").arg(meshModel.getMesh().isRegular() ? tr("Yes") : tr("No")));
  groupBoxLayout->addWidget(isRegularLabel_, 1, 0);

  mainLayout->addWidget(groupBox);

  // mesh display
  tabWidget_ = new QTabWidget;
  bool canUseParaview = false;
// #ifdef PERSALYS_HAVE_PARAVIEW
//   if (OTguiSubWindow::HaveOpenGL32())
//   {
//     addParaviewWidgetsTabs();
//     canUseParaview = true;
//   }
// #endif
  if (!canUseParaview)
  {
    addWidgetsTabs();
  }

  mainLayout->addWidget(tabWidget_, 1);
}


void MeshWindow::resizeEvent(QResizeEvent* event)
{
  SubWindow::resizeEvent(event);

  if (isVisible() && event->oldSize().width() > 0 && tableView_ && tableModel_)
  {
    tableView_->resizeWithOptimalHeight();
  }
}


void MeshWindow::editMesh()
{
  MeshDefinitionWizard wizard(meshItem_->getPhysicalModel().getMeshModel(), this);
  if (wizard.exec())
  {
    MeshModel meshModel(wizard.getMesh());
    meshItem_->getPhysicalModel().setMeshModel(meshModel);
  }
}


void MeshWindow::updateTable()
{
  const MeshModel meshModel(meshItem_->getPhysicalModel().getMeshModel());
  const Variable param(meshModel.getIndexParameters()[0]);
  tableModel_->setData(tableModel_->index(0, 0), param.getName().c_str());
  tableModel_->setData(tableModel_->index(0, 1), param.getDescription().c_str());
  tableModel_->setData(tableModel_->index(0, 2), QString::number(meshModel.getBounds().getLowerBound()[0], 'g', StudyTreeViewModel::DefaultSignificantDigits));
  tableModel_->setData(tableModel_->index(0, 3), QString::number(meshModel.getBounds().getUpperBound()[0], 'g', StudyTreeViewModel::DefaultSignificantDigits));
  tableModel_->setData(tableModel_->index(0, 4), QString::number(int(meshModel.getNumberOfNodes()[0]), 'g', StudyTreeViewModel::DefaultSignificantDigits));
  updatePlot();
  isRegularLabel_->setText(tr("Regular mesh : %1").arg(meshModel.getMesh().isRegular() ? tr("Yes") : tr("No")));
}


void MeshWindow::updateModel(QStandardItem *item)
{
  MeshModel meshModel(meshItem_->getPhysicalModel().getMeshModel());
  Variable param(meshModel.getIndexParameters()[0]);
  if (item->column() == 0)
    param.setName(item->text().toStdString());
  else if (item->column() == 1)
    param.setDescription(item->text().toStdString());
  VariableCollection coll;
  coll.add(param);
  meshModel.setIndexParameters(coll);
  meshItem_->getPhysicalModel().blockNotification("Mesh");
  meshItem_->getPhysicalModel().setMeshModel(meshModel);
  meshItem_->getPhysicalModel().blockNotification();
  updatePlot();
}


void MeshWindow::updatePlot()
{
  MeshModel meshModel(meshItem_->getPhysicalModel().getMeshModel());

  // update nodes table
  Sample sample(meshModel.getMesh().getVertices());
  nodesModel_->updateData(sample);
  // update plot
  Variable param(meshModel.getIndexParameters()[0]);
  meshPlot_->clear();
  if (!param.getDescription().empty())
    meshPlot_->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(param.getDescription().c_str()));
  else
    meshPlot_->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(param.getName().c_str()));
  meshPlot_->setTitle(tr("Number of nodes = %1").arg(meshModel.getNumberOfNodes()[0]));
  sample.stack(Sample(sample.getSize(), 1));
  meshPlot_->plotCurve(sample, QPen(Qt::black, 2), QwtPlotCurve::Lines, new QwtSymbol(QwtSymbol::XCross, Qt::red, QPen(Qt::red), QSize(7, 7)));
}

// void MeshWindow::addParaviewWidgetsTabs()
// {
//   // graph
//   WidgetBoundToDockWidget * mainWidget = new WidgetBoundToDockWidget(this);
//   QVBoxLayout * mainWidgetLayout = new QVBoxLayout(mainWidget);
// 
//   PVXYChartViewWidget * plotWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get());
// //   plotWidget->setData(designOfExperiment_.getInputSample(), Qt::blue);
//   mainWidgetLayout->addWidget(plotWidget);
// 
//   // scatter plots setting widget
//   PVXYChartSettingWidget * settingWidget = new PVXYChartSettingWidget(plotWidget,
//       Sample(),
//       Sample(),
//       Sample(),
//       Sample(),
//       Sample(),
//       Sample(),
//       PVXYChartSettingWidget::NoType,
//       this);
//   mainWidget->setDockWidget(settingWidget);
// 
//   tabWidget_->addTab(plotWidget, tr("Mesh"));
// 
//   // table
//   QWidget * tableView_ = new QWidget;
//   QVBoxLayout * tableViewLayout = new QVBoxLayout(tableView_);
// 
//   // with paraview the table is always shown in order to use the selection behavior
//   PVSpreadSheetViewWidget * pvSpreadSheetWidget = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
// //   pvSpreadSheetWidget->setData(designOfExperiment_.getSample());
//   connect(getItem(), SIGNAL(dataExportRequested()), pvSpreadSheetWidget, SLOT(exportData()));
// 
//   tableViewLayout->addWidget(pvSpreadSheetWidget);
// 
//   tabWidget_->addTab(tableView_, tr("Table"));
// }


void MeshWindow::addWidgetsTabs()
{
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  // Mesh
  WidgetBoundToDockWidget * plotWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * plotWidgetLayout = new QVBoxLayout(plotWidget);

  meshPlot_ = new PlotWidget();
//   meshPlot->plotCurve();
//   meshPlot_->setTitle(tr("Number of nodes = ") + " ");
//   meshPlot_->setAxisTitle(QwtPlot::xBottom, "time");

  // Graph Setting
  GraphConfigurationWidget * graphSetting = new GraphConfigurationWidget(meshPlot_,
      QStringList(),
      QStringList(),
      GraphConfigurationWidget::NoType,
      this);
  plotWidget->setDockWidget(graphSetting);

  plotWidgetLayout->addWidget(meshPlot_);

  scrollArea->setWidget(plotWidget);
  tabWidget_->addTab(scrollArea, tr("Mesh"));

  // nodes
  nodesView_ = new ExportableTableView;
//   connect(getItem(), SIGNAL(dataExportRequested()), nodesView, SLOT(exportData()));
  nodesView_->setSortingEnabled(true);
  nodesModel_ = new SampleTableModel(Sample(), nodesView_);

  // QSortFilterProxyModel
  QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(nodesView_);
  proxyModel->setSourceModel(nodesModel_);
  proxyModel->setSortRole(Qt::UserRole);
  nodesView_->setModel(proxyModel);

  tabWidget_->addTab(nodesView_, tr("Nodes"));
  updatePlot();
}
}
