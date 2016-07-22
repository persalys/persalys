//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a central tendency analysis
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
#include "otgui/CentralTendencyWizard.hxx"

#include "otgui/CollapsibleGroupBox.hxx"

#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>

#include <limits>

using namespace OT;

namespace OTGUI {

CentralTendencyWizard::CentralTendencyWizard(OTStudy * otStudy, const PhysicalModel & physicalModel)
  : AnalysisWizard(MonteCarloAnalysis(otStudy->getAvailableAnalysisName("centralTendencyMC_"), physicalModel))
  , MCAnalysis_(*dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation()))
  , taylorAnalysis_(TaylorExpansionMomentsAnalysis(otStudy->getAvailableAnalysisName("centralTendencyTaylor_"), physicalModel))
{
  buildInterface();
}


CentralTendencyWizard::CentralTendencyWizard(const Analysis & analysis)
  : AnalysisWizard(analysis)
{
  if (analysis.getImplementation()->getClassName() == "MonteCarloAnalysis")
  {
    MCAnalysis_ = *dynamic_cast<const MonteCarloAnalysis*>(&*analysis.getImplementation());
    taylorAnalysis_ = TaylorExpansionMomentsAnalysis(analysis.getName(), analysis.getPhysicalModel());
  }
  else
  {
    MCAnalysis_ = MonteCarloAnalysis(analysis.getName(), analysis.getPhysicalModel());
    taylorAnalysis_ = *dynamic_cast<const TaylorExpansionMomentsAnalysis*>(&*analysis.getImplementation());
  }
  buildInterface();
}


void CentralTendencyWizard::buildInterface()
{
  setWindowTitle("Central tendency");

  // First Page: model

  // Second Page: methods
  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * mainLayout = new QVBoxLayout(page);

  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  QButtonGroup * methodGroup = new QButtonGroup;
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Monte Carlo"));
  if (analysis_.getImplementation()->getClassName() == "MonteCarloAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup->addButton(buttonToChooseMethod, CentralTendencyWizard::MonteCarlo);
  methodLayout->addWidget(buttonToChooseMethod);
  buttonToChooseMethod = new QRadioButton(tr("Taylor expansion"));
  if (analysis_.getImplementation()->getClassName() == "TaylorExpansionMomentsAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup->addButton(buttonToChooseMethod, CentralTendencyWizard::TaylorExpansionMoments);
  methodLayout->addWidget(buttonToChooseMethod);
  connect(methodGroup, SIGNAL(buttonClicked(int)), this, SLOT(updateMethodWidgets(int)));

  mainLayout->addWidget(methodBox);

  /// -- Monte carlo widgets --
  monteCarloWidget_ = new QWidget;
  QVBoxLayout * monteCarloLayout = new QVBoxLayout(monteCarloWidget_);

  // stop criteria
  const double maxCoef = MCAnalysis_.getMaximumCoefficientOfVariation();
  const UnsignedInteger maxTime = MCAnalysis_.getMaximumElapsedTime();
  const UnsignedInteger maxCalls = MCAnalysis_.getNbSimulations();
  stopCriteriaGroupBox_ = new StopCriteriaGroupBox(maxCoef, maxTime, maxCalls);
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCoefficientOfVariationChanged(double)), this, SLOT(maxiCoefficientOfVariationChanged(double)));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiTimeChanged(int)), this, SLOT(maxiTimeChanged(int)));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCallsChanged(int)), this, SLOT(maxiCallsChanged(int)));
  monteCarloLayout->addWidget(stopCriteriaGroupBox_);

  //// advanced parameters
  CollapsibleGroupBox * advancedGroup = new CollapsibleGroupBox;
  advancedGroup->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedWidgetsLayout = new QGridLayout(advancedGroup);

  QCheckBox * confidenceIntervalCheckBox = new QCheckBox;
  confidenceIntervalCheckBox->setChecked(MCAnalysis_.isConfidenceIntervalRequired());
  confidenceIntervalCheckBox->setText(tr("Compute confidence interval at"));
  advancedWidgetsLayout->addWidget(confidenceIntervalCheckBox, 0, 0);

  // confidence interval level
  DoubleSpinBox * levelConfidenceIntervalSpinbox = new DoubleSpinBox;
  levelConfidenceIntervalSpinbox->setRange(0.0, 0.99);
  levelConfidenceIntervalSpinbox->setSingleStep(0.01);
  levelConfidenceIntervalSpinbox->setValue(MCAnalysis_.getLevelConfidenceInterval());
  connect(confidenceIntervalCheckBox, SIGNAL(toggled(bool)), levelConfidenceIntervalSpinbox, SLOT(setEnabled(bool)));
  connect(confidenceIntervalCheckBox, SIGNAL(toggled(bool)), this, SLOT(confidenceIntervalRequired(bool)));
  connect(levelConfidenceIntervalSpinbox, SIGNAL(valueChanged(double)), this, SLOT(levelConfidenceIntervalChanged(double)));
  advancedWidgetsLayout->addWidget(levelConfidenceIntervalSpinbox, 0, 1);

  // block size
  QLabel * blockSizeLabel = new QLabel(tr("Block size"));
  advancedWidgetsLayout->addWidget(blockSizeLabel, 1, 0);
  blockSizeSpinbox_ = new UIntSpinBox;
  blockSizeSpinbox_->setMinimum(1);
  blockSizeSpinbox_->setMaximum(std::numeric_limits<int>::max());
  blockSizeSpinbox_->setSingleStep(100);
  blockSizeLabel->setBuddy(blockSizeSpinbox_);
  advancedWidgetsLayout->addWidget(blockSizeSpinbox_, 1, 1);
  blockSizeSpinbox_->setValue(MCAnalysis_.getBlockSize());
  connect(blockSizeSpinbox_, SIGNAL(valueChanged(double)), this, SLOT(blockSizeChanged(double)));

  // seed
  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedWidgetsLayout->addWidget(seedLabel, 2, 0);
  QSpinBox * seedSpinbox = new QSpinBox;
  seedLabel->setBuddy(seedSpinbox);
  advancedWidgetsLayout->addWidget(seedSpinbox, 2, 1);
  seedSpinbox->setValue(MCAnalysis_.getSeed());
  connect(seedSpinbox, SIGNAL(valueChanged(int)), this, SLOT(seedChanged(int)));

  advancedGroup->setExpanded(false);
  monteCarloLayout->addWidget(advancedGroup);

  mainLayout->addWidget(monteCarloWidget_);

  /// -- Taylor expansions widgets --
  TaylorExpansionsWidget_ = new QWidget;
  mainLayout->addWidget(TaylorExpansionsWidget_);
  mainLayout->addStretch();

//   mainLayout->addLayout(methodParametersLayout);

  /// -- error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCoefficientOfVariationChanged(double)), errorMessageLabel_, SLOT(clear()));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiTimeChanged(int)), errorMessageLabel_, SLOT(clear()));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCallsChanged(int)), errorMessageLabel_, SLOT(clear()));
  connect(blockSizeSpinbox_, SIGNAL(valueChanged(double)), errorMessageLabel_, SLOT(clear()));
  mainLayout->addStretch();
  mainLayout->addWidget(errorMessageLabel_);

  updateMethodWidgets(methodGroup->checkedId());

  addPage(page);
}


void CentralTendencyWizard::updateMethodWidgets(int id)
{
  monteCarloWidget_->setVisible(id == 0);
  TaylorExpansionsWidget_->setVisible(id == 1);
  errorMessageLabel_->setText("");
}


void CentralTendencyWizard::maxiCoefficientOfVariationChanged(double maxi)
{
  MCAnalysis_.setMaximumCoefficientOfVariation(maxi);
}


void CentralTendencyWizard::maxiTimeChanged(int value)
{
  MCAnalysis_.setMaximumElapsedTime(value);
}


void CentralTendencyWizard::maxiCallsChanged(int maxi)
{
  try
  {
    MCAnalysis_.setNbSimulations(maxi);
  }
  catch (InvalidValueException exception)
  {
    // check in validateCurrentPage
  }
}


void CentralTendencyWizard::confidenceIntervalRequired(bool confidenceIntervalRequired)
{
  MCAnalysis_.setIsConfidenceIntervalRequired(confidenceIntervalRequired);
}


void CentralTendencyWizard::levelConfidenceIntervalChanged(double confidenceInterval)
{
  MCAnalysis_.setLevelConfidenceInterval(confidenceInterval);
}


void CentralTendencyWizard::seedChanged(int seed)
{
  MCAnalysis_.setSeed(seed);
}


void CentralTendencyWizard::blockSizeChanged(double size)
{
  try
  {
    MCAnalysis_.setBlockSize(size);
  }
  catch (InvalidValueException exception)
  {
    // check in validateCurrentPage
  }
}


bool CentralTendencyWizard::validateCurrentPage()
{
  if (monteCarloWidget_->isVisible())
  {
    QString errorMessage = "";
    if (!stopCriteriaGroupBox_->isValid())
      errorMessage = tr("Please select at least one stop criteria");
    else
    {
      if (!stopCriteriaGroupBox_->isMaxElapsedTimeValid())
        errorMessage = tr("The maximum time must not be null");
      if (stopCriteriaGroupBox_->isMaxCallsRequired())
        if (MCAnalysis_.getNbSimulations() < (int)blockSizeSpinbox_->value())
          errorMessage = tr("The maximum calls can not be inferior to the block size");
    }
    errorMessageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(errorMessage).arg("</font>"));
    if (!errorMessage.isEmpty())
      return false;
    analysis_ = MCAnalysis_;
  }
  else
    analysis_ = taylorAnalysis_;

  return QWizard::validateCurrentPage();
}
}