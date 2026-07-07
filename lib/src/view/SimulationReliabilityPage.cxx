//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define simulation reliability analysis
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
#include "persalys/SimulationReliabilityPage.hxx"

#include "persalys/MonteCarloReliabilityAnalysis.hxx"
#include "persalys/CollapsibleGroupBox.hxx"

#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>

using namespace OT;

namespace PERSALYS
{

SimulationReliabilityPage::SimulationReliabilityPage(QWidget* parent)
  : QWizardPage(parent)
{
  buildInterface();
}


void SimulationReliabilityPage::buildInterface()
{
  setTitle(tr("Simulation methods"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  /// simulation widgets

  // Stopping criteria
  stopCriteriaGroupBox_ = new StopCriteriaGroupBox;
  pageLayout->addWidget(stopCriteriaGroupBox_);

  // block size
  blockSizeGroupBox_ = new BlockSizeGroupBox(tr("Evaluation parameter"));
  pageLayout->addWidget(blockSizeGroupBox_);

  //// advanced parameters
  CollapsibleGroupBox * advancedParamGroupBox = new CollapsibleGroupBox;
  advancedParamGroupBox->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedWidgetsLayout = new QGridLayout(advancedParamGroupBox);

  // seed
  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedWidgetsLayout->addWidget(seedLabel, 1, 0);

  seedSpinbox_ = new QSpinBox;
  seedSpinbox_->setMaximum(std::numeric_limits<int>::max());
  seedLabel->setBuddy(seedSpinbox_);
  advancedWidgetsLayout->addWidget(seedSpinbox_, 1, 1);

  // individual event probabilities
  computeIndividualProbabilitiesCheckBox_ = new QCheckBox(tr("Compute individual event probabilities"));
  computeIndividualProbabilitiesCheckBox_->setChecked(false);
  advancedWidgetsLayout->addWidget(computeIndividualProbabilitiesCheckBox_, 2, 0, 1, 2);

  pageLayout->addWidget(advancedParamGroupBox);

  // error message
  errorWidget_ = new ErrorWidget;
  connect(stopCriteriaGroupBox_, SIGNAL(criteriaChanged()), errorWidget_, SLOT(reset()));
  connect(blockSizeGroupBox_, SIGNAL(blockSizeChanged(double)), errorWidget_, SLOT(reset()));

  pageLayout->addStretch();
  pageLayout->addWidget(errorWidget_);

  initialize(MonteCarloReliabilityAnalysis());
}


void SimulationReliabilityPage::initialize(const Analysis& analysis)
{
  const SimulationReliabilityAnalysis * analysis_ptr = dynamic_cast<const SimulationReliabilityAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  stopCriteriaGroupBox_->setMaximumCoefficientOfVariation(analysis_ptr->getMaximumCoefficientOfVariation());
  stopCriteriaGroupBox_->setMaximumElapsedTime(analysis_ptr->getMaximumElapsedTime());
  stopCriteriaGroupBox_->setMaximumCalls(analysis_ptr->getMaximumCalls());

  blockSizeGroupBox_->setBlockSizeValue(analysis_ptr->getBlockSize());

  seedSpinbox_->setValue(analysis_ptr->getSeed());
  computeIndividualProbabilitiesCheckBox_->setChecked(analysis_ptr->getComputeIndividualEventProbabilities());
}


void SimulationReliabilityPage::updateAnalysis(const Analysis& analysis)
{
  SimulationReliabilityAnalysis * analysis_ptr = dynamic_cast<SimulationReliabilityAnalysis*>(analysis.getImplementation().get());
  if (!analysis_ptr)
    return;
  analysis_ptr->setMaximumCalls(stopCriteriaGroupBox_->getMaximumCalls());
  analysis_ptr->setMaximumCoefficientOfVariation(stopCriteriaGroupBox_->getMaximumCoefficientOfVariation());
  analysis_ptr->setMaximumElapsedTime(stopCriteriaGroupBox_->getMaximumElapsedTime());
  analysis_ptr->setBlockSize(blockSizeGroupBox_->getBlockSizeValue());
  analysis_ptr->setSeed(seedSpinbox_->value());
  analysis_ptr->setComputeIndividualEventProbabilities(computeIndividualProbabilitiesCheckBox_->isChecked());
}


bool SimulationReliabilityPage::validatePage()
{
  QString errorMessage;

  if (!stopCriteriaGroupBox_->isValid())
    errorMessage = tr("Select at least one stopping criterion");
  else
  {
    if (!stopCriteriaGroupBox_->isMaxElapsedTimeValid())
      errorMessage = tr("The maximum time must not be null");
  }

  errorWidget_->setFramelessErrorMessage(errorMessage);
  if (!errorMessage.isEmpty())
    return false;

  return true;
}
}
