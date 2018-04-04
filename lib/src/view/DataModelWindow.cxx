//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the data model
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
#include "otgui/DataModelWindow.hxx"

#include "otgui/ComboBoxDelegate.hxx"
#include "otgui/FileTools.hxx"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QToolButton>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>

using namespace OT;

namespace OTGUI
{

DataModelWindow::DataModelWindow(DataModelDefinitionItem * item, QWidget * parent)
  : OTguiSubWindow(item, parent)
  , dataModel_(0)
  , filePathLineEdit_(0)
  , defaultLineEditPalette_()
  , sampleSizeLabel_(0)
  , dataTableView_(0)
  , dataTableModel_(0)
  , proxyModel_(0)
{
  dataModel_ = dynamic_cast<DataModel*>(item->getDesignOfExperiment().getImplementation().get());
  if (!dataModel_)
    throw InvalidArgumentException(HERE) << "DataModelWindow: the design of experiments must be a DataModel";

  connect(item, SIGNAL(variablesChanged()), this, SLOT(updateTableView()));

  buildInterface();
}


DataModelWindow::~DataModelWindow()
{
  dataModel_ = 0;
}


void DataModelWindow::buildInterface()
{
  setWindowTitle(tr("Data model definition"));

  QWidget * mainWidget = new QWidget;
  QGridLayout * mainGridLayout = new QGridLayout(mainWidget);

  // first row
  QHBoxLayout * hboxLayout = new QHBoxLayout;
  QLabel * label = new QLabel(tr("Data file"));
  hboxLayout->addWidget(label);

  // file path line edit
  filePathLineEdit_ = new QLineEdit;
  defaultLineEditPalette_ = filePathLineEdit_->palette();
  filePathLineEdit_->setReadOnly(true);
  filePathLineEdit_->setText(dataModel_->getFileName().c_str());
  hboxLayout->addWidget(filePathLineEdit_);

  // open file button
  QToolButton * openFileButton = new QToolButton;
  openFileButton->setText("...");
  connect(openFileButton, SIGNAL(clicked()), this, SLOT(openFileRequested()));
  hboxLayout->addWidget(openFileButton);

  // reload button
  QToolButton * reloadButton = new QToolButton;
  reloadButton->setIcon(QIcon(":/images/view-refresh.png"));
  connect(reloadButton, SIGNAL(clicked()), this, SLOT(refreshTable()));
  hboxLayout->addWidget(reloadButton);

  mainGridLayout->addLayout(hboxLayout, 0, 0, 1, 3);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  mainGridLayout->addWidget(errorMessageLabel_, 1, 0, 1, 1);

  // file preview
  QGroupBox * groupBox = new QGroupBox(tr("Sample"));
  QGridLayout * gridLayout = new QGridLayout(groupBox);
  gridLayout->setSpacing(6);
  gridLayout->setContentsMargins(11, 11, 11, 11);

  // sample size
  QGridLayout * sizeLayout = new QGridLayout;
  QLabel * sizeLabel = new QLabel(tr("Size") + " : ");
  sizeLayout->addWidget(sizeLabel, 0, 0);
  sampleSizeLabel_ = new QLabel;
  sizeLayout->addWidget(sampleSizeLabel_, 0, 1);
  sizeLayout->setColumnStretch(1, 1);
  sizeLayout->setSizeConstraint(QLayout::SetFixedSize);
  gridLayout->addLayout(sizeLayout, 0, 0, 1, 1);

  // - table view
  dataTableView_ = new ExportableTableView(groupBox);
  dataTableView_->setSortingEnabled(true);
  dataTableView_->setEditTriggers(QTableView::SelectedClicked);
  dataTableView_->setItemDelegateForRow(1, new ComboBoxDelegate(dataTableView_));

  // - table model
  dataTableModel_ = new DataModelTableModel(dataModel_, dataTableView_);
  connect(dataTableModel_, SIGNAL(errorMessageChanged(QString)), this, SLOT(setErrorMessage(QString)));
  connect(dataTableModel_, SIGNAL(temporaryErrorMessageChanged(QString)), this, SLOT(setTemporaryErrorMessage(QString)));

  // - QSortFilterProxyModel
  proxyModel_ = new DataModelProxModel(dataTableView_);
  proxyModel_->setSourceModel(dataTableModel_);
  proxyModel_->setSortRole(Qt::UserRole);

  // - set model
  dataTableView_->setModel(proxyModel_);

  gridLayout->addWidget(dataTableView_, 1, 0, 1, 1);

  mainGridLayout->addWidget(groupBox, 2, 0, 1, 1);

  // if the model has a sample: fill the table
  if (dataModel_->getSample().getSize())
  {
    // table view
    updateTableView(false);
  }

  setWidget(mainWidget);
}


void DataModelWindow::updateTable(const QString& fileName)
{
  // re-initialization
  setErrorMessage("");

  // try to retrieve data from the selected file
  try
  {
    // update file name
    dataModel_->setFileName(fileName.toLocal8Bit().data());
    filePathLineEdit_->setText(fileName);
  }
  catch (std::exception& ex)
  {
    QMessageBox::warning(this,
                         tr("Warning"),
                         tr("Cannot update the table.\n%1").arg(ex.what()));
  }
}


void DataModelWindow::openFileRequested()
{
  const QString fileName = QFileDialog::getOpenFileName(this,
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
      updateTable(fileName);
    }
  }
}


void DataModelWindow::refreshTable()
{
  if (!dataModel_->getFileName().empty())
  {
    updateTable(QString::fromUtf8(dataModel_->getFileName().c_str()));
  }
}


void DataModelWindow::updateTableView(const bool useSampleFromFile)
{
  // clear table
  dataTableView_->clearSpans();
  dataTableView_->clearSelection();

  // block signal of the selection model to avoid a crash in DataModelTableModel::endResetModel()
  const bool block = dataTableView_->selectionModel()->blockSignals(true);
  dataTableModel_->updateData(useSampleFromFile);
  dataTableView_->selectionModel()->blockSignals(block);

  dataTableView_->sortByColumn(0, Qt::AscendingOrder);

  // use comboboxes to define the variable type
  if (useSampleFromFile)
  {
    for (int i = 0; i < proxyModel_->columnCount(); ++i)
      dataTableView_->openPersistentEditor(proxyModel_->index(1, i));
  }
  else
  {
    // table span
    const UnsignedInteger nbInputs = dataModel_->getInputSample().getSize() ? dataModel_->getInputSample().getDimension() : 0;
    if (nbInputs > 1)
      dataTableView_->setSpan(1, 0, 1, nbInputs);

    if (dataModel_->getOutputSample().getSize() && dataModel_->getOutputSample().getDimension() > 1)
      dataTableView_->setSpan(1, nbInputs, 1, dataModel_->getSample().getDimension());
  }
  sampleSizeLabel_->setText(QString::number(dataModel_->getSample().getSize()));
}
}
