//                                               -*- C++ -*-
/**
 *  @brief QWizard to import sample to define Observations
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#include "persalys/ObservationsWizard.hxx"

#include "persalys/QtTools.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace PERSALYS
{
ImportObservationsPage::ImportObservationsPage(const PhysicalModel& physicalModel, const String& obsName, QWidget* parent)
  : QWizardPage(parent)
  , sampleWidget_(new ImportSampleWidget(this))
  , physicalModel_(physicalModel)
  , obsName_(obsName)
{
  buildInterface();
}


void ImportObservationsPage::buildInterface()
{
  setTitle(tr("Define observations"));
  setSubTitle(tr("Import data from a file"));

  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(sampleWidget_);
  connect(sampleWidget_, SIGNAL(updateTableRequested(QString)), this, SLOT(setTable(QString)));
  connect(sampleWidget_, SIGNAL(checkColumnsRequested()), this, SLOT(checkColumns()));
}


void ImportObservationsPage::setTable(const QString& fileName)
{
  const String fname = fileName.toUtf8().data();

  // Keep the file path line edit in sync (e.g. when called directly in tests)
  sampleWidget_->filePathLineEdit_->setText(fileName);

  // Load the file with default column assignments
  ImportedDataset dataset;
  dataset.setFileName(fname);
  fileName_ = fname;

  // All physical model variable names (for comboboxes)
  Description allVarNames(physicalModel_.getInputNames());
  allVarNames.add(physicalModel_.getOutputNames());

  // Initial variable names and column indices from file headers
  const Sample sampleFromFile = dataset.getSampleFromFile();
  const Indices inputCols(dataset.getInputColumns());
  const Indices outputCols(dataset.getOutputColumns());
  Description initVarNames = sampleFromFile.getMarginal(inputCols).getDescription();
  initVarNames.add(sampleFromFile.getMarginal(outputCols).getDescription());
  Indices columns(inputCols);
  columns.add(outputCols);

  sampleWidget_->updateWidgets(sampleFromFile, initVarNames, columns, allVarNames);
}


void ImportObservationsPage::checkColumns()
{
  const Description inputNames(physicalModel_.getInputNames());
  const Description outputNames(physicalModel_.getOutputNames());

  Indices inColumns;
  Indices outColumns;
  try
  {
    inColumns = sampleWidget_->getColumns(inputNames);
    outColumns = sampleWidget_->getColumns(outputNames);
    sampleWidget_->tableValidity_ = true;
    sampleWidget_->errorWidget_->reset();
  }
  catch (const InvalidArgumentException &)
  {
    sampleWidget_->errorWidget_->setFramelessErrorMessage(tr("Each variable must be associated with one column."));
    sampleWidget_->tableValidity_ = false;
    return;
  }

  QStringList inNames;
  QStringList outNames;
  for (UnsignedInteger i = 0; i < inColumns.getSize(); ++i)
    inNames << sampleWidget_->dataPreviewTableView_->model()->headerData(inColumns[i], Qt::Horizontal).toString();
  for (UnsignedInteger i = 0; i < outColumns.getSize(); ++i)
    outNames << sampleWidget_->dataPreviewTableView_->model()->headerData(outColumns[i], Qt::Horizontal).toString();

  if (inNames.size() == (int)inputNames.getSize())
  {
    sampleWidget_->errorWidget_->setFramelessErrorMessage(tr("All the input variables can not be observed. At least an input variable must be calibrated."));
    sampleWidget_->tableValidity_ = false;
    return;
  }

  // Validate: at least one output, and no column assigned to both input and output
  Indices allCols(inColumns);
  allCols.add(outColumns);
  const int ncols = sampleWidget_->dataPreviewTableView_->model()->columnCount();
  if (outColumns.getSize() == 0 || !allCols.check(ncols))
  {
    sampleWidget_->errorWidget_->setFramelessErrorMessage(tr("Define observations for at least an output variable and an input variable. A variable must be associated with only one column."));
    sampleWidget_->tableValidity_ = false;
    return;
  }

  // Store validated state
  inColumns_ = inColumns;
  outColumns_ = outColumns;
  inNames_ = QtOT::StringListToDescription(inNames);
  outNames_ = QtOT::StringListToDescription(outNames);
  sampleWidget_->tableValidity_ = true;
  sampleWidget_->errorWidget_->reset();
}


DesignOfExperiment ImportObservationsPage::getDesignOfExperiment() const
{
  return Observations(obsName_, physicalModel_, fileName_, inColumns_, outColumns_, inNames_, outNames_);
}


bool ImportObservationsPage::validatePage()
{
  return sampleWidget_->tableValidity_;
}
}
