//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to import sample to define designs of experiments
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
#include "persalys/ImportedDesignPage.hxx"

#include "persalys/QtTools.hxx"

#include <QVBoxLayout>

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

  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  QLabel * estimatedTimeLabel = new QLabel(tr("Estimated duration (s): "));
  estimatedTimeValueLabel_ = new QLabel;
  QWidget * timeWidget = new QWidget;
  QHBoxLayout * timeLayout = new QHBoxLayout(timeWidget);
  timeLayout->addWidget(estimatedTimeLabel, 0);
  timeLayout->addWidget(estimatedTimeValueLabel_, 1);
  mainLayout->addWidget(timeWidget);
  mainLayout->addWidget(sampleWidget_);
  connect(sampleWidget_, SIGNAL(updateTableRequested(QString)), this, SLOT(setTable(QString)));
  connect(sampleWidget_, SIGNAL(checkColumnsRequested()), this, SLOT(checkColumns()));

  estimatedTimeValueLabel_->setVisible(estimatedTimeValueLabel_->text().toFloat()>1e-6);
  estimatedTimeLabel->setVisible(estimatedTimeValueLabel_->text().toFloat()>1e-6);
  connect(this, SIGNAL(showTime()), estimatedTimeLabel, SLOT(show()));
}


void ImportedDesignPage::setTable(const QString& fileName)
{
  // set file name
  designOfExperiment_.setFileName(fileName.toUtf8().data());

  // update widgets
  estimatedTimeValueLabel_->setText(QString::number(designOfExperiment_.getSampleFromFile().getSize()*designOfExperiment_.getPhysicalModel().getEvalTime()));
  if(estimatedTimeValueLabel_->text().toFloat()>1e-6) {
    estimatedTimeValueLabel_->setVisible(true);
    emit showTime();
  }
  sampleWidget_->updateWidgets(designOfExperiment_.getSampleFromFile(),
                               designOfExperiment_.getPhysicalModel().getInputNames(),
                               designOfExperiment_.getInputColumns());
}


void ImportedDesignPage::checkColumns()
{
  const Description inputNames = designOfExperiment_.getPhysicalModel().getInputNames();

  // try to update the design of experiments
  try
  {
    designOfExperiment_.setInputColumns(sampleWidget_->getColumns(inputNames));
    sampleWidget_->tableValidity_ = true;
    sampleWidget_->errorMessageLabel_->reset();
  }
  catch(InvalidArgumentException & ex)
  {
    sampleWidget_->errorMessageLabel_->setErrorMessage(tr("Each variable must be associated with one column."));
    sampleWidget_->tableValidity_ = false;
  }
}


void ImportedDesignPage::initialize(const Analysis& analysis)
{
  ImportedDesignOfExperiment * analysis_ptr = dynamic_cast<ImportedDesignOfExperiment*>(analysis.getImplementation().get());

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
}


Analysis ImportedDesignPage::getAnalysis()
{
  return designOfExperiment_;
}


bool ImportedDesignPage::validatePage()
{
  return sampleWidget_->tableValidity_;
}
}
