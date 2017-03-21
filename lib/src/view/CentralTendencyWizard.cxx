//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a central tendency analysis
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
#include "otgui/CentralTendencyWizard.hxx"

#include "otgui/CollapsibleGroupBox.hxx"

#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>

using namespace OT;

namespace OTGUI {

CentralTendencyWizard::CentralTendencyWizard(const OTStudy& otStudy, const PhysicalModel & physicalModel, QWidget* parent)
  : AnalysisWizard(MonteCarloAnalysis(otStudy.getAvailableAnalysisName("centralTendencyMC_"), physicalModel), parent)
  , MCAnalysis_(*dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation()))
  , taylorAnalysis_(TaylorExpansionMomentsAnalysis(otStudy.getAvailableAnalysisName("centralTendencyTaylor_"), physicalModel))
  , errorMessageLabel_(new QLabel)
{
  buildInterface();
}


CentralTendencyWizard::CentralTendencyWizard(const Analysis & analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
  , errorMessageLabel_(new QLabel)
{
  if (analysis.getImplementation()->getClassName() == "MonteCarloAnalysis")
  {
    MCAnalysis_ = *dynamic_cast<const MonteCarloAnalysis*>(&*analysis.getImplementation());
    taylorAnalysis_ = TaylorExpansionMomentsAnalysis(MCAnalysis_.getName(), MCAnalysis_.getPhysicalModel());
    try
    {
      taylorAnalysis_.setInterestVariables(MCAnalysis_.getInterestVariables());
    }
    catch (std::exception)
    {
      // do nothing
    }
  }
  else
  {
    taylorAnalysis_ = *dynamic_cast<const TaylorExpansionMomentsAnalysis*>(&*analysis.getImplementation());
    MCAnalysis_ = MonteCarloAnalysis(taylorAnalysis_.getName(), taylorAnalysis_.getPhysicalModel());
    try
    {
      MCAnalysis_.setInterestVariables(taylorAnalysis_.getInterestVariables());
    }
    catch (std::exception)
    {
      // do nothing
    }
  }
  buildInterface();
}


void CentralTendencyWizard::buildInterface()
{
  setWindowTitle(tr("Central tendency"));

  // First Page: model

  // Second Page: methods
  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * mainLayout = new QVBoxLayout(page);

  // output selection
  outputsGroupBox_ = new OutputsSelectionGroupBox(MCAnalysis_.getPhysicalModel().getSelectedOutputsNames(), MCAnalysis_.getInterestVariables(), this);
  setInterestVariables(outputsGroupBox_->getSelectedOutputsNames());
  connect(outputsGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), this, SLOT(setInterestVariables(QStringList)));
  mainLayout->addWidget(outputsGroupBox_);

  // method selection
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
  const UnsignedInteger maxCalls = MCAnalysis_.getMaximumCalls();
  stopCriteriaGroupBox_ = new StopCriteriaGroupBox(maxCoef, maxTime, maxCalls);
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCoefficientOfVariationChanged(double)), this, SLOT(maxiCoefficientOfVariationChanged(double)));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiTimeChanged(int)), this, SLOT(maxiTimeChanged(int)));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCallsChanged(int)), this, SLOT(maxiCallsChanged(int)));
  monteCarloLayout->addWidget(stopCriteriaGroupBox_);

  // block size
  blockSizeGroupBox_ = new BlockSizeGroupBox(tr("Evaluation parameter"));
  blockSizeGroupBox_->setBlockSizeValue(MCAnalysis_.getBlockSize());
  connect(blockSizeGroupBox_, SIGNAL(blockSizeChanged(double)), this, SLOT(blockSizeChanged(double)));
  monteCarloLayout->addWidget(blockSizeGroupBox_);

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
  errorMessageLabel_->setWordWrap(true);
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
  errorMessageLabel_->setText("");
  MCAnalysis_.setMaximumElapsedTime(value);
}


void CentralTendencyWizard::maxiCallsChanged(int maxi)
{
  errorMessageLabel_->setText("");
  try
  {
    MCAnalysis_.setMaximumCalls(maxi);
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
  errorMessageLabel_->setText("");
  try
  {
    MCAnalysis_.setBlockSize(size);
  }
  catch (InvalidValueException exception)
  {
    // check in validateCurrentPage
  }
}


void CentralTendencyWizard::setInterestVariables(QStringList outputsList)
{
  errorMessageLabel_->setText("");

  Description desc(outputsList.size());
  for (int i=0; i<outputsList.size(); ++i)
    desc[i] = outputsList[i].toUtf8().constData();

  try
  {
    taylorAnalysis_.setInterestVariables(desc);
    MCAnalysis_.setInterestVariables(desc);
  }
  catch (InvalidDimensionException exception)
  {
    // check in validateCurrentPage
  }
}


bool CentralTendencyWizard::validateCurrentPage()
{
  errorMessageLabel_->setText("");
  if (!outputsGroupBox_->getSelectedOutputsNames().size())
  {
    errorMessageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(tr("At least one output must be selected")).arg("</font>"));
    return false;
  }
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
        if (MCAnalysis_.getMaximumCalls() < blockSizeGroupBox_->getBlockSizeValue())
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