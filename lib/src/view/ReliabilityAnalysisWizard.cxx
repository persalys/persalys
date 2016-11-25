//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a reliability analysis
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/ReliabilityAnalysisWizard.hxx"

#include "otgui/MonteCarloReliabilityAnalysis.hxx"
#include "otgui/CollapsibleGroupBox.hxx"

#include <QRadioButton>
#include <QVBoxLayout>

#include <limits>

using namespace OT;

namespace OTGUI {

ReliabilityAnalysisWizard::ReliabilityAnalysisWizard(const OTStudy& otStudy, const LimitState & limitState, QWidget* parent)
  : AnalysisWizard(MonteCarloReliabilityAnalysis(otStudy.getAvailableAnalysisName("reliability_"), limitState), parent)
{
  buildInterface();
}


ReliabilityAnalysisWizard::ReliabilityAnalysisWizard(const Analysis & analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
{
  buildInterface();
}


void ReliabilityAnalysisWizard::buildInterface()
{
  setWindowTitle("Threshold exceedance");

  // First Page: model

  // Second Page: methods
  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * mainLayout = new QVBoxLayout(page);

  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  methodGroup_ = new QButtonGroup;
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Monte Carlo"));
  if (analysis_.getImplementation()->getClassName() == "MonteCarloReliabilityAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, ReliabilityAnalysisWizard::MonteCarlo);
  methodLayout->addWidget(buttonToChooseMethod);

  mainLayout->addWidget(methodBox);

  /// monte carlo widgets
  monteCarloWidget_ = new QWidget;
  QVBoxLayout * monteCarloLayout = new QVBoxLayout(monteCarloWidget_);

  // stop criteria
  const double maxCoef = dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->getMaximumCoefficientOfVariation();
  const UnsignedInteger maxTime = dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->getMaximumElapsedTime();
  const UnsignedInteger maxCalls = dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->getMaximumCalls();
  stopCriteriaGroupBox_ = new StopCriteriaGroupBox(maxCoef, maxTime, maxCalls);
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCoefficientOfVariationChanged(double)), this, SLOT(maxiCoefficientOfVariationChanged(double)));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiTimeChanged(int)), this, SLOT(maxiTimeChanged(int)));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCallsChanged(int)), this, SLOT(maxiCallsChanged(int)));

  monteCarloLayout->addWidget(stopCriteriaGroupBox_);

  // block size
  blockSizeGroupBox_ = new BlockSizeGroupBox(tr("Evaluation parameter"));
  blockSizeGroupBox_->setBlockSizeValue(dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->getBlockSize());
  connect(blockSizeGroupBox_, SIGNAL(blockSizeChanged(double)), this, SLOT(blockSizeChanged(double)));
  monteCarloLayout->addWidget(blockSizeGroupBox_);

  //// advanced parameters
  CollapsibleGroupBox * advancedParamGroupBox = new CollapsibleGroupBox;
  advancedParamGroupBox->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedWidgetsLayout = new QGridLayout(advancedParamGroupBox);

  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedWidgetsLayout->addWidget(seedLabel, 1, 0);
  seedSpinbox_ = new QSpinBox;
  seedSpinbox_->setMaximum(std::numeric_limits<int>::max());
  seedLabel->setBuddy(seedSpinbox_);
  advancedWidgetsLayout->addWidget(seedSpinbox_, 1, 1);
  if (analysis_.getImplementation()->getClassName() == "MonteCarloReliabilityAnalysis")
    seedSpinbox_->setValue(dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->getSeed());
  connect(seedSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(seedChanged(int)));

  monteCarloLayout->addWidget(advancedParamGroupBox);

  mainLayout->addWidget(monteCarloWidget_);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCoefficientOfVariationChanged(double)), errorMessageLabel_, SLOT(clear()));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiTimeChanged(int)), errorMessageLabel_, SLOT(clear()));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCallsChanged(int)), errorMessageLabel_, SLOT(clear()));
  connect(blockSizeGroupBox_, SIGNAL(blockSizeChanged(double)), errorMessageLabel_, SLOT(clear()));
  mainLayout->addStretch();
  mainLayout->addWidget(errorMessageLabel_);

  addPage(page);
}

void ReliabilityAnalysisWizard::maxiCoefficientOfVariationChanged(double maxi)
{
  dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->setMaximumCoefficientOfVariation(maxi);
}


void ReliabilityAnalysisWizard::maxiTimeChanged(int value)
{
  dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->setMaximumElapsedTime(value);
}


void ReliabilityAnalysisWizard::maxiCallsChanged(int maxi)
{
  try
  {
    dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->setMaximumCalls(maxi);
  }
  catch (InvalidValueException exception)
  {
    // check in validateCurrentPage
  }
}


void ReliabilityAnalysisWizard::blockSizeChanged(double size)
{
  try
  {
    dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->setBlockSize(size);
  }
  catch (InvalidValueException exception)
  {
    // check in validateCurrentPage
  }
}


void ReliabilityAnalysisWizard::seedChanged(int seed)
{
  dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->setSeed(seed);
}


bool ReliabilityAnalysisWizard::validateCurrentPage()
{
  QString errorMessage = "";
  if (!stopCriteriaGroupBox_->isValid())
    errorMessage = tr("Please select at least one stop criteria");
  else
  {
    if (!stopCriteriaGroupBox_->isMaxElapsedTimeValid())
      errorMessage = tr("The maximum time must not be null");
    if (stopCriteriaGroupBox_->isMaxCallsRequired())
      if (dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->getMaximumCalls() < blockSizeGroupBox_->getBlockSizeValue())
        errorMessage = tr("The maximum calls can not be inferior to the block size");
  }

  errorMessageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(errorMessage).arg("</font>"));
  if (!errorMessage.isEmpty())
    return false;
  return QWizard::validateCurrentPage();
}
}