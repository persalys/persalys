//                                               -*- C++ -*-
/**
 *  @brief OTStudy
 *
 *  Copyright 2015-2025 EDF-Phimeca
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

MeshWindow::MeshWindow(PhysicalModelItem * item, QWidget * parent)
  : SubWindow(item, parent)
  , meshItem_(item)
{
  connect(item, SIGNAL(meshChanged()), this, SLOT(updateTable()));

  buildInterface();
}


MeshWindow::MeshWindow(DataFieldModelItem * item, QWidget * parent)
  : SubWindow(item, parent)
  , dataMeshItem_(item)
{
  connect(item, SIGNAL(meshChanged()), this, SLOT(updateTable()));
  connect(item, SIGNAL(meshOverwritten()), this, SIGNAL(meshOverwritten()));
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
  const MeshModel meshModel = getMeshModel();
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

  // Error message if mesh gets overwritten
  errorMessageLabel_ = new TemporaryLabel;
  groupBoxLayout->addWidget(errorMessageLabel_, 2, 0);
  mainLayout->addWidget(groupBox);

  connect(this, &MeshWindow::meshOverwritten, [ = ] ()
  {
    errorMessageLabel_->setTemporaryErrorMessage(tr("Warning: Specified mesh is incompatbile and has been reset."));
  });

  // mesh display
  tabWidget_ = new QTabWidget;
  bool canUseParaview = false;

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
  MeshDefinitionWizard wizard(getMeshModel(), (Bool)dataMeshItem_, this);
  if (wizard.exec())
  {
    MeshModel meshModel(wizard.getMesh());
    if (meshItem_)
      meshItem_->getPhysicalModel().setMeshModel(meshModel);
    if (dataMeshItem_)
      dataMeshItem_->getDataFieldModel().setMeshModel(meshModel);
  }
}


void MeshWindow::updateTable()
{
  const MeshModel meshModel = getMeshModel();
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
  MeshModel meshModel(getMeshModel());
  Variable param(meshModel.getIndexParameters()[0]);
  if (item->column() == 0)
    param.setName(item->text().toStdString());
  else if (item->column() == 1)
    param.setDescription(item->text().toStdString());
  VariableCollection coll;
  coll.add(param);
  meshModel.setIndexParameters(coll);
  if (meshItem_)
  {
    meshItem_->getPhysicalModel().blockNotification("MeshItem");
    meshItem_->getPhysicalModel().setMeshModel(meshModel);
    meshItem_->getPhysicalModel().blockNotification();
  }
  else if (dataMeshItem_)
  {
    // notifications are blocked to avoid infinite loop
    dataMeshItem_->getDataFieldModel().blockNotification("DataMeshItem");
    // but setting mesh model implies meshChanged signal which is connected to
    // DataFieldModelWindow::updateProcessSample and mesh can be overwritten there if not compatible
    // if so we need to catch notify("meshOverwritten") to set errorMessageLabel_ warning
    dataMeshItem_->getDataFieldModel().setMeshModel(meshModel);
    dataMeshItem_->getDataFieldModel().blockNotification();
  }
  updatePlot();
  // Need table update in case mesh gets overwritten
  updateTable();
}


void MeshWindow::updatePlot()
{
  MeshModel meshModel(getMeshModel());

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


void MeshWindow::addWidgetsTabs()
{
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  // Mesh
  meshPlot_ = new PlotWidget;

  // Graph Setting
  SimpleGraphSetting * graphSetting = new SimpleGraphSetting(meshPlot_, this);

  scrollArea->setWidget(new WidgetBoundToDockWidget(meshPlot_, graphSetting, this));
  tabWidget_->addTab(scrollArea, tr("Mesh"));

  // nodes
  nodesView_ = new ExportableTableView;
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

MeshModel MeshWindow::getMeshModel() const
{
  if (meshItem_)
    return meshItem_->getPhysicalModel().getMeshModel();
  else if (dataMeshItem_)
    return dataMeshItem_->getDataFieldModel().getMeshModel();
  else
    throw InvalidArgumentException(HERE) << "MeshWindow::getMeshModel : cannot get MeshModel";
}
}
