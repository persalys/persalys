//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a SensitivityAnalysis
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
#include "otgui/SensitivityAnalysisWizard.hxx"

#include "otgui/SobolAnalysis.hxx"
#include "otgui/SRCAnalysis.hxx"
#include "otgui/CollapsibleGroupBox.hxx"

#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>

#include <limits>

namespace OTGUI {

SensitivityAnalysisWizard::SensitivityAnalysisWizard(OTStudy * otStudy, const PhysicalModel & physicalModel)
  : OTguiWizard()
  , analysis_(SobolAnalysis(otStudy->getAvailableAnalysisName("sensitivity_"), physicalModel))
  , otStudy_(otStudy)
  , physicalModel_(physicalModel)
{
  buildInterface();
}


SensitivityAnalysisWizard::SensitivityAnalysisWizard(const Analysis & analysis)
  : OTguiWizard()
  , analysis_(analysis)
  , physicalModel_(analysis_.getPhysicalModel())
{
  buildInterface();
}


void SensitivityAnalysisWizard::buildInterface()
{
  setWindowTitle("Sensitivity analysis");

  // First Page: model

  // Second Page: methods
  QWizardPage * page = new QWizardPage(this);

  QVBoxLayout * mainLayout = new QVBoxLayout(page);

  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  methodGroup_ = new QButtonGroup;
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Sobol"));
  if (analysis_.getImplementation()->getClassName() == "SobolAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, SensitivityAnalysisWizard::Sobol);
  methodLayout->addWidget(buttonToChooseMethod);
  buttonToChooseMethod = new QRadioButton(tr("SRC"));
  if (analysis_.getImplementation()->getClassName() == "SRCAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, SensitivityAnalysisWizard::SRC);
  methodLayout->addWidget(buttonToChooseMethod);
  connect(methodGroup_, SIGNAL(buttonClicked(int)), this, SLOT(updateMethodWidgets()));

  mainLayout->addWidget(methodBox);

  QWidget * methodParametersWidgets = new QWidget;
  QHBoxLayout * methodParametersLayout = new QHBoxLayout(methodParametersWidgets);

  QLabel * sampleSizeLabel = new QLabel(tr("Sample size"));
  sampleSizeSpinbox_ = new QSpinBox;
  sampleSizeSpinbox_->setMinimum(2);
  sampleSizeSpinbox_->setMaximum(std::numeric_limits<int>::max());
  sampleSizeSpinbox_->setValue(dynamic_cast<SimulationAnalysis*>(&*analysis_.getImplementation())->getNbSimulations());
  connect(sampleSizeSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(sampleSizeChanged(int)));

  sampleSizeLabel->setBuddy(sampleSizeSpinbox_);
  methodParametersLayout->addWidget(sampleSizeLabel);
  methodParametersLayout->addWidget(sampleSizeSpinbox_);
  mainLayout->addWidget(methodParametersWidgets);

  // Sobol widgets
  sobolWidgets_ = new QWidget;
  QVBoxLayout * sobolWidgetsLayout = new QVBoxLayout(sobolWidgets_);

  QHBoxLayout * nbSimuLayout = new QHBoxLayout;
  QLabel * totalNbSimuLabel = new QLabel(tr("The total number of simulations"));
  // total nb simu: N=n*(d+2)
  // n = nb inputs; d=sample size
  double nbSimu = physicalModel_.getInputs().getSize() * (sampleSizeSpinbox_->value() + 2);
  totalNbSimuLabel_ = new QLabel(QString::number(nbSimu));
  nbSimuLayout->addWidget(totalNbSimuLabel);
  nbSimuLayout->addWidget(totalNbSimuLabel_);
  sobolWidgetsLayout->addLayout(nbSimuLayout);

  // sobol advanced parameters
  CollapsibleGroupBox * sobolAdvancedParamGroupBox = new CollapsibleGroupBox;
  sobolAdvancedParamGroupBox->setTitle(tr("Advanced parameters"));
  QGridLayout * sobolAdvancedParamGroupBoxLayout = new QGridLayout(sobolAdvancedParamGroupBox);

  QLabel * seedLabel = new QLabel(tr("Seed"));
  sobolAdvancedParamGroupBoxLayout->addWidget(seedLabel, 0, 0);
  QSpinBox * sobolSeedSpinbox = new QSpinBox;
  sobolSeedSpinbox->setMaximum(std::numeric_limits<int>::max());
  seedLabel->setBuddy(sobolSeedSpinbox);
  sobolAdvancedParamGroupBoxLayout->addWidget(sobolSeedSpinbox, 0, 1);
  sobolSeedSpinbox->setValue(dynamic_cast<SimulationAnalysis*>(&*analysis_.getImplementation())->getSeed());
  connect(sobolSeedSpinbox, SIGNAL(valueChanged(int)), this, SLOT(seedChanged(int)));

  QLabel * blockSizeLabel = new QLabel(tr("Block size"));
  sobolAdvancedParamGroupBoxLayout->addWidget(blockSizeLabel, 1, 0);
  QSpinBox * blockSizeSpinbox = new QSpinBox;
  blockSizeSpinbox->setMinimum(1);
  blockSizeSpinbox->setMaximum(std::numeric_limits<int>::max());
  blockSizeLabel->setBuddy(blockSizeSpinbox);
  sobolAdvancedParamGroupBoxLayout->addWidget(blockSizeSpinbox, 1, 1);
  if (analysis_.getImplementation()->getClassName() == "SobolAnalysis")
    blockSizeSpinbox->setValue(dynamic_cast<SobolAnalysis*>(&*analysis_.getImplementation())->getBlockSize());

  connect(blockSizeSpinbox, SIGNAL(valueChanged(int)), this, SLOT(blockSizeChanged(int)));

  sobolAdvancedParamGroupBox->setExpanded(false);
  sobolWidgetsLayout->addWidget(sobolAdvancedParamGroupBox);
  mainLayout->addWidget(sobolWidgets_);

  // SRC widgets
  srcWidgets_ = new QWidget;
  QVBoxLayout * srcWidgetsLayout = new QVBoxLayout(srcWidgets_);

  // SRC advanced parameters
  CollapsibleGroupBox * srcAdvancedParamGroupBox = new CollapsibleGroupBox;
  srcAdvancedParamGroupBox->setTitle(tr("Advanced parameters"));
  QGridLayout * srcAdvancedParamGroupBoxLayout = new QGridLayout(srcAdvancedParamGroupBox);

  seedLabel = new QLabel(tr("Seed"));
  srcAdvancedParamGroupBoxLayout->addWidget(seedLabel, 0, 0);
  QSpinBox * srcSeedSpinbox = new QSpinBox;
  srcSeedSpinbox->setMaximum(std::numeric_limits<int>::max());
  seedLabel->setBuddy(srcSeedSpinbox);
  srcAdvancedParamGroupBoxLayout->addWidget(srcSeedSpinbox, 0, 1);
  srcSeedSpinbox->setValue(dynamic_cast<SimulationAnalysis*>(&*analysis_.getImplementation())->getSeed());
  connect(srcSeedSpinbox, SIGNAL(valueChanged(int)), this, SLOT(seedChanged(int)));

  srcAdvancedParamGroupBox->setExpanded(false);
  srcWidgetsLayout->addWidget(srcAdvancedParamGroupBox);
  mainLayout->addWidget(srcWidgets_);

  updateMethodWidgets();

  addPage(page);
}


void SensitivityAnalysisWizard::updateMethodWidgets()
{
  switch (SensitivityAnalysisWizard::Method(methodGroup_->checkedId()))
  {
    case SensitivityAnalysisWizard::Sobol:
    {
      if (analysis_.getImplementation()->getClassName() == "SRCAnalysis")
      {
        analysis_ = SobolAnalysis(analysis_.getName(), physicalModel_);
        emit analysisChanged(analysis_);
      }
      break;
    }
    case SensitivityAnalysisWizard::SRC:
    {
      if (analysis_.getImplementation()->getClassName() == "SobolAnalysis")
      {
        analysis_ = SRCAnalysis(analysis_.getName(), physicalModel_);
        emit analysisChanged(analysis_);
      }
      break;
    }
    default:
      break;
  }
  sobolWidgets_->setVisible(analysis_.getImplementation()->getClassName() == "SobolAnalysis");
  srcWidgets_->setVisible(analysis_.getImplementation()->getClassName() == "SRCAnalysis");
}


void SensitivityAnalysisWizard::sampleSizeChanged(int sampleSize)
{
  // total nb simu: N=n*(d+2)
  // n = nb inputs; d=sample size
  int nbSimu = physicalModel_.getInputs().getSize() * (sampleSize + 2);
  totalNbSimuLabel_->setText(QString::number(nbSimu));
  dynamic_cast<SimulationAnalysis*>(&*analysis_.getImplementation())->setNbSimulations(sampleSize);
}


void SensitivityAnalysisWizard::blockSizeChanged(int size)
{
  dynamic_cast<SobolAnalysis*>(&*analysis_.getImplementation())->setBlockSize(size);
}


void SensitivityAnalysisWizard::seedChanged(int seed)
{
  dynamic_cast<SimulationAnalysis*>(&*analysis_.getImplementation())->setSeed(seed);
}


QString SensitivityAnalysisWizard::getAnalysisName() const
{
  return analysis_.getName().c_str();
}


void SensitivityAnalysisWizard::validate()
{
  otStudy_->add(analysis_);
}
}
