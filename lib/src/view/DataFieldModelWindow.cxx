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
#include "persalys/TranslationManager.hxx"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QToolButton>
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
  mainLayout->addWidget(new TitleLabel(tr("Field data set"), 
  "user_manual/graphical_interface/data_model/user_manual_data_model.html#datafieldmodel"));

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  mainLayout->addWidget(scrollArea, 1);

  QWidget * mainWidget = new QWidget;
  QGridLayout * mainGridLayout = new QGridLayout(mainWidget);
  scrollArea->setWidget(mainWidget);

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
  QRadioButton * orderButton = new QRadioButton(tr("One column in the CSV file\ncorresponds to one time step\nand one row to one variable"));
  orderButtonGroup_->addButton(orderButton, Tools::Columns);
  buttonsLayout->addWidget(orderButton);

  bool fileExists = QFile(filePathLineEdit_->text()).exists();
  orderButton = new QRadioButton(tr("One row in the CSV file\ncorresponds to one time step\nand one column to one variable"));
  orderButtonGroup_->addButton(orderButton, Tools::Rows);
  orderButtonGroup_->button(Tools::Columns)->click();
  orderButtonGroup_->button(0)->setEnabled(fileExists);
  orderButtonGroup_->button(1)->setEnabled(fileExists);
  buttonsLayout->addWidget(orderButton);
  buttonsLayout->addStretch();

  mainGridLayout->addLayout(buttonsLayout, row++, 0, 1, 3);

  connect(orderButtonGroup_, &QButtonGroup::idClicked, [this] (int) {
    setTable(filePathLineEdit_->text());
  });

  connect(filePathLineEdit_, &QLineEdit::textChanged, [this] (QString) {
    bool exists = QFile(filePathLineEdit_->text()).exists();
    orderButtonGroup_->button(0)->setEnabled(exists);
    orderButtonGroup_->button(1)->setEnabled(exists);
  });

  includeMeshCB_ = new QCheckBox(tr("Data contains mesh as first row/column"));
  includeMeshCB_->setEnabled(fileExists);
  mainGridLayout->addWidget(includeMeshCB_, row++, 0);

  connect(filePathLineEdit_, &QLineEdit::textChanged, [this] (QString) {
    includeMeshCB_->setEnabled(QFile(filePathLineEdit_->text()).exists());
  });

  connect(includeMeshCB_, &QCheckBox::clicked, [this] () {
    forceUpdateProcessSample_ = !includeMeshCB_->isChecked();
    setTable(filePathLineEdit_->text());
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
  errorWidget_ = new ErrorWidget;
  mainGridLayout->addWidget(errorWidget_, ++row, 0, 1, 3);

}

void DataFieldModelWindow::launchCleaningWizard()
{
  errorWidget_->reset();
  if(!tableModel_->getSample().getSize())
  {
    errorWidget_->setFramelessErrorMessage(tr("Sample must not be empty"));
    return;
  }
  DataCleaning* cleaner = new DataCleaning(tableModel_->getSample());
  DataCleaningWizard wizard(cleaner, this);
  tableModel_->updateData(cleaner->getSample());
  updateProcessSample();
}


void DataFieldModelWindow::setTable(const QString& fileName)
{
  filePathLineEdit_->setText(fileName);

  try
  {
    auto order = static_cast<Tools::DataOrder>(orderButtonGroup_->checkedId());
    Sample fileSample{
      Tools::ImportSample(fileName.toStdString(), order)
    };

    if(! includeMeshCB_->isChecked())
      tableModel_->updateData(fileSample);
    else
    {
      const Sample fileData = fileSample.split(1);
      tableModel_->updateData(fileData);

      // order for data and mesh is inverted
      order = order == Tools::DataOrder::Columns ? Tools::DataOrder::Rows : Tools::DataOrder::Columns;
      MeshModel newMesh{ImportedMeshModel(
                          fileName.toStdString(), 
                          dataModel_.getMeshModel().getIndexParameters(),
                          Indices(1, 0), 
                          order)};
      dataModel_.setMeshModel(newMesh);
    }
  }
  catch (const InvalidArgumentException &e)
  {
    errorWidget_->setFramelessErrorMessage(TranslationManager::GetTranslatedErrorMessage(e.what()));
  }

  updateProcessSample();
}


void DataFieldModelWindow::updateProcessSample()
{
  errorWidget_->reset();

  // The process sample must not be empty
  if (tableModel_->getSample().getSize())
  {
    dataModel_.setSampleAsProcessSample(tableModel_->getSample());
    // Override mesh model if incompatible
    if ((tableModel_->getSample().getDimension() != dataModel_.getMeshModel().getMesh().getVerticesNumber())
        || forceUpdateProcessSample_)
    {
      forceUpdateProcessSample_ = false;
      try
      {
        dataModel_.blockNotification();
        dataModel_.setMeshModel(GridMeshModel(Interval(0, 1), Indices(1, tableModel_->getSample().getDimension())));
        dataModel_.getImplementation().get()->notify("meshOverwritten");
      }
      catch (const InvalidArgumentException &e)
      {
        errorWidget_->setFramelessErrorMessage(TranslationManager::GetTranslatedErrorMessage(e.what()));
      }
    }
  }
  if (!dataModel_.isValid() && errorWidget_->toPlainText().isEmpty())
    errorWidget_->setFramelessErrorMessage(tr("The model is not valid. Check data and/or mesh numerical validity."));
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
