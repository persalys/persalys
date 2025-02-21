//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the data model
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
#include "persalys/DataFieldModelWindow.hxx"

#include "persalys/ComboBoxDelegate.hxx"
#include "persalys/FileTools.hxx"
#include "persalys/LineEditWithQValidatorDelegate.hxx"
#include "persalys/CheckableHeaderView.hxx"
#include "persalys/SampleTableModel.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/ImportedMeshModel.hxx"
#include "persalys/DataCleaningWizard.hxx"
#include "persalys/GridMeshModel.hxx"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QToolButton>
//#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QScrollArea>
#include <QRadioButton>

using namespace OT;

namespace PERSALYS
{

DataFieldModelWindow::DataFieldModelWindow(DataFieldModelItem * item, QWidget * parent)
  : SubWindow(item, parent)
{
  dataModel_ = item->getDataFieldModel();
  connect(item, SIGNAL(meshChanged()), this, SLOT(updateProcessSample()));
  buildInterface();
}


void DataFieldModelWindow::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  layout()->invalidate();
  adjustSize();
}

void DataFieldModelWindow::buildInterface()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(new TitleLabel(tr("Data Field model"), "user_manual/graphical_interface/data_model/user_manual_data_model.html#datafieldmodel"));

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  mainLayout->addWidget(scrollArea, 1);

  QWidget * mainWidget = new QWidget;
  QGridLayout * mainGridLayout = new QGridLayout(mainWidget);
  scrollArea->setWidget(mainWidget);

  //QGroupBox * groupBox = new QGroupBox(tr("Fields"));
  //QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);

  // file line edit
  QHBoxLayout * hboxLayout = new QHBoxLayout;
  QLabel * label = new QLabel(tr("Data file"));
  hboxLayout->addWidget(label);

  filePathLineEdit_ = new QLineEdit;
  hboxLayout->addWidget(filePathLineEdit_);

  QToolButton * openFileButton = new QToolButton;
  openFileButton->setText("...");
  openFileButton->setToolTip(tr("Search file"));
  connect(openFileButton, SIGNAL(clicked()), this, SLOT(openFileRequested()));
  hboxLayout->addWidget(openFileButton);

  int row = 0;
  mainGridLayout->addLayout(hboxLayout, row++, 0, 1, 3);

  // column/row radio button
  QHBoxLayout * buttonsLayout = new QHBoxLayout;
  orderButtonGroup_ = new QButtonGroup(this);
  QRadioButton * orderButton = new QRadioButton(tr("Columns"));
  orderButtonGroup_->addButton(orderButton, Tools::Columns);
  buttonsLayout->addWidget(orderButton);

  orderButton = new QRadioButton(tr("Rows"));
  orderButtonGroup_->addButton(orderButton, Tools::Rows);
  orderButtonGroup_->button(Tools::Columns)->click();
  orderButtonGroup_->button(0)->setEnabled(QFile(filePathLineEdit_->text()).exists());
  orderButtonGroup_->button(1)->setEnabled(QFile(filePathLineEdit_->text()).exists());
  buttonsLayout->addWidget(orderButton);
  buttonsLayout->addStretch();

  mainGridLayout->addLayout(buttonsLayout, row++, 0, 1, 3);

  connect(orderButtonGroup_, &QButtonGroup::idClicked, [=] (int) {
    setTable(filePathLineEdit_->text());
  });

  connect(filePathLineEdit_, &QLineEdit::textChanged, [=] (QString) {
    orderButtonGroup_->button(0)->setEnabled(QFile(filePathLineEdit_->text()).exists());
    orderButtonGroup_->button(1)->setEnabled(QFile(filePathLineEdit_->text()).exists());
  });

  tableModel_ = new SampleTableModel(dataModel_.getProcessSampleAsSample(), true, this);
  QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(this);
  proxyModel->setSourceModel(tableModel_);
  proxyModel->setSortRole(Qt::UserRole);

  QFrame * frame = new QFrame;
  QHBoxLayout * tableLayout = new QHBoxLayout(frame);
  tableLayout->setContentsMargins(0, 0, 0, 0);

  dataTableView_ = new EditableExportableTableView;
  dataTableView_->setObjectName("sampleTable");
  dataTableView_->setModel(proxyModel);
  dataTableView_->verticalHeader()->hide();
  tableLayout->addWidget(dataTableView_);

  mainGridLayout->addWidget(frame, row++, 0, 1, 3);

  // connections
  connect(tableModel_, SIGNAL(sampleChanged()), this, SLOT(updateProcessSample()));
  connect(dataTableView_, SIGNAL(cleanRequested()), this, SLOT(launchCleaningWizard()));

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  mainGridLayout->addWidget(errorMessageLabel_, ++row, 0, 1, 3);

}

void DataFieldModelWindow::launchCleaningWizard()
{
  errorMessageLabel_->reset();
  if(!tableModel_->getSample().getSize())
  {
    errorMessageLabel_->setText(tr("Sample must not be empty"));
    return;
  }
  DataCleaning* cleaner = new DataCleaning(tableModel_->getSample());
  DataCleaningWizard wizard(cleaner, this);
  tableModel_->updateData(cleaner->getSample());
  updateProcessSample();
}


void DataFieldModelWindow::setTable(const QString& fileName)
{
  // re-initialization
  errorMessageLabel_->reset();
  // try to retrieve data from the selected file
  try
  {
    // update file name
    filePathLineEdit_->setText(fileName);
    tableModel_->updateData(Tools::ImportSample(fileName.toStdString(), static_cast<Tools::DataOrder>(orderButtonGroup_->checkedId())));
  }
  catch (std::exception& ex)
  {
    QMessageBox::warning(this,
                         tr("Warning"),
                         tr("Cannot update the table.\n%1").arg(ex.what()));
  }
  updateProcessSample();
}


void DataFieldModelWindow::updateProcessSample()
{
  errorMessageLabel_->reset();
  dataModel_.setSampleAsProcessSample(tableModel_->getSample());
  // Override mesh model if incompatible
  if (tableModel_->getSample().getDimension() != dataModel_.getMeshModel().getMesh().getVerticesNumber())
  {
    if (dynamic_cast<GridMeshModel*>(dataModel_.getMeshModel().getImplementation().get()))//ClassName() == "GridMeshModel")
      dataModel_.setMeshModel(GridMeshModel(Interval(0, 1), Indices(1, tableModel_->getSample().getDimension())));
    else
      errorMessageLabel_->setErrorMessage(tr("Mesh vertices number (")
                                          + QString::number(dataModel_.getMeshModel().getMesh().getVerticesNumber())
                                          + ") must match field discretization ("
                                          + QString::number(tableModel_->getSample().getDimension()) + QString(")."));
  }
  else if (!dataModel_.isValid())
    errorMessageLabel_->setErrorMessage(tr("The model is not valid. Check data and/or mesh numerical validity."));
}

void DataFieldModelWindow::openFileRequested()
{
  const QString fileName = QFileDialog::getOpenFileName(this,
                           tr("Data to import..."),
                           FileTools::GetCurrentDir(),
                           tr("Data files (*.csv *.txt *.dat)"));

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
      setTable(fileName);
    }
  }
}

}
