//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to import sample to define designs of experiments
 *
 *  Copyright 2015-2016 EDF
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/ImportTablePage.hxx"

#include "otgui/HorizontalHeaderViewWithCombobox.hxx"

#include <NumericalSample.hxx>

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>

using namespace OT;

namespace OTGUI {

ImportTablePage::ImportTablePage(const DesignOfExperiment & designOfExperiment, QWidget *parent)
  : QWizardPage(parent)
  , designOfExperiment_(designOfExperiment)
  , pageValidity_(false)
{
  buildInterface();
}

void ImportTablePage::buildInterface()
{
  setWindowTitle(tr("Import table from file"));

  QGridLayout * mainGridLayout = new QGridLayout(this);

  // first row
  QHBoxLayout * hboxLayout = new QHBoxLayout;
  QLabel * label = new QLabel(tr("File path:"));
  hboxLayout->addWidget(label);

  filePathLineEdit_ = new QLineEdit;
  filePathLineEdit_->setText(designOfExperiment_.getFileName().c_str());
  hboxLayout->addWidget(filePathLineEdit_);

  QPushButton * openFileButton = new QPushButton(tr("Open"));
  connect(openFileButton, SIGNAL(clicked()), this, SLOT(openFileRequested()));
  hboxLayout->addWidget(openFileButton);

  mainGridLayout->addLayout(hboxLayout, 0, 0, 1, 3);

  QGroupBox * groupBox = new QGroupBox(tr("File Preview"));
  QGridLayout * gridLayout = new QGridLayout(groupBox);
  gridLayout->setSpacing(6);
  gridLayout->setContentsMargins(11, 11, 11, 11);

  dataPreviewTableView_ = new OTguiTableView(groupBox);
  gridLayout->addWidget(dataPreviewTableView_, 0, 0, 1, 1);
  if (designOfExperiment_.getFileName().size() != 0)
  {
    try
    {
      loadFile();
      pageValidity_ = true;
    }
    catch (std::exception & ex)
    {
//       TODO set an error message
      pageValidity_ = false;
    }
  }
  
  mainGridLayout->addWidget(groupBox, 1, 0, 1, 1);
}


void ImportTablePage::loadFile()
{
  std::vector< String > separatorsList(3);
  separatorsList[0] = " ";
  separatorsList[1] = ",";
  separatorsList[2] = ";";
  NumericalSample sampleFromFile;
  for (UnsignedInteger i=0; i<separatorsList.size(); ++i)
  {
    // import sample from the file
    sampleFromFile = NumericalSample::ImportFromTextFile(designOfExperiment_.getFileName(), separatorsList[i]);
    if (sampleFromFile.getSize())
      break;
  }
  if (!sampleFromFile.getSize())
    throw InvalidArgumentException(HERE) << "In DesignOfExperimentImplementation: impossible to load sample";

  if (!designOfExperiment_.getColumns().check(sampleFromFile.getDimension()))
    throw InvalidArgumentException(HERE) << "In DesignOfExperimentImplementation: impossible to load sample marginals";

  Description desc = Description(sampleFromFile.getDimension());
  Description inputNames = designOfExperiment_.getPhysicalModel().getInputNames();
  Indices columns(designOfExperiment_.getColumns());
  if (!columns.getSize())
  {
    columns = Indices(inputNames.getSize());
    columns.fill();
  }
    
  for (UnsignedInteger i=0; i<columns.getSize(); ++i)
    desc[columns[i]] = inputNames[i];
  sampleFromFile.setDescription(desc);

  dataPreviewTableView_->setModel(new OTguiTableModel(sampleFromFile));
  connect(dataPreviewTableView_->model(), SIGNAL(headerDataChanged(Qt::Orientation,int,int)), this, SLOT(columnChanged(Qt::Orientation,int,int)));

  QStringList comboBoxItems;
  for (UnsignedInteger i=0; i<inputNames.getSize(); ++i)
    comboBoxItems << inputNames[i].c_str();
  comboBoxItems << "";

  QVector<int> columnsWithCombo(dataPreviewTableView_->model()->columnCount());
  for (int i=0; i<columnsWithCombo.size(); ++i)
    columnsWithCombo[i] = i;
  HorizontalHeaderViewWithCombobox * header = new HorizontalHeaderViewWithCombobox(comboBoxItems, columnsWithCombo, dataPreviewTableView_);
  dataPreviewTableView_->setHorizontalHeader(header);
  dataPreviewTableView_->horizontalHeader()->show();
}


void ImportTablePage::openFileRequested()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Data to import..."),
                     QDir::homePath(),
                     tr("Text Files (*.txt);;"));

  if (!fileName.isEmpty())
  {
    filePathLineEdit_->setText(fileName);
    try
    {
      designOfExperiment_.setFileName(filePathLineEdit_->text().toStdString());
      loadFile();
      pageValidity_ = true;
    }
    catch (std::exception & ex)
    {
//       TODO set an error message
      pageValidity_ = false;
    }
  }
}


void ImportTablePage::columnChanged(Qt::Orientation, int, int)
{
  Description inputNames = designOfExperiment_.getPhysicalModel().getInputNames();
  // test the unicity of each variable
  Indices columns;
  for (UnsignedInteger i=0; i<inputNames.getSize(); ++i)
    for (int j=0; j<dataPreviewTableView_->model()->columnCount(); ++j)
      if (inputNames[i] == dataPreviewTableView_->model()->headerData(j, Qt::Horizontal).toString().toStdString())
      {
        columns.add(j);
        break;
      }
  // test the presence of all variables
  Indices columns2;
  for (UnsignedInteger i=0; i<inputNames.getSize(); ++i)
    for (int j=0; j<dataPreviewTableView_->model()->columnCount(); ++j)
      if (inputNames[i] == dataPreviewTableView_->model()->headerData(j, Qt::Horizontal).toString().toStdString())
        columns2.add(j);

  if (columns != columns2)
    pageValidity_ = false;

  try
  {
    designOfExperiment_.setColumns(columns);
    pageValidity_ = true;
  }
  catch(InvalidArgumentException &)
  {
    pageValidity_ = false;
  }
}

bool ImportTablePage::validatePage()
{
  return pageValidity_;
}
}