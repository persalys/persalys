//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a SensitivityAnalysis
 *
 *  Copyright 2015-2016 EDF
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

  sobolWidgets_ = new QWidget;
  QHBoxLayout * sobolWidgetsLayout = new QHBoxLayout(sobolWidgets_);
  QLabel * totalNbSimuLabel = new QLabel(tr("The total number of simulations"));
  // total nb simu: N=n*(d+2)
  // n = nb inputs; d=sample size
  double nbSimu = physicalModel_.getInputs().getSize() * (sampleSizeSpinbox_->value() + 2);
  totalNbSimuLabel_ = new QLabel(QString::number(nbSimu));
  sobolWidgetsLayout->addWidget(totalNbSimuLabel);
  sobolWidgetsLayout->addWidget(totalNbSimuLabel_);
  mainLayout->addWidget(sobolWidgets_);
  if (analysis_.getImplementation()->getClassName() == "SRCAnalysis")
    sobolWidgets_->hide();

  // advanced parameters
  advancedGroup_ = new QGroupBox(tr("Advanced parameters"));
  QVBoxLayout * advancedGroup_Layout = new QVBoxLayout(advancedGroup_);
  advancedGroup_->setCheckable(true);
  advancedGroup_->setChecked(false);
  advancedGroup_->setStyleSheet("QGroupBox::indicator::unchecked {image: url(:/images/down_arrow.png);}\
                                 QGroupBox::indicator::checked {image: url(:/images/up_arrow.png);}");

  advancedWidgets_ = new QWidget;
  QGridLayout * advancedWidgetsLayout = new QGridLayout(advancedWidgets_);

  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedWidgetsLayout->addWidget(seedLabel, 0, 0);
  seedSpinbox_ = new QSpinBox;
  seedSpinbox_->setMaximum(std::numeric_limits<int>::max());
  seedLabel->setBuddy(seedSpinbox_);
  advancedWidgetsLayout->addWidget(seedSpinbox_, 0, 1);
  seedSpinbox_->setValue(dynamic_cast<SimulationAnalysis*>(&*analysis_.getImplementation())->getSeed());
  connect(seedSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(seedChanged(int)));

  advancedWidgets_->hide();
  advancedGroup_Layout->addWidget(advancedWidgets_);

  mainLayout->addWidget(advancedGroup_);

  connect(advancedGroup_, SIGNAL(toggled(bool)), this, SLOT(showHideAdvancedWidgets(bool)));

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
        sobolWidgets_->show();
        emit analysisChanged(analysis_);
      }
      break;
    }
    case SensitivityAnalysisWizard::SRC:
    {
      if (analysis_.getImplementation()->getClassName() == "SobolAnalysis")
      {
        analysis_ = SRCAnalysis(analysis_.getName(), physicalModel_);
        sobolWidgets_->hide();
        emit analysisChanged(analysis_);
      }
      break;
    }
    default:
      break;
  }
}


void SensitivityAnalysisWizard::showHideAdvancedWidgets(bool show)
{
  if (show)
    advancedWidgets_->show();
  else
    advancedWidgets_->hide();
}


void SensitivityAnalysisWizard::sampleSizeChanged(int sampleSize)
{
  // total nb simu: N=n*(d+2)
  // n = nb inputs; d=sample size
  int nbSimu = physicalModel_.getInputs().getSize() * (sampleSize + 2);
  totalNbSimuLabel_->setText(QString::number(nbSimu));
  dynamic_cast<SimulationAnalysis*>(&*analysis_.getImplementation())->setNbSimulations(sampleSize);
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
