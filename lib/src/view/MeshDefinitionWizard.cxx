//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a mesh
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/MeshDefinitionWizard.hxx"

#include "otgui/GridMeshModel.hxx"
#include "otgui/ImportedMeshModel.hxx"
#include "otgui/FileTools.hxx"
#include "otgui/SampleTableModel.hxx"
#include "otgui/HorizontalHeaderViewWithCombobox.hxx"
#include "otgui/SpinBoxDelegate.hxx"

#include <QScrollBar>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QToolButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QGroupBox>

using namespace OT;

namespace OTGUI
{

MeshDefinitionWizard::MeshDefinitionWizard(const MeshModel& mesh, QWidget* parent)
  : Wizard(parent)
  , mesh_(mesh)
  , columns_()
  , methodGroup_(0)
  , tableModel_(0)
  , filePathLineEdit_(0)
  , dataPreviewTableView_(0)
  , sizeLabel_(0)
  , errorMessageLabel_(0)
{
  buildInterface();
}


void MeshDefinitionWizard::buildInterface()
{
  setWindowTitle(tr("Mesh definition"));

  QWizardPage * page = new QWizardPage;
  page->setTitle(tr("Define the mesh"));
  QVBoxLayout * pageLayout = new QVBoxLayout(page);
  pageLayout->setSizeConstraint(QLayout::SetMaximumSize);

  methodGroup_ = new QButtonGroup(page);

  // define manually
  QRadioButton * manuallyButton = new QRadioButton(tr("Regular mesh"));
  methodGroup_->addButton(manuallyButton, MeshDefinitionWizard::Grid);
  pageLayout->addWidget(manuallyButton);

  QStringList headerLabels = QStringList() << tr("Name")
                                           << tr("Description")
                                           << tr("Minimum")
                                           << tr("Maximum")
                                           << tr("Number of nodes");

  QTableView * tableView = new QTableView;
  tableView->setEditTriggers(QTableView::AllEditTriggers);
  tableView->horizontalHeader()->setStretchLastSection(true);
  tableView->verticalHeader()->hide();

  tableModel_ = new CustomStandardItemModel(1, headerLabels.size(), tableView);
  tableModel_->setHorizontalHeaderLabels(headerLabels);
  const Variable param(mesh_.getIndexParameters()[0]);
  tableModel_->setNotEditableItem(0, 0, QString::fromUtf8(param.getName().c_str()));
  tableModel_->setNotEditableItem(0, 1, QString::fromUtf8(param.getDescription().c_str()));
  tableModel_->setItem(0, 2, new QStandardItem(QString::number(mesh_.getBounds().getLowerBound()[0])));
  tableModel_->setItem(0, 3, new QStandardItem(QString::number(mesh_.getBounds().getUpperBound()[0])));
  tableModel_->setItem(0, 4, new QStandardItem(QString::number(mesh_.getNumberOfNodes()[0])));
  tableView->setModel(tableModel_);

  SpinBoxDelegate * spinBoxDelegate = new SpinBoxDelegate(tableView);
  spinBoxDelegate->setSpinBoxType(SpinBoxDelegate::doubleValue);
  for (int i = 2; i < tableModel_->columnCount() - 1; ++i)
    tableView->setItemDelegateForColumn(i, spinBoxDelegate);
  spinBoxDelegate = new SpinBoxDelegate(tableView);
  tableView->setItemDelegateForColumn(4, spinBoxDelegate);

  // resize table
  const int h = tableView->verticalHeader()->length() + tableView->horizontalHeader()->height();
  int x1, y1, x2, y2;
  tableView->getContentsMargins(&x1, &y1, &x2, &y2);
  tableView->setFixedHeight(h + y1 + y2);

  QWidget * aWidget = new QWidget;
  QVBoxLayout * aLayout = new QVBoxLayout(aWidget);
  aLayout->addWidget(tableView);
  pageLayout->addWidget(aWidget);

  // import
  QRadioButton * importButton = new QRadioButton(tr("Import mesh"));
  methodGroup_->addButton(importButton, MeshDefinitionWizard::Import);
  pageLayout->addWidget(importButton);

  // file path
  QWidget * importWidget = new QWidget;
  QVBoxLayout * importLayout = new QVBoxLayout(importWidget);

  QHBoxLayout * hboxLayout = new QHBoxLayout;
  QLabel * label = new QLabel(tr("File"));
  hboxLayout->addWidget(label);

  filePathLineEdit_ = new QLineEdit;
  hboxLayout->addWidget(filePathLineEdit_);

  QToolButton * openFileButton = new QToolButton;
  openFileButton->setText("...");
  connect(openFileButton, SIGNAL(clicked()), this, SLOT(openFileRequested()));
  hboxLayout->addWidget(openFileButton);

  importLayout->addLayout(hboxLayout);

  // file preview
  QGroupBox * groupBox = new QGroupBox(tr("File Preview"));
  QGridLayout * gridLayout = new QGridLayout(groupBox);
  gridLayout->setSpacing(6);
  gridLayout->setContentsMargins(11, 11, 11, 11);

  // DOE size
  QHBoxLayout * sizeLayout = new QHBoxLayout;
  QLabel * sizeLabel = new QLabel(tr("Size") + " : ");
  sizeLayout->addWidget(sizeLabel);
  sizeLabel_ = new QLabel(QString::number(0));
  sizeLayout->addWidget(sizeLabel_);
  sizeLayout->addStretch();
  gridLayout->addLayout(sizeLayout, 0, 0);

  dataPreviewTableView_ = new ExportableTableView(groupBox);
  gridLayout->addWidget(dataPreviewTableView_, 1, 0, 1, 1);

  importLayout->addWidget(groupBox);

  pageLayout->addWidget(importWidget);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  pageLayout->addWidget(errorMessageLabel_);

  connect(tableModel_, SIGNAL(dataChanged(QModelIndex, QModelIndex)), errorMessageLabel_, SLOT(reset()));
  connect(manuallyButton, SIGNAL(toggled(bool)), errorMessageLabel_, SLOT(reset()));

  connect(importButton, SIGNAL(toggled(bool)), importWidget, SLOT(setEnabled(bool)));
  connect(importButton, SIGNAL(toggled(bool)), tableView, SLOT(setDisabled(bool)));

  const ImportedMeshModel * importedMeshModel = dynamic_cast<ImportedMeshModel*>(mesh_.getImplementation().get());
  if (importedMeshModel)
  {
    importButton->click();
    columns_ = importedMeshModel->getColumns();
    setData(QString::fromUtf8(importedMeshModel->getFileName().c_str()));
  }
  else
  {
    manuallyButton->click();
    importWidget->setEnabled(false);
    columns_ = Indices(1, 0);
  }

  addPage(page);
}


void MeshDefinitionWizard::openFileRequested()
{
  QString fileName = QFileDialog::getOpenFileName(this,
                     tr("Data to import..."),
                     FileTools::GetCurrentDir(),
                     tr("Data files (*.csv *.txt)"));

  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    FileTools::SetCurrentDir(fileName);

    // check
    if (!file.open(QFile::ReadOnly))
    {
      QMessageBox::warning(this,
                           tr("Warning"),
                           tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    }
    else
    {
      setData(fileName);
    }
  }
}


void MeshDefinitionWizard::setData(const QString& fileName)
{
  filePathLineEdit_->setText(fileName);
  try
  {
    errorMessageLabel_->reset();
    setTable(fileName);
  }
  catch (std::exception & ex)
  {
    dataPreviewTableView_->setModel(0);
    // DOE size
    sizeLabel_->setText("");
    errorMessageLabel_->setErrorMessage(tr("Impossible to load the file.%1%2").arg("\n").arg(ex.what()));
  }
}


void MeshDefinitionWizard::setTable(const QString& fileName)
{
  // get sample From File
  Sample sample(Tools::ImportSample(fileName.toStdString()));

  // get input names
  const Description paramNames(1, mesh_.getIndexParameters()[0].getName());

  // get inputs columns indices
  Indices columns(columns_);
  if (!columns.check(sample.getDimension()))
    columns = Indices(1, 0);

  // set sample description
  Description desc(sample.getDimension());
  for (UnsignedInteger i = 0; i < columns.getSize(); ++i)
    desc[columns[i]] = paramNames[i];
  sample.setDescription(desc);

  // set table model
  dataPreviewTableView_->setModel(new SampleTableModel(sample, false, dataPreviewTableView_));
  connect(dataPreviewTableView_->model(), SIGNAL(headerDataChanged(Qt::Orientation, int, int)), errorMessageLabel_, SLOT(reset()));

  // set comboboxes items: each of them contains the input Names and an empty item
  QStringList comboBoxItems;
  for (UnsignedInteger i = 0; i < paramNames.getSize(); ++i)
    comboBoxItems << QString::fromUtf8(paramNames[i].c_str());
  comboBoxItems << "";

  // set horizontal header view
  QVector<int> columnsWithCombo(sample.getDimension());
  for (int i = 0; i < columnsWithCombo.size(); ++i)
    columnsWithCombo[i] = i;
  HorizontalHeaderViewWithCombobox * header = new HorizontalHeaderViewWithCombobox(comboBoxItems, columnsWithCombo, dataPreviewTableView_);
  dataPreviewTableView_->setHorizontalHeader(header);
  connect(dataPreviewTableView_->horizontalScrollBar(), SIGNAL(valueChanged(int)), header, SLOT(fixComboPositions()));
  dataPreviewTableView_->horizontalHeader()->show();
#if QT_VERSION >= 0x050000
  dataPreviewTableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
  dataPreviewTableView_->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
#endif

  // size
  sizeLabel_->setText(QString::number(sample.getSize()));
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
    Indices column;
    if (dataPreviewTableView_->model())
    {
      for (int i = 0; i < dataPreviewTableView_->model()->columnCount(); ++i)
      {
        if (newMesh.getIndexParameters()[0].getName() == dataPreviewTableView_->model()->headerData(i, Qt::Horizontal).toString().toStdString())
        {
          column.add(i);
          break;
        }
      }
    }
    newMesh = ImportedMeshModel(mesh_.getIndexParameters(), filePathLineEdit_->text().toStdString(), column);
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
      errorMessageLabel_->setErrorMessage(tr("The lower bound must be lesser than the upper bound"));
    if (tableModel_->item(0, 4)->data(Qt::DisplayRole).toInt() < 2)
      errorMessageLabel_->setErrorMessage(tr("The mesh must contain at least two nodes"));
  }
  else
  {
    if (filePathLineEdit_->text().isEmpty())
      errorMessageLabel_->setErrorMessage(tr("The file name is empty"));
    else if (!dataPreviewTableView_->model())
      errorMessageLabel_->setErrorMessage("No sample");
    else
    {
      Indices column;
      for (int i = 0; i < dataPreviewTableView_->model()->columnCount(); ++i)
      {
        if (mesh_.getIndexParameters()[0].getName() == dataPreviewTableView_->model()->headerData(i, Qt::Horizontal).toString().toStdString())
          column.add(i);
      }
      if (column.getSize() != 1)
        errorMessageLabel_->setErrorMessage(tr("Choose a column"));

      if (dataPreviewTableView_->model()->rowCount() < 2)
        errorMessageLabel_->setErrorMessage("The mesh must contain at least two nodes");
    }
  }
  return errorMessageLabel_->text().isEmpty();
}
}
