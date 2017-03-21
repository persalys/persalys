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
#include <QRadioButton>

using namespace OT;

namespace OTGUI {

SimulationReliabilityPage::SimulationReliabilityPage(QWidget* parent)
  : QWizardPage(parent)
{
  buildInterface();
}


void SimulationReliabilityPage::buildInterface()
{
  setTitle(tr("Simulation methods"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  QGroupBox * methodBox = new QGroupBox(tr("Methods"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  // radioButtons
  methodGroup_ = new QButtonGroup;
  // Monte carlo
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Monte Carlo"));
  buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, SimulationReliabilityPage::MonteCarlo);
  methodLayout->addWidget(buttonToChooseMethod);
  // FORM - IS
  buttonToChooseMethod = new QRadioButton(tr("FORM - Importance sampling"));
  methodGroup_->addButton(buttonToChooseMethod, SimulationReliabilityPage::FORM_IS);
  methodLayout->addWidget(buttonToChooseMethod);
  connect(methodGroup_, SIGNAL(buttonClicked(int)), this, SIGNAL(methodChanged(int)));

  pageLayout->addWidget(methodBox);

  /// simulation widgets
  QWidget * mainSimuWidget = new QWidget;
  QVBoxLayout * monteCarloLayout = new QVBoxLayout(mainSimuWidget);

  // stop criteria
  stopCriteriaGroupBox_ = new StopCriteriaGroupBox(0.01, 60, (UnsignedInteger)std::numeric_limits<int>::max());
  monteCarloLayout->addWidget(stopCriteriaGroupBox_);

  // block size
  blockSizeGroupBox_ = new BlockSizeGroupBox(tr("Evaluation parameter"));
  monteCarloLayout->addWidget(blockSizeGroupBox_);

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

  monteCarloLayout->addWidget(advancedParamGroupBox);

  pageLayout->addWidget(mainSimuWidget);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);

  // error message
  pageLayout->addWidget(errorMessageLabel_);
}


void SimulationReliabilityPage::initialize(const Analysis& analysis)
{
  if (!dynamic_cast<const SimulationReliabilityAnalysis*>(&*analysis.getImplementation()))
    return;

  const SimulationReliabilityAnalysis * analysis_ptr = dynamic_cast<const SimulationReliabilityAnalysis*>(analysis.getImplementation().get());
  if (dynamic_cast<const MonteCarloReliabilityAnalysis*>(analysis_ptr))
    methodGroup_->button(SimulationReliabilityPage::MonteCarlo)->click();
  else
    methodGroup_->button(SimulationReliabilityPage::FORM_IS)->click();

  stopCriteriaGroupBox_->setMaximumCoefficientOfVariation(analysis_ptr->getMaximumCoefficientOfVariation());
  stopCriteriaGroupBox_->setMaximumElapsedTime(analysis_ptr->getMaximumElapsedTime());
  stopCriteriaGroupBox_->setMaximumCalls(analysis_ptr->getMaximumCalls());

  blockSizeGroupBox_->setBlockSizeValue(analysis_ptr->getBlockSize());

  seedSpinbox_->setValue(analysis_ptr->getSeed());
}


Analysis SimulationReliabilityPage::getAnalysis(const OT::String& name, const LimitState& limitState) const
{
  if (methodGroup_->checkedId() == SimulationReliabilityPage::MonteCarlo)
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


int SimulationReliabilityPage::nextId() const
{
  if (methodGroup_->checkedId() == SimulationReliabilityPage::MonteCarlo)
    return -1;
  else
    return ReliabilityAnalysisWizard::Page_FORM;
}


bool SimulationReliabilityPage::validatePage()
{
  QString errorMessage = "";
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

  errorMessageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(errorMessage).arg("</font>"));
  if (!errorMessage.isEmpty())
    return false;

  return true;
}
}