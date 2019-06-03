//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define a probabilistic design of experiments
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
#include "persalys/ProbabilisticDesignPage.hxx"

#include "persalys/ProbabilisticDesignOfExperiment.hxx"
#include "persalys/CollapsibleGroupBox.hxx"

#include <QGridLayout>
#include <QRadioButton>
#include <QGroupBox>
#include <QLabel>

using namespace OT;

namespace PERSALYS
{

ProbabilisticDesignPage::ProbabilisticDesignPage(QWidget* parent)
  : QWizardPage(parent)
{
  buildInterface();
}


void ProbabilisticDesignPage::buildInterface()
{
  setTitle(tr("Probabilistic design of experiments"));

  QGridLayout * pageLayout = new QGridLayout(this);

  QGroupBox * designGroupBox = new QGroupBox(tr("Designs"));
  QGridLayout * designGroupBoxLayout = new QGridLayout(designGroupBox);
  designsGroup_ = new QButtonGroup(this);

  QRadioButton * buttonToChooseDesign = new QRadioButton(tr("LHS"));
  buttonToChooseDesign->setToolTip(tr("Latin Hypercube Sampling"));
  buttonToChooseDesign->setChecked(true);
  designsGroup_->addButton(buttonToChooseDesign, ProbabilisticDesignPage::LHS);
  designGroupBoxLayout->addWidget(buttonToChooseDesign);

  buttonToChooseDesign = new QRadioButton(tr("Monte Carlo"));
  designsGroup_->addButton(buttonToChooseDesign, ProbabilisticDesignPage::MonteCarlo);
  designGroupBoxLayout->addWidget(buttonToChooseDesign);

  buttonToChooseDesign = new QRadioButton(tr("Quasi-Monte Carlo"));
  designsGroup_->addButton(buttonToChooseDesign, ProbabilisticDesignPage::QuasiMonteCarlo);
  designGroupBoxLayout->addWidget(buttonToChooseDesign);

  pageLayout->addWidget(designGroupBox);

  // sample size
  QGroupBox * sampleSizeGroupBox = new QGroupBox(tr("Generation parameter"));
  QGridLayout * sampleSizeLayout = new QGridLayout(sampleSizeGroupBox);

  QLabel * sampleSizeLabel = new QLabel(tr("Sample size"));
  sampleSizeLayout->addWidget(sampleSizeLabel, 0, 0);

  sampleSizeSpinbox_ = new LogSpinBox;
  sampleSizeSpinbox_->setMinimum(1);
  sampleSizeSpinbox_->setMaximum(std::numeric_limits<int>::max());
  sampleSizeLabel->setBuddy(sampleSizeSpinbox_);
  sampleSizeLayout->addWidget(sampleSizeSpinbox_, 0, 1);

  pageLayout->addWidget(sampleSizeGroupBox);

  // advanced parameters
  CollapsibleGroupBox * advancedParamGroupBox = new CollapsibleGroupBox;
  advancedParamGroupBox->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedParamGroupBoxLayout = new QGridLayout(advancedParamGroupBox);

  // seed
  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedParamGroupBoxLayout->addWidget(seedLabel, 0, 0);

  seedSpinbox_ = new QSpinBox;
  seedSpinbox_->setMaximum(std::numeric_limits<int>::max());
  seedLabel->setBuddy(seedSpinbox_);
  advancedParamGroupBoxLayout->addWidget(seedSpinbox_, 0, 1);

  advancedParamGroupBox->setExpanded(false);
  pageLayout->addWidget(advancedParamGroupBox);
  pageLayout->setRowStretch(3, 1);

  initialize(ProbabilisticDesignOfExperiment());
}


void ProbabilisticDesignPage::initialize(const Analysis& analysis)
{
  const DesignOfExperimentEvaluation * doe_ptr = dynamic_cast<const DesignOfExperimentEvaluation*>(analysis.getImplementation().get());

  if (!doe_ptr)
    return;

  // check the independence of the copula
  const bool independentCopula = doe_ptr->getPhysicalModel().getCopula().hasIndependentCopula();
  if (!independentCopula)
  {
    designsGroup_->button(ProbabilisticDesignPage::LHS)->setEnabled(false);
    designsGroup_->button(ProbabilisticDesignPage::LHS)->setToolTip(tr("The physical model does not have an independent copula"));
    designsGroup_->button(ProbabilisticDesignPage::MonteCarlo)->click(); // default
  }

  // initialize widgets if the analysis is already a ProbabilisticDesignOfExperiment
  const ProbabilisticDesignOfExperiment * probaDoe_ptr = dynamic_cast<const ProbabilisticDesignOfExperiment*>(doe_ptr);
  if (!probaDoe_ptr)
    return;

  if (probaDoe_ptr->getDesignName() == "LHS" && independentCopula)
    designsGroup_->button(ProbabilisticDesignPage::LHS)->click();
  else if (probaDoe_ptr->getDesignName() == "QUASI_MONTE_CARLO")
    designsGroup_->button(ProbabilisticDesignPage::QuasiMonteCarlo)->click();
  else
    designsGroup_->button(ProbabilisticDesignPage::MonteCarlo)->click();

  sampleSizeSpinbox_->setValue(probaDoe_ptr->getSize());
  seedSpinbox_->setValue(probaDoe_ptr->getSeed());
}


Analysis ProbabilisticDesignPage::getAnalysis(const String& name, const PhysicalModel& model) const
{
  String designType = "LHS";
  if (designsGroup_->checkedId() == ProbabilisticDesignPage::MonteCarlo)
    designType = "MONTE_CARLO";
  else if (designsGroup_->checkedId() == ProbabilisticDesignPage::QuasiMonteCarlo)
    designType = "QUASI_MONTE_CARLO";
  ProbabilisticDesignOfExperiment design(name, model, sampleSizeSpinbox_->value(), designType);
  design.setSeed(seedSpinbox_->value());

  return design;
}
}
