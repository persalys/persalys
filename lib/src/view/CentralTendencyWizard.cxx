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

  QVBoxLayout * methodParametersLayout = new QVBoxLayout;

  /// -- Monte carlo widgets --
  monteCarloWidget_ = new QWidget;
  QGridLayout *mclayout = new QGridLayout(monteCarloWidget_);

  // nb simu
  QLabel * nbSimuLabel = new QLabel(tr("Number of simulations"));
  QSpinBox * nbSimuSpinbox = new QSpinBox;
  nbSimuSpinbox->setMinimum(2);
  nbSimuSpinbox->setMaximum(std::numeric_limits<int>::max());
  nbSimuSpinbox->setValue(MCAnalysis_.getNbSimulations());
  connect(nbSimuSpinbox, SIGNAL(valueChanged(int)), this, SLOT(nbSimuChanged(int)));
  nbSimuLabel->setBuddy(nbSimuSpinbox);
  mclayout->addWidget(nbSimuLabel, 0, 0);
  mclayout->addWidget(nbSimuSpinbox, 0, 1);

  //// advanced parameters
  CollapsibleGroupBox * advancedGroup = new CollapsibleGroupBox;
  advancedGroup->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedWidgetsLayout = new QGridLayout(advancedGroup);

  QCheckBox * confidenceIntervalCheckBox = new QCheckBox;
  confidenceIntervalCheckBox->setChecked(MCAnalysis_.isConfidenceIntervalRequired());
  confidenceIntervalCheckBox->setText(tr("Compute confidence interval at"));
  advancedWidgetsLayout->addWidget(confidenceIntervalCheckBox, 0, 0);

  DoubleSpinBox * levelConfidenceIntervalSpinbox = new DoubleSpinBox;
  levelConfidenceIntervalSpinbox->setRange(0.0, 0.99);
  levelConfidenceIntervalSpinbox->setSingleStep(0.01);
  levelConfidenceIntervalSpinbox->setValue(MCAnalysis_.getLevelConfidenceInterval());
  connect(confidenceIntervalCheckBox, SIGNAL(toggled(bool)), levelConfidenceIntervalSpinbox, SLOT(setEnabled(bool)));
  connect(confidenceIntervalCheckBox, SIGNAL(toggled(bool)), this, SLOT(confidenceIntervalRequired(bool)));
  connect(levelConfidenceIntervalSpinbox, SIGNAL(valueChanged(double)), this, SLOT(levelConfidenceIntervalChanged(double)));
  advancedWidgetsLayout->addWidget(levelConfidenceIntervalSpinbox, 0, 1);

  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedWidgetsLayout->addWidget(seedLabel, 2, 0);
  QSpinBox * seedSpinbox = new QSpinBox;
  seedLabel->setBuddy(seedSpinbox);
  advancedWidgetsLayout->addWidget(seedSpinbox, 2, 1);
  seedSpinbox->setValue(MCAnalysis_.getSeed());
  connect(seedSpinbox, SIGNAL(valueChanged(int)), this, SLOT(seedChanged(int)));

  advancedGroup->setExpanded(false);
  mclayout->addWidget(advancedGroup, 2, 0, 1, 2);

  methodParametersLayout->addWidget(monteCarloWidget_);

  /// -- Taylor expansions widgets --
  TaylorExpansionsWidget_ = new QWidget;
  //QVBoxLayout * taylorLayout = new QVBoxLayout(TaylorExpansionsWidget_);
  methodParametersLayout->addWidget(TaylorExpansionsWidget_);
  methodParametersLayout->addStretch();

  mainLayout->addLayout(methodParametersLayout);

  updateMethodWidgets(methodGroup->checkedId());

  addPage(page);
}


void CentralTendencyWizard::updateMethodWidgets(int id)
{
  monteCarloWidget_->setVisible(id == 0);
  TaylorExpansionsWidget_->setVisible(id == 1);
}


void CentralTendencyWizard::nbSimuChanged(int nbSimu)
{
  MCAnalysis_.setNbSimulations(nbSimu);
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


void CentralTendencyWizard::accept()
{
  if (monteCarloWidget_->isVisible())
    analysis_ = MCAnalysis_;
  else
    analysis_ = taylorAnalysis_;
  QDialog::accept();
}
}