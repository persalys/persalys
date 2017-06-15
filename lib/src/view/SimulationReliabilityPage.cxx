//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define simulation reliability analysis
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/SimulationReliabilityPage.hxx"

#include "otgui/MonteCarloReliabilityAnalysis.hxx"
#include "otgui/FORMImportanceSamplingAnalysis.hxx"
#include "otgui/CollapsibleGroupBox.hxx"
#include "otgui/ReliabilityAnalysisWizard.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI {

SimulationReliabilityPage::SimulationReliabilityPage(QWidget* parent)
  : QWizardPage(parent)
  , method_(SimulationReliabilityPage::MonteCarlo)
  , stopCriteriaGroupBox_(0)
  , blockSizeGroupBox_(0)
  , seedSpinbox_(0)
  , errorMessageLabel_(0)
{
  buildInterface();
}


void SimulationReliabilityPage::buildInterface()
{
  setTitle(tr("Simulation methods"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  /// simulation widgets

  // stop criteria
  stopCriteriaGroupBox_ = new StopCriteriaGroupBox;
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCoefficientOfVariationChanged(double)), this, SLOT(clearErrorMessageLabel()));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiTimeChanged(int)), this, SLOT(clearErrorMessageLabel()));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCallsChanged(double)), this, SLOT(clearErrorMessageLabel()));
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

  pageLayout->addWidget(advancedParamGroupBox);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);

  initialize(MonteCarloReliabilityAnalysis());
}


void SimulationReliabilityPage::initialize(const Analysis& analysis)
{
  const SimulationReliabilityAnalysis * analysis_ptr = dynamic_cast<const SimulationReliabilityAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  if (dynamic_cast<const MonteCarloReliabilityAnalysis*>(analysis_ptr))
    method_ = SimulationReliabilityPage::MonteCarlo;
  else
    method_ = SimulationReliabilityPage::FORM_IS;

  stopCriteriaGroupBox_->setMaximumCoefficientOfVariation(analysis_ptr->getMaximumCoefficientOfVariation());
  stopCriteriaGroupBox_->setMaximumElapsedTime(analysis_ptr->getMaximumElapsedTime());
  stopCriteriaGroupBox_->setMaximumCalls(analysis_ptr->getMaximumCalls());

  blockSizeGroupBox_->setBlockSizeValue(analysis_ptr->getBlockSize());

  seedSpinbox_->setValue(analysis_ptr->getSeed());
}


Analysis SimulationReliabilityPage::getAnalysis(const String& name, const LimitState& limitState) const
{
  if (method_ == SimulationReliabilityPage::MonteCarlo)
  {
    MonteCarloReliabilityAnalysis analysis(name, limitState);
    analysis.setMaximumCalls(stopCriteriaGroupBox_->getMaximumCalls());
    analysis.setMaximumCoefficientOfVariation(stopCriteriaGroupBox_->getMaximumCoefficientOfVariation());
    analysis.setMaximumElapsedTime(stopCriteriaGroupBox_->getMaximumElapsedTime());
    analysis.setBlockSize(blockSizeGroupBox_->getBlockSizeValue());
    analysis.setSeed(seedSpinbox_->value());
    return analysis;
  }
  else
  {
    FORMImportanceSamplingAnalysis analysis(name, limitState);
    analysis.setMaximumCalls(stopCriteriaGroupBox_->getMaximumCalls());
    analysis.setMaximumCoefficientOfVariation(stopCriteriaGroupBox_->getMaximumCoefficientOfVariation());
    analysis.setMaximumElapsedTime(stopCriteriaGroupBox_->getMaximumElapsedTime());
    analysis.setBlockSize(blockSizeGroupBox_->getBlockSizeValue());
    analysis.setSeed(seedSpinbox_->value());
    return analysis;
  }
}


void SimulationReliabilityPage::updateMethod(int id)
{
  method_ = Method(id);
  setFinalPage(id == SimulationReliabilityPage::MonteCarlo);
}


int SimulationReliabilityPage::nextId() const
{
  if (method_ == SimulationReliabilityPage::MonteCarlo)
    return -1;
  else
    return ReliabilityAnalysisWizard::Page_FORM;
}


void SimulationReliabilityPage::clearErrorMessageLabel()
{
  // the slot clear() of QLabel does not work...
  errorMessageLabel_->setText("");
}


bool SimulationReliabilityPage::validatePage()
{
  QString errorMessage;

  if (!stopCriteriaGroupBox_->isValid())
    errorMessage = tr("Please select at least one stop criteria");
  else
  {
    if (!stopCriteriaGroupBox_->isMaxElapsedTimeValid())
      errorMessage = tr("The maximum time must not be null");

    if (stopCriteriaGroupBox_->isMaxCallsRequired())
    {
      if (stopCriteriaGroupBox_->getMaximumCalls() < blockSizeGroupBox_->getBlockSizeValue())
        errorMessage = tr("The maximum calls can not be inferior to the block size");
    }
  }

  errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(errorMessage));
  if (!errorMessage.isEmpty())
    return false;

  return true;
}
}