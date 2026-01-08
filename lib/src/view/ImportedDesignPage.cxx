//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to import sample to define designs of experiments
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
  , designOfExperiment_()
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

  typeCombo_->addItem(tr("Monte-Carlo"), static_cast<int>(ImportedDesignOfExperiment::MC));
  typeCombo_->addItem(tr("Quasi-Monte-Carlo"), static_cast<int>(ImportedDesignOfExperiment::QMC));
  typeCombo_->addItem(tr("LHS"), static_cast<int>(ImportedDesignOfExperiment::LHS));
  typeCombo_->addItem(tr("Grid"), static_cast<int>(ImportedDesignOfExperiment::GRID));

  connect(typeCombo_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
    if (index < 0) return;
    const auto type = static_cast<ImportedDesignOfExperiment::Type>(typeCombo_->itemData(index).toInt());
    designOfExperiment_.setType(type);
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

  selectType(designOfExperiment_.getType());
  checkColumns();
}


void ImportedDesignPage::setTable(const QString& fileName)
{
  // set file name
  designOfExperiment_.setFileName(fileName.toUtf8().data());

  // update widgets
  estimatedTimeValueLabel_->setText(
    QString::number(
      (double) designOfExperiment_.getSampleFromFile().getSize()
      * designOfExperiment_.getPhysicalModel().getEvalTime()
    )
  );

  if(estimatedTimeValueLabel_->text().toFloat() > 1e-6)
  {
    estimatedTimeValueLabel_->setVisible(true);
    emit showTime();
  }

  Description variableDescription{designOfExperiment_.getPhysicalModel().getInputNames()};
  variableDescription.add(designOfExperiment_.getPhysicalModel().getSelectedOutputsNames());

  Indices variableColumns{designOfExperiment_.getInputColumns()};
  variableColumns.add(designOfExperiment_.getOutputColumns());
  
  sampleWidget_->updateWidgets(designOfExperiment_.getSampleFromFile(),
                               variableDescription,
                               variableColumns);
}


void ImportedDesignPage::checkColumns()
{
  const Description inputNames{designOfExperiment_.getPhysicalModel().getInputNames()};
  const Description outputNames{designOfExperiment_.getPhysicalModel().getSelectedOutputsNames()};

  // try to update the design of experiments
  try
  {
    designOfExperiment_.setColumns(sampleWidget_->getColumns(inputNames), sampleWidget_->getColumns(outputNames));
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
    designOfExperiment_ = *analysis_ptr;
    sampleWidget_->setData(QString::fromUtf8(designOfExperiment_.getFileName().c_str()));
  }
  else
  {
    // create a new analysis
    PhysicalModel physicalModel = dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get())->getPhysicalModel();
    designOfExperiment_ = ImportedDesignOfExperiment(analysis.getName(), physicalModel);
  }
  selectType(designOfExperiment_.getType());
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
  return designOfExperiment_;
}


bool ImportedDesignPage::validatePage()
{
  return sampleWidget_->tableValidity_;
}
}
