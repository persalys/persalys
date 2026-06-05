//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to import sample to define designs of experiments
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
#include "persalys/ImportedDesignPage.hxx"

#include "persalys/QtTools.hxx"

#include <QVBoxLayout>
#include <QComboBox>

using namespace OT;

namespace PERSALYS
{

ImportedDesignPage::ImportedDesignPage(QWidget* parent)
  : QWizardPage(parent)
  , sampleWidget_(new ImportSampleWidget(this))
  , importedDoE_()
{
  buildInterface();
}


void ImportedDesignPage::buildInterface()
{
  setTitle(tr("Imported design"));
  setSubTitle(tr("Import data from a file"));

  auto * mainLayout         = new QVBoxLayout(this);
  auto * estimatedTimeLabel = new QLabel(tr("Estimated duration (s): "));
  estimatedTimeValueLabel_  = new QLabel;
  auto * timeWidget         = new QWidget;
  auto * timeLayout         = new QHBoxLayout(timeWidget);

  timeLayout->addWidget(estimatedTimeLabel, 0);
  timeLayout->addWidget(estimatedTimeValueLabel_, 1);

  auto * typeLayout = new QHBoxLayout;
  auto * typeLabel  = new QLabel(tr("Design type:"));
  typeCombo_        = new QComboBox;

  typeCombo_->addItem(tr("Generic"), static_cast<int>(ImportedDesignOfExperiment::Type::UK));
  typeCombo_->addItem(tr("Monte-Carlo"), static_cast<int>(ImportedDesignOfExperiment::Type::MC));
  typeCombo_->addItem(tr("Quasi-Monte-Carlo"), static_cast<int>(ImportedDesignOfExperiment::Type::QMC));
  typeCombo_->addItem(tr("LHS"), static_cast<int>(ImportedDesignOfExperiment::Type::RLHS));
  typeCombo_->addItem(tr("Optimized LHS"), static_cast<int>(ImportedDesignOfExperiment::Type::OLHS));
  typeCombo_->addItem(tr("Grid"), static_cast<int>(ImportedDesignOfExperiment::Type::GRID));
  typeCombo_->addItem((tr("Morris")), static_cast<int>(ImportedDesignOfExperiment::Type::MORRIS));

  connect(typeCombo_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
    if (index < 0) return;
    const auto type = static_cast<ImportedDesignOfExperiment::Type>(typeCombo_->itemData(index).toInt());
    importedDoE_.setType(type);
  });

  typeLayout->addWidget(typeLabel);
  typeLayout->addWidget(typeCombo_);

  mainLayout->addWidget(timeWidget);
  mainLayout->addWidget(sampleWidget_);
  mainLayout->addLayout(typeLayout);

  connect(sampleWidget_, &ImportSampleWidget::updateTableRequested, this, &ImportedDesignPage::setTable);
  connect(sampleWidget_, &ImportSampleWidget::checkColumnsRequested, this, &ImportedDesignPage::checkColumns);

  estimatedTimeValueLabel_->setVisible(estimatedTimeValueLabel_->text().toFloat() > 1e-6);
  estimatedTimeLabel->setVisible(estimatedTimeValueLabel_->text().toFloat() > 1e-6);

  connect(this, &ImportedDesignPage::showTime, estimatedTimeValueLabel_, &QLabel::show);

  selectType(importedDoE_.getType());
  checkColumns();
}


void ImportedDesignPage::setTable(const QString& fileName)
{
  // set file name
  importedDoE_.setFileName(fileName.toUtf8().data());

  // update widgets
  estimatedTimeValueLabel_->setText(
    QString::number(
      (double) importedDoE_.getImportedDataset().getSampleFromFile().getSize()
      * importedDoE_.getPhysicalModel().getEvalTime()
    )
  );

  if(estimatedTimeValueLabel_->text().toFloat() > 1e-6)
  {
    estimatedTimeValueLabel_->setVisible(true);
    emit showTime();
  }

  const UnsignedInteger fileDim   = importedDoE_.getImportedDataset().getSampleFromFile().getDimension();
  const Description inputNames    = importedDoE_.getPhysicalModel().getInputNames();
  const Description outputNames   = importedDoE_.getPhysicalModel().getSelectedOutputsNames();
  const UnsignedInteger inputDim  = inputNames.getSize();
  const UnsignedInteger outputDim = outputNames.getSize();

  const Indices savedInputCols  = importedDoE_.getImportedDataset().getInputColumns();
  const Indices savedOutputCols = importedDoE_.getImportedDataset().getOutputColumns();

  // Use saved column mappings when they are consistent with the physical model and
  // the file (e.g. reopening an existing DoE). Fall back to the positional heuristic
  // when the file is new or the saved mapping does not match the model dimensions.
  //
  // A mapping is "valid" when:
  //   - input column count matches the model's input dimension and all indices fit
  //     within the file;
  //   - output columns are either absent (outputs are optional for DoE) OR their
  //     count matches the model's selected-output dimension and all indices fit.
  const bool hasValidMappings =
      savedInputCols.getSize() == inputDim &&
      savedInputCols.check(fileDim) &&
      (savedOutputCols.isEmpty() ||
       (savedOutputCols.getSize() == outputDim && savedOutputCols.check(fileDim)));

  Description variableDescription;
  Indices variableColumns;

  if (hasValidMappings)
  {
    // Preserve the saved assignment (non-consecutive column order is possible).
    variableDescription.add(inputNames);
    variableColumns.add(savedInputCols);
    if (!savedOutputCols.isEmpty())
    {
      variableDescription.add(outputNames);
      variableColumns.add(savedOutputCols);
    }
  }
  else
  {
    // No valid saved mapping: assign the first min(inputDim, fileDim) columns to
    // inputs, then the next outputDim columns to outputs — if the file is wide enough.
    const UnsignedInteger nInputCols = inputDim < fileDim ? inputDim : fileDim;
    variableDescription = inputNames;
    for (UnsignedInteger i = 0; i < nInputCols; ++i)
      variableColumns.add(i);
    if (outputDim > 0 && fileDim >= nInputCols + outputDim)
    {
      variableDescription.add(outputNames);
      for (UnsignedInteger i = 0; i < outputDim; ++i)
        variableColumns.add(nInputCols + i);
    }
  }
  
  sampleWidget_->updateWidgets(importedDoE_.getImportedDataset().getSampleFromFile(),
                               variableDescription,
                               variableColumns);
}


void ImportedDesignPage::checkColumns()
{
  const Description inputNames{importedDoE_.getPhysicalModel().getInputNames()};
  const Description outputNames{importedDoE_.getPhysicalModel().getSelectedOutputsNames()};

  // try to update the design of experiments
  try
  {
    importedDoE_.setColumns(sampleWidget_->getColumns(inputNames), sampleWidget_->getColumns(outputNames));
    sampleWidget_->tableValidity_ = true;
    sampleWidget_->errorWidget_->reset();
  }
  catch (const InvalidArgumentException &)
  {
    sampleWidget_->errorWidget_->setFramelessErrorMessage(tr("Each variable must be associated with one column."));
    sampleWidget_->tableValidity_ = false;
  }
}


void ImportedDesignPage::initialize(const Analysis& analysis)
{
  const auto * analysis_ptr = dynamic_cast<const ImportedDesignOfExperiment*>(analysis.getImplementation().get());

  // if already an ImportedDesignOfExperiment
  if (analysis_ptr)
  {
    importedDoE_ = *analysis_ptr;
    sampleWidget_->setData(QString::fromUtf8(importedDoE_.getImportedDataset().getFileName().c_str()));
  }
  else
  {
    // create a new analysis
    PhysicalModel physicalModel = dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get())->getPhysicalModel();
    importedDoE_ = ImportedDesignOfExperiment(analysis.getName(), physicalModel);
  }
  selectType(importedDoE_.getType());
}


void ImportedDesignPage::selectType(ImportedDesignOfExperiment::Type type)
{
  if (!typeCombo_) return;
  const int index = typeCombo_->findData(static_cast<int>(type));
  if (index >= 0)
    typeCombo_->setCurrentIndex(index);
}


Analysis ImportedDesignPage::getAnalysis() const
{
  return importedDoE_;
}


bool ImportedDesignPage::validatePage()
{
  if (importedDoE_.getImportedDataset().getFileName().empty())
  {
    sampleWidget_->errorWidget_->setFramelessErrorMessage(tr("Please select a data file."));
    return false;
  }
  return sampleWidget_->tableValidity_;
}
}
