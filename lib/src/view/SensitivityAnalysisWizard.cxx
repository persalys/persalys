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

#include "otgui/CollapsibleGroupBox.hxx"

#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QButtonGroup>

#include <limits>

namespace OTGUI {

SensitivityAnalysisWizard::SensitivityAnalysisWizard(OTStudy * otstudy, const PhysicalModel & physicalModel)
  : AnalysisWizard(SobolAnalysis(otstudy->getAvailableAnalysisName("sensitivitySobol_"), physicalModel))
  , sobolAnalysis_(*dynamic_cast<SobolAnalysis*>(&*analysis_.getImplementation()))
  , srcAnalysis_(SRCAnalysis(otstudy->getAvailableAnalysisName("sensitivitySRC_"), physicalModel))
{
  buildInterface();
}


SensitivityAnalysisWizard::SensitivityAnalysisWizard(const Analysis & analysis)
  : AnalysisWizard(analysis)
{
  if (analysis.getImplementation()->getClassName() == "SobolAnalysis")
  {
    sobolAnalysis_ = *dynamic_cast<const SobolAnalysis*>(&*analysis.getImplementation());
    srcAnalysis_ = SRCAnalysis(analysis.getName(), analysis.getPhysicalModel());
  }
  else
  {
    sobolAnalysis_ = SobolAnalysis(analysis.getName(), analysis.getPhysicalModel());
    srcAnalysis_ = *dynamic_cast<const SRCAnalysis*>(&*analysis.getImplementation());
  }

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

  QButtonGroup * methodGroup = new QButtonGroup;
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Sobol"));
  if (analysis_.getImplementation()->getClassName() == "SobolAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup->addButton(buttonToChooseMethod, SensitivityAnalysisWizard::Sobol);
  methodLayout->addWidget(buttonToChooseMethod);
  buttonToChooseMethod = new QRadioButton(tr("SRC"));
  if (analysis_.getImplementation()->getClassName() == "SRCAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup->addButton(buttonToChooseMethod, SensitivityAnalysisWizard::SRC);
  methodLayout->addWidget(buttonToChooseMethod);
  connect(methodGroup, SIGNAL(buttonClicked(int)), this, SLOT(updateMethodWidgets(int)));

  mainLayout->addWidget(methodBox);

  // -- Sobol widgets --
  sobolWidgets_ = new QWidget;
  QVBoxLayout * sobolWidgetsLayout = new QVBoxLayout(sobolWidgets_);

  // sample size
  QHBoxLayout * sampleSizeLayout = new QHBoxLayout;
  QLabel * sampleSizeLabel = new QLabel(tr("Sample size"));
  QSpinBox * sobolSampleSizeSpinbox = new QSpinBox;
  sobolSampleSizeSpinbox->setMinimum(2);
  sobolSampleSizeSpinbox->setMaximum(std::numeric_limits<int>::max());
  sobolSampleSizeSpinbox->setValue(sobolAnalysis_.getSimulationsNumber());
  sobolSampleSizeSpinbox->setProperty("type", "Sobol");
  connect(sobolSampleSizeSpinbox, SIGNAL(valueChanged(int)), this, SLOT(sampleSizeChanged(int)));
  sampleSizeLabel->setBuddy(sobolSampleSizeSpinbox);
  sampleSizeLayout->addWidget(sampleSizeLabel);
  sampleSizeLayout->addWidget(sobolSampleSizeSpinbox);
  sobolWidgetsLayout->addLayout(sampleSizeLayout);

  // nb simu
  QHBoxLayout * nbSimuLayout = new QHBoxLayout;
  QLabel * totalNbSimuLabel = new QLabel(tr("Total number of simulations"));
  // total nb simu: N=n*(d+2)
  // n = nb inputs; d=sample size
  double nbSimu = sobolAnalysis_.getPhysicalModel().getInputs().getSize() * (sobolSampleSizeSpinbox->value() + 2);
  totalNbSimuLabel_ = new QLabel(QString::number(nbSimu));
  nbSimuLayout->addWidget(totalNbSimuLabel);
  nbSimuLayout->addWidget(totalNbSimuLabel_);
  sobolWidgetsLayout->addLayout(nbSimuLayout);

  // block size
  blockSizeGroupBox_ = new BlockSizeGroupBox(tr("Evaluation parameter"));
  blockSizeGroupBox_->setBlockSizeValue(sobolAnalysis_.getBlockSize());
  connect(blockSizeGroupBox_, SIGNAL(blockSizeChanged(double)), this, SLOT(blockSizeChanged(double)));
  sobolWidgetsLayout->addWidget(blockSizeGroupBox_);

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
  sobolSeedSpinbox->setValue(sobolAnalysis_.getSeed());
  sobolSeedSpinbox->setProperty("type", "Sobol");
  connect(sobolSeedSpinbox, SIGNAL(valueChanged(int)), this, SLOT(seedChanged(int)));

  sobolAdvancedParamGroupBox->setExpanded(false);
  sobolWidgetsLayout->addWidget(sobolAdvancedParamGroupBox);
  mainLayout->addWidget(sobolWidgets_);

  // -- SRC widgets --
  srcWidgets_ = new QWidget;
  QVBoxLayout * srcWidgetsLayout = new QVBoxLayout(srcWidgets_);

  // sample size
  sampleSizeLayout = new QHBoxLayout;
  sampleSizeLabel = new QLabel(tr("Sample size"));
  QSpinBox * srcSampleSizeSpinbox = new QSpinBox;
  srcSampleSizeSpinbox->setMinimum(2);
  srcSampleSizeSpinbox->setMaximum(std::numeric_limits<int>::max());
  srcSampleSizeSpinbox->setValue(srcAnalysis_.getSimulationsNumber());
  srcSampleSizeSpinbox->setProperty("type", "SRC");
  connect(srcSampleSizeSpinbox, SIGNAL(valueChanged(int)), this, SLOT(sampleSizeChanged(int)));
  sampleSizeLabel->setBuddy(srcSampleSizeSpinbox);
  sampleSizeLayout->addWidget(sampleSizeLabel);
  sampleSizeLayout->addWidget(srcSampleSizeSpinbox);
  srcWidgetsLayout->addLayout(sampleSizeLayout);

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
  srcSeedSpinbox->setValue(srcAnalysis_.getSeed());
  srcSeedSpinbox->setProperty("type", "SRC");
  connect(srcSeedSpinbox, SIGNAL(valueChanged(int)), this, SLOT(seedChanged(int)));

  srcAdvancedParamGroupBox->setExpanded(false);
  srcWidgetsLayout->addWidget(srcAdvancedParamGroupBox);
  mainLayout->addWidget(srcWidgets_);

  updateMethodWidgets(methodGroup->checkedId());

  addPage(page);
}


void SensitivityAnalysisWizard::updateMethodWidgets(int id)
{
  sobolWidgets_->setVisible(id == 0);
  srcWidgets_->setVisible(id == 1);
}


void SensitivityAnalysisWizard::sampleSizeChanged(int sampleSize)
{
  QSpinBox * spinbox = qobject_cast<QSpinBox*>(sender());
  if (spinbox->property("type").toString() == "Sobol")
  {
    // total nb simu: N=n*(d+2)
    // n = nb inputs; d=sample size
    int nbSimu = sobolAnalysis_.getPhysicalModel().getInputs().getSize() * (sampleSize + 2);
    totalNbSimuLabel_->setText(QString::number(nbSimu));
    sobolAnalysis_.setSimulationsNumber(sampleSize);
  }
  else
    srcAnalysis_.setSimulationsNumber(sampleSize);
}


void SensitivityAnalysisWizard::blockSizeChanged(double size)
{
  sobolAnalysis_.setBlockSize(size);
}


void SensitivityAnalysisWizard::seedChanged(int seed)
{
  QSpinBox * spinbox = qobject_cast<QSpinBox*>(sender());
  if (spinbox->property("type").toString() == "Sobol")
    sobolAnalysis_.setSeed(seed);
  else
    srcAnalysis_.setSeed(seed);
}


void SensitivityAnalysisWizard::accept()
{
  if (sobolWidgets_->isVisible())
    analysis_ = sobolAnalysis_;
  else
    analysis_ = srcAnalysis_;
  QWizard::accept();
}
}
