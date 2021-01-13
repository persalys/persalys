//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a mesh
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/MeshDefinitionWizard.hxx"

#include "persalys/SpinBoxDelegate.hxx"

#include <QHBoxLayout>
#include <QRadioButton>
#include <QHeaderView>

using namespace OT;

namespace PERSALYS
{

MeshDefinitionWizard::MeshDefinitionWizard(const MeshModel& mesh, QWidget* parent)
  : Wizard(parent)
  , mesh_(mesh)
  , methodGroup_(0)
  , tableModel_(0)
  , errorMessageLabel_(0)
{
  buildInterface();
}


void MeshDefinitionWizard::buildInterface()
{
  setWindowTitle(tr("Mesh definition"));
  docLink_ = "user_manual/graphical_interface/field_model/user_manual_field_model.html#meshdefinitionwizard";

  QWizardPage * page = new QWizardPage;
  page->setTitle(tr("Define the mesh"));
  QVBoxLayout * pageLayout = new QVBoxLayout(page);
  pageLayout->setSizeConstraint(QLayout::SetMaximumSize);

  methodGroup_ = new QButtonGroup(page);

  // define manually
  QRadioButton * manuallyButton = new QRadioButton(tr("Regular mesh"));
  methodGroup_->addButton(manuallyButton, MeshDefinitionWizard::Grid);
  pageLayout->addWidget(manuallyButton);

  // build table
  QStringList headerLabels = QStringList() << tr("Name")
                                           << tr("Description")
                                           << tr("Minimum")
                                           << tr("Maximum")
                                           << tr("Number of nodes");

  tableView_ = new CopyableTableView;
  tableView_->setEditTriggers(QTableView::AllEditTriggers);
  tableView_->horizontalHeader()->setStretchLastSection(true);
  tableView_->verticalHeader()->hide();

  tableModel_ = new CustomStandardItemModel(1, headerLabels.size(), tableView_);
  tableModel_->setHorizontalHeaderLabels(headerLabels);
  const Variable param(mesh_.getIndexParameters()[0]);
  tableModel_->setNotEditableItem(0, 0, QString::fromUtf8(param.getName().c_str()));
  tableModel_->setNotEditableItem(0, 1, QString::fromUtf8(param.getDescription().c_str()));
  tableModel_->setItem(0, 2, new QStandardItem(QString::number(mesh_.getBounds().getLowerBound()[0])));
  tableModel_->setItem(0, 3, new QStandardItem(QString::number(mesh_.getBounds().getUpperBound()[0])));
  tableModel_->setItem(0, 4, new QStandardItem(QString::number(mesh_.getNumberOfNodes()[0])));
  tableView_->setModel(tableModel_);

  SpinBoxDelegate * spinBoxDelegate = new SpinBoxDelegate(tableView_);
  spinBoxDelegate->setSpinBoxType(SpinBoxDelegate::doubleValue);
  for (int i = 2; i < tableModel_->columnCount() - 1; ++i)
    tableView_->setItemDelegateForColumn(i, spinBoxDelegate);
  spinBoxDelegate = new SpinBoxDelegate(tableView_);
  tableView_->setItemDelegateForColumn(4, spinBoxDelegate);

  // resize table
  tableView_->resizeWithOptimalHeight();

  QWidget * aWidget = new QWidget;
  QVBoxLayout * aLayout = new QVBoxLayout(aWidget);
  aLayout->addWidget(tableView_);
  pageLayout->addWidget(aWidget, 0, Qt::AlignTop);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  pageLayout->addWidget(errorMessageLabel_);

  // import
  QRadioButton * importButton = new QRadioButton(tr("Import mesh"));
  methodGroup_->addButton(importButton, MeshDefinitionWizard::Import);
  pageLayout->addWidget(importButton);

  sampleWidget_ = new ImportSampleWidget;
  pageLayout->addWidget(sampleWidget_);
  connect(sampleWidget_, SIGNAL(updateTableRequested(QString)), this, SLOT(setTable(QString)));
  connect(sampleWidget_, SIGNAL(checkColumnsRequested()), this, SLOT(checkColumns()));

  addPage(page);

  // connections
  connect(tableModel_, SIGNAL(dataChanged(QModelIndex, QModelIndex)), errorMessageLabel_, SLOT(reset()));
  connect(importButton, SIGNAL(toggled(bool)), sampleWidget_, SLOT(setEnabled(bool)));
  connect(importButton, SIGNAL(toggled(bool)), tableView_, SLOT(setDisabled(bool)));
  connect(importButton, SIGNAL(toggled(bool)), errorMessageLabel_, SLOT(setDisabled(bool)));

  // initialize widgets
  ImportedMeshModel * importedMeshModel = dynamic_cast<ImportedMeshModel*>(mesh_.getImplementation().get());
  GridMeshModel * gridMeshModel = dynamic_cast<GridMeshModel*>(mesh_.getImplementation().get());

  if (importedMeshModel)
  {
    importButton->click();
    importedMesh_ = *importedMeshModel;
    sampleWidget_->setData(QString::fromUtf8(importedMeshModel->getFileName().c_str()));
  }
  else if (gridMeshModel)
  {
    manuallyButton->click();
    gridMesh_ = *gridMeshModel;
    sampleWidget_->setEnabled(false);
  }
  else
    throw InvalidArgumentException(HERE) << "Unknown mesh class name";
}


void MeshDefinitionWizard::resizeEvent(QResizeEvent* event)
{
  QWizard::resizeEvent(event);

  if (isVisible() && event->oldSize().width() > 0 && tableView_ && tableModel_)
  {
    tableView_->resizeWithOptimalHeight();
  }
}


void MeshDefinitionWizard::setTable(const QString& fileName)
{
  // set file name
  importedMesh_.setFileName(fileName.toUtf8().data());
  // update widgets
  sampleWidget_->updateWidgets(importedMesh_.getSampleFromFile(),
                               Description(1, mesh_.getIndexParameters()[0].getName()),
                               importedMesh_.getInputColumns());
}


void MeshDefinitionWizard::checkColumns()
{
  const Description paramNames(1, mesh_.getIndexParameters()[0].getName());

  // try to update the model
  try
  {
    importedMesh_.setParameterColumns(sampleWidget_->getColumns(paramNames));
    sampleWidget_->tableValidity_ = true;
    sampleWidget_->errorMessageLabel_->reset();
  }
  catch (InvalidArgumentException & ex)
  {
    sampleWidget_->errorMessageLabel_->setErrorMessage(tr("The parameter must be associated with one column."));
    sampleWidget_->tableValidity_ = false;
  }
}


MeshModel MeshDefinitionWizard::getMesh() const
{
  MeshModel newMesh;
  if (methodGroup_->checkedId() == MeshDefinitionWizard::Grid)
  {
    const Interval bounds(tableModel_->item(0, 2)->data(Qt::DisplayRole).toDouble(), tableModel_->item(0, 3)->data(Qt::DisplayRole).toDouble());
    newMesh = GridMeshModel(mesh_.getIndexParameters(), bounds, Indices(1, tableModel_->item(0, 4)->data(Qt::DisplayRole).toInt()));
  }
  else
  {
    newMesh = ImportedMeshModel(mesh_.getIndexParameters(), importedMesh_.getFileName(), importedMesh_.getInputColumns());
  }
  return newMesh;
}


bool MeshDefinitionWizard::validateCurrentPage()
{
  errorMessageLabel_->reset();
  if (methodGroup_->checkedId() == MeshDefinitionWizard::Grid)
  {
    const Interval bounds(tableModel_->item(0, 2)->data(Qt::DisplayRole).toDouble(), tableModel_->item(0, 3)->data(Qt::DisplayRole).toDouble());
    if (bounds.isNumericallyEmpty())
      errorMessageLabel_->setErrorMessage(tr("The lower bound must be less than the upper bound"));
    if (tableModel_->item(0, 4)->data(Qt::DisplayRole).toInt() < 2)
      errorMessageLabel_->setErrorMessage(tr("The mesh must contain at least two nodes"));
    return errorMessageLabel_->text().isEmpty();
  }
  else
  {
    return sampleWidget_->tableValidity_;
  }
}
}
