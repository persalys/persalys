//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define monte carlo analysis
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
#include "otgui/MonteCarloPage.hxx"

#include "otgui/MonteCarloAnalysis.hxx"
#include "otgui/CollapsibleGroupBox.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI
{

MonteCarloPage::MonteCarloPage(QWidget* parent)
  : QWizardPage(parent)
  , stopCriteriaGroupBox_(0)
  , blockSizeGroupBox_(0)
  , seedSpinbox_(0)
  , errorMessageLabel_(0)
{
  buildInterface();
  setFinalPage(true);
}


void MonteCarloPage::buildInterface()
{
  setTitle(tr("Monte Carlo parameters"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  /// simulation widgets

  // stopping criteria
  stopCriteriaGroupBox_ = new StopCriteriaGroupBox;
  pageLayout->addWidget(stopCriteriaGroupBox_);

  // block size
  blockSizeGroupBox_ = new BlockSizeGroupBox(tr("Evaluation parameter"));
  pageLayout->addWidget(blockSizeGroupBox_);

  //// advanced parameters
  CollapsibleGroupBox * advancedParamGroupBox = new CollapsibleGroupBox;
  advancedParamGroupBox->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedWidgetsLayout = new QGridLayout(advancedParamGroupBox);

  // confidence interval
  confidenceIntervalCheckBox_ = new QCheckBox(tr("Compute confidence interval at"));
  advancedWidgetsLayout->addWidget(confidenceIntervalCheckBox_, 0, 0);

  // confidence interval level
  levelConfidenceIntervalSpinbox_ = new DoubleSpinBox;
  levelConfidenceIntervalSpinbox_->setRange(0.0, 0.99);
  levelConfidenceIntervalSpinbox_->setSingleStep(0.01);
  connect(confidenceIntervalCheckBox_, SIGNAL(toggled(bool)), levelConfidenceIntervalSpinbox_, SLOT(setEnabled(bool)));
  advancedWidgetsLayout->addWidget(levelConfidenceIntervalSpinbox_, 0, 1);

  // seed
  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedWidgetsLayout->addWidget(seedLabel, 1, 0);

  seedSpinbox_ = new QSpinBox;
  seedSpinbox_->setMaximum(std::numeric_limits<int>::max());
  seedLabel->setBuddy(seedSpinbox_);
  advancedWidgetsLayout->addWidget(seedSpinbox_, 1, 1);

  pageLayout->addWidget(advancedParamGroupBox);

  pageLayout->addStretch();

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  connect(stopCriteriaGroupBox_, SIGNAL(criteriaChanged()), errorMessageLabel_, SLOT(reset()));
  connect(blockSizeGroupBox_, SIGNAL(blockSizeChanged(double)), errorMessageLabel_, SLOT(reset()));

  pageLayout->addWidget(errorMessageLabel_);

  initialize(MonteCarloAnalysis());
}


void MonteCarloPage::initialize(const Analysis& analysis)
{
  const MonteCarloAnalysis * analysis_ptr = dynamic_cast<const MonteCarloAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  stopCriteriaGroupBox_->setMaximumCoefficientOfVariation(analysis_ptr->getMaximumCoefficientOfVariation());
  stopCriteriaGroupBox_->setMaximumElapsedTime(analysis_ptr->getMaximumElapsedTime());
  stopCriteriaGroupBox_->setMaximumCalls(analysis_ptr->getMaximumCalls());
  blockSizeGroupBox_->setBlockSizeValue(analysis_ptr->getBlockSize());
  confidenceIntervalCheckBox_->setChecked(analysis_ptr->isConfidenceIntervalRequired());
  levelConfidenceIntervalSpinbox_->setValue(analysis_ptr->getLevelConfidenceInterval());
  levelConfidenceIntervalSpinbox_->setEnabled(analysis_ptr->isConfidenceIntervalRequired());
  seedSpinbox_->setValue(analysis_ptr->getSeed());
}


Analysis MonteCarloPage::getAnalysis(const String& name, const PhysicalModel& physicalModel) const
{
  MonteCarloAnalysis analysis(name, physicalModel);
  analysis.setMaximumCalls(stopCriteriaGroupBox_->getMaximumCalls());
  analysis.setMaximumCoefficientOfVariation(stopCriteriaGroupBox_->getMaximumCoefficientOfVariation());
  analysis.setMaximumElapsedTime(stopCriteriaGroupBox_->getMaximumElapsedTime());
  analysis.setBlockSize(blockSizeGroupBox_->getBlockSizeValue());
  analysis.setSeed(seedSpinbox_->value());
  analysis.setIsConfidenceIntervalRequired(confidenceIntervalCheckBox_->isChecked());
  analysis.setLevelConfidenceInterval(levelConfidenceIntervalSpinbox_->value());

  return analysis;
}


int MonteCarloPage::nextId() const
{
  return -1;
}


bool MonteCarloPage::validatePage()
{
  QString errorMessage;

  if (!stopCriteriaGroupBox_->isValid())
    errorMessage = tr("Select at least one stopping criterion");
  else
  {
    if (!stopCriteriaGroupBox_->isMaxElapsedTimeValid())
      errorMessage = tr("The maximum time must not be null");

    if (stopCriteriaGroupBox_->isMaxCallsRequired())
    {
      if (stopCriteriaGroupBox_->getMaximumCalls() < blockSizeGroupBox_->getBlockSizeValue())
        errorMessage = tr("The maximum calls cannot be less than the block size");
    }
  }

  errorMessageLabel_->setErrorMessage(errorMessage);
  if (!errorMessage.isEmpty())
    return false;

  return true;
}
}
