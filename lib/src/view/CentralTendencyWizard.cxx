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

#include "otgui/MonteCarloAnalysis.hxx"
#include "otgui/TaylorExpansionMomentsAnalysis.hxx"
#include "otgui/CollapsibleGroupBox.hxx"

#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <limits>

namespace OTGUI {

CentralTendencyWizard::CentralTendencyWizard(OTStudy * otStudy, const PhysicalModel & physicalModel)
  : OTguiWizard()
  , analysis_(MonteCarloAnalysis(otStudy->getAvailableAnalysisName("centralTendency_"), physicalModel))
  , otStudy_(otStudy)
  , physicalModel_(physicalModel)
{
  buildInterface();
}


CentralTendencyWizard::CentralTendencyWizard(const Analysis & analysis)
  : OTguiWizard()
  , analysis_(analysis)
  , physicalModel_(analysis_.getPhysicalModel())
{
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

  methodGroup_ = new QButtonGroup;
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Monte Carlo"));
  if (analysis_.getImplementation()->getClassName() == "MonteCarloAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, CentralTendencyWizard::MonteCarlo);
  methodLayout->addWidget(buttonToChooseMethod);
  buttonToChooseMethod = new QRadioButton(tr("Taylor expansion"));
  if (analysis_.getImplementation()->getClassName() == "TaylorExpansionMomentsAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, CentralTendencyWizard::TaylorExpansionMoments);
  methodLayout->addWidget(buttonToChooseMethod);
  connect(methodGroup_, SIGNAL(buttonClicked(int)), this, SLOT(updateMethodWidgets()));

  mainLayout->addWidget(methodBox);

  QVBoxLayout * methodParametersLayout = new QVBoxLayout;

  /// monte carlo widgets
  monteCarloWidget_ = new QWidget;
  QGridLayout *mclayout = new QGridLayout(monteCarloWidget_);

  QLabel * nbSimuLabel = new QLabel(tr("Number of simulations"));
  nbSimuSpinbox_ = new QSpinBox;
  nbSimuSpinbox_->setMinimum(2);
  nbSimuSpinbox_->setMaximum(std::numeric_limits<int>::max());
  if (analysis_.getImplementation()->getClassName() == "MonteCarloAnalysis")
    nbSimuSpinbox_->setValue(dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->getNbSimulations());
  connect(nbSimuSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(nbSimuChanged(int)));

  nbSimuLabel->setBuddy(nbSimuSpinbox_);
  mclayout->addWidget(nbSimuLabel, 0, 0);
  mclayout->addWidget(nbSimuSpinbox_, 0, 1);

  //// advanced parameters
  CollapsibleGroupBox * advancedGroup = new CollapsibleGroupBox;
  advancedGroup->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedWidgetsLayout = new QGridLayout(advancedGroup);

  confidenceIntervalCheckBox_ = new QCheckBox;
  if (analysis_.getImplementation()->getClassName() == "MonteCarloAnalysis")
    confidenceIntervalCheckBox_->setChecked(dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->isConfidenceIntervalRequired());
  else
    confidenceIntervalCheckBox_->setChecked(true);
  confidenceIntervalCheckBox_->setText(tr("Compute confidence interval at"));
  connect(confidenceIntervalCheckBox_, SIGNAL(toggled(bool)), this, SLOT(confidenceIntervalRequired(bool)));
  advancedWidgetsLayout->addWidget(confidenceIntervalCheckBox_, 0, 0);
  levelConfidenceIntervalSpinbox_ = new QDoubleSpinBox;
  levelConfidenceIntervalSpinbox_->setRange(0.0, 1.0);
  levelConfidenceIntervalSpinbox_->setSingleStep(0.05);

  if (analysis_.getImplementation()->getClassName() == "MonteCarloAnalysis")
  {
    levelConfidenceIntervalSpinbox_->setValue(dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->getLevelConfidenceInterval());
    confidenceIntervalRequired(confidenceIntervalCheckBox_->isChecked());
  }
  else
  {
    levelConfidenceIntervalSpinbox_->setValue(0.95);
    levelConfidenceIntervalSpinbox_->setEnabled(true);
  }

  connect(levelConfidenceIntervalSpinbox_, SIGNAL(valueChanged(double)), this, SLOT(levelConfidenceIntervalChanged(double)));
  advancedWidgetsLayout->addWidget(levelConfidenceIntervalSpinbox_, 0, 1);

  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedWidgetsLayout->addWidget(seedLabel, 2, 0);
  seedSpinbox_ = new QSpinBox;
  seedLabel->setBuddy(seedSpinbox_);
  advancedWidgetsLayout->addWidget(seedSpinbox_, 2, 1);
  if (analysis_.getImplementation()->getClassName() == "MonteCarloAnalysis")
    seedSpinbox_->setValue(dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->getSeed());
  connect(seedSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(seedChanged(int)));

  advancedGroup->setExpanded(false);
  mclayout->addWidget(advancedGroup, 2, 0, 1, 2);

  methodParametersLayout->addWidget(monteCarloWidget_);

  /// Taylor expansions widgets
  TaylorExpansionsWidget_ = new QWidget;
  //QVBoxLayout * taylorLayout = new QVBoxLayout(TaylorExpansionsWidget_);
  methodParametersLayout->addWidget(TaylorExpansionsWidget_);
  methodParametersLayout->addStretch();

  mainLayout->addLayout(methodParametersLayout);

  updateMethodWidgets();

  addPage(page);
}


void CentralTendencyWizard::updateMethodWidgets()
{
  switch (CentralTendencyWizard::Method(methodGroup_->checkedId()))
  {
    case CentralTendencyWizard::MonteCarlo:
    {
      monteCarloWidget_->show();
      TaylorExpansionsWidget_->hide();
      if (analysis_.getImplementation()->getClassName() == "TaylorExpansionMomentsAnalysis")
      {
        analysis_ = MonteCarloAnalysis(analysis_.getName(), physicalModel_);
        emit analysisChanged(analysis_);
      }
      nbSimuSpinbox_->setValue(dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->getNbSimulations());
      break;
    }
    case CentralTendencyWizard::TaylorExpansionMoments:
    {
      monteCarloWidget_->hide();
      TaylorExpansionsWidget_->show();
      if (analysis_.getImplementation()->getClassName() == "MonteCarloAnalysis")
      {
        analysis_ = TaylorExpansionMomentsAnalysis(analysis_.getName(), physicalModel_);
        emit analysisChanged(analysis_);
      }
      break;
    }
    default:
      break;
  }
}


void CentralTendencyWizard::confidenceIntervalRequired(bool confidenceIntervalRequired)
{
  if (confidenceIntervalRequired)
  {
    levelConfidenceIntervalSpinbox_->setEnabled(true);
    dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->setIsConfidenceIntervalRequired(true);
  }
  else
  {
    levelConfidenceIntervalSpinbox_->setEnabled(false);
    dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->setIsConfidenceIntervalRequired(false);
  }
}


void CentralTendencyWizard::nbSimuChanged(int nbSimu)
{
  dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->setNbSimulations(nbSimu);
}


void CentralTendencyWizard::levelConfidenceIntervalChanged(double confidenceInterval)
{
  dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->setLevelConfidenceInterval(confidenceInterval);
}


void CentralTendencyWizard::seedChanged(int seed)
{
  dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->setSeed(seed);
}


QString CentralTendencyWizard::getAnalysisName() const
{
  return analysis_.getName().c_str();
}


void CentralTendencyWizard::validate()
{
  otStudy_->add(analysis_);
}
}
