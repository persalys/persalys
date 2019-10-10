//                                               -*- C++ -*-
/**
 *  @brief QWizard to import sample to define Observations
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
ImportObservationsPage::ImportObservationsPage(QWidget* parent)
  : QWizardPage(parent)
  , sampleWidget_(new ImportSampleWidget(this))
  , observations_()
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
  // set file name
  observations_.setFileName(fileName.toUtf8().data());
  // get variable names
  Description allVarNames(observations_.getPhysicalModel().getInputNames());
  allVarNames.add(observations_.getPhysicalModel().getOutputNames());
  Description initVarNames(observations_.getInputNames());
  initVarNames.add(observations_.getOutputNames());
  // get variable columns indices
  Indices columns(observations_.getInputColumns());
  columns.add(observations_.getOutputColumns());
  // update widgets
  sampleWidget_->updateWidgets(observations_.getSampleFromFile(), initVarNames, columns, allVarNames);
}


void ImportObservationsPage::checkColumns()
{
  const Description inputNames(observations_.getPhysicalModel().getInputNames());
  const Description outputNames(observations_.getPhysicalModel().getOutputNames());

  Indices inColumns(sampleWidget_->getColumns(inputNames));
  Indices outColumns(sampleWidget_->getColumns(outputNames));

  QStringList inNames;
  QStringList outNames;
  for (UnsignedInteger i = 0; i < inColumns.getSize(); ++i)
    inNames << sampleWidget_->dataPreviewTableView_->model()->headerData(inColumns[i], Qt::Horizontal).toString();
  for (UnsignedInteger i = 0; i < outColumns.getSize(); ++i)
    outNames << sampleWidget_->dataPreviewTableView_->model()->headerData(outColumns[i], Qt::Horizontal).toString();

  if (inNames.size() == (int)inputNames.getSize())
  {
    sampleWidget_->errorMessageLabel_->setErrorMessage(tr("All the input variables can not be observed. At least an input variable must be calibrated."));
    sampleWidget_->tableValidity_ = false;
    return;
  }
  // try to update the observations
  try
  {
    observations_.setColumns(inColumns, QtOT::StringListToDescription(inNames), outColumns, QtOT::StringListToDescription(outNames));
    sampleWidget_->tableValidity_ = true;
    sampleWidget_->errorMessageLabel_->reset();
  }
  catch (InvalidArgumentException & ex)
  {
    sampleWidget_->errorMessageLabel_->setErrorMessage(tr("Define observations for at least an output variable and an input variable. A variable must be associated with only one column."));
    sampleWidget_->tableValidity_ = false;
  }
}


void ImportObservationsPage::initialize(const DesignOfExperiment& designOfExp)
{
  Observations * obs_ptr = dynamic_cast<Observations*>(designOfExp.getImplementation().get());

  Q_ASSERT(obs_ptr);

  observations_ = *obs_ptr;
  if (!observations_.getFileName().empty())
    sampleWidget_->setData(QString::fromUtf8(observations_.getFileName().c_str()));
}


DesignOfExperiment ImportObservationsPage::getDesignOfExperiment()
{
  return observations_;
}


bool ImportObservationsPage::validatePage()
{
  return sampleWidget_->tableValidity_;
}
}
