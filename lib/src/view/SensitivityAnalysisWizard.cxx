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

using namespace OT;

namespace OTGUI {

SensitivityAnalysisWizard::SensitivityAnalysisWizard(const OTStudy& otStudy, const PhysicalModel & physicalModel, QWidget* parent)
  : AnalysisWizard(SobolAnalysis(otStudy.getAvailableAnalysisName("sensitivitySobol_"), physicalModel), parent)
  , sobolAnalysis_(*dynamic_cast<SobolAnalysis*>(&*analysis_.getImplementation()))
  , srcAnalysis_(SRCAnalysis(otStudy.getAvailableAnalysisName("sensitivitySRC_"), physicalModel))
  , errorMessageLabel_(new QLabel)
{
  buildInterface();
}


SensitivityAnalysisWizard::SensitivityAnalysisWizard(const Analysis & analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
  , errorMessageLabel_(new QLabel)
{
  if (analysis.getImplementation()->getClassName() == "SobolAnalysis")
  {
    sobolAnalysis_ = *dynamic_cast<const SobolAnalysis*>(&*analysis.getImplementation());
    srcAnalysis_ = SRCAnalysis(sobolAnalysis_.getName(), sobolAnalysis_.getPhysicalModel());
    try
    {
      srcAnalysis_.setInterestVariables(sobolAnalysis_.getInterestVariables());
    }
    catch (std::exception & ex)
    {
      // do nothing
    }
  }
  else
  {
    srcAnalysis_ = *dynamic_cast<const SRCAnalysis*>(&*analysis.getImplementation());
    sobolAnalysis_ = SobolAnalysis(srcAnalysis_.getName(), srcAnalysis_.getPhysicalModel());
    try
    {
      sobolAnalysis_.setInterestVariables(srcAnalysis_.getInterestVariables());
    }
    catch (std::exception & ex)
    {
      // do nothing
    }
  }

  buildInterface();
}


void SensitivityAnalysisWizard::buildInterface()
{
  setWindowTitle(tr("Sensitivity analysis"));

  // First Page: model

  // Second Page: methods
  QWizardPage * page = new QWizardPage(this);

  QVBoxLayout * mainLayout = new QVBoxLayout(page);

  // output selection
  outputsGroupBox_ = new OutputsSelectionGroupBox(sobolAnalysis_.getPhysicalModel().getSelectedOutputsNames(), sobolAnalysis_.getInterestVariables(), this);
  setInterestVariables(outputsGroupBox_->getSelectedOutputsNames());
  connect(outputsGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), this, SLOT(setInterestVariables(QStringList)));
  mainLayout->addWidget(outputsGroupBox_);

  // method selection
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

  // stop criteria
  const double maxCoef = sobolAnalysis_.getMaximumCoefficientOfVariation();
  const UnsignedInteger maxTime = sobolAnalysis_.getMaximumElapsedTime();
  const UnsignedInteger maxCalls = sobolAnalysis_.getMaximumCalls();
  stopCriteriaGroupBox_ = new StopCriteriaGroupBox(maxCoef, maxTime, maxCalls);
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCoefficientOfVariationChanged(double)), this, SLOT(maxiCoefficientOfVariationChanged(double)));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiTimeChanged(int)), this, SLOT(maxiTimeChanged(int)));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCallsChanged(int)), this, SLOT(maxiCallsChanged(int)));
  sobolWidgetsLayout->addWidget(stopCriteriaGroupBox_);

  // block size
  blockSizeGroupBox_ = new BlockSizeGroupBox(tr("Evaluation parameter"));
  blockSizeGroupBox_->setBlockSizeValue(sobolAnalysis_.getBlockSize());
  connect(blockSizeGroupBox_, SIGNAL(blockSizeChanged(double)), this, SLOT(blockSizeChanged(double)));
  sobolWidgetsLayout->addWidget(blockSizeGroupBox_);

  // nb simu
  QHBoxLayout * nbSimuLayout = new QHBoxLayout;
  QLabel * totalNbSimuLabel = new QLabel(tr("Number of calls by iteration"));
  totalNbSimuLabel->setToolTip(tr("= block_size * ( number_of_inputs + 2 )"));
  // total nb simu/iteration: N=blockSize*(nb_inputs+2)
  double nbSimu = blockSizeGroupBox_->getBlockSizeValue() * (sobolAnalysis_.getPhysicalModel().getStochasticInputNames().getSize() + 2);
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
  QHBoxLayout * sampleSizeLayout = new QHBoxLayout;
  QLabel * sampleSizeLabel = new QLabel(tr("Sample size"));
  QSpinBox * srcSampleSizeSpinbox = new QSpinBox;
  srcSampleSizeSpinbox->setMinimum(2);
  srcSampleSizeSpinbox->setMaximum(std::numeric_limits<int>::max());
  srcSampleSizeSpinbox->setValue(srcAnalysis_.getSimulationsNumber());
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

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  mainLayout->addStretch();
  mainLayout->addWidget(errorMessageLabel_);

  updateMethodWidgets(methodGroup->checkedId());

  addPage(page);
}


void SensitivityAnalysisWizard::updateMethodWidgets(int id)
{
  sobolWidgets_->setVisible(id == 0);
  srcWidgets_->setVisible(id == 1);
}


void SensitivityAnalysisWizard::maxiCoefficientOfVariationChanged(double maxi)
{
  sobolAnalysis_.setMaximumCoefficientOfVariation(maxi);
}


void SensitivityAnalysisWizard::maxiTimeChanged(int value)
{
  errorMessageLabel_->setText("");
  sobolAnalysis_.setMaximumElapsedTime(value);
}


void SensitivityAnalysisWizard::maxiCallsChanged(int maxi)
{
  errorMessageLabel_->setText("");
  try
  {
    sobolAnalysis_.setMaximumCalls(maxi);
  }
  catch (InvalidValueException exception)
  {
    // check in validateCurrentPage
  }
}


void SensitivityAnalysisWizard::sampleSizeChanged(int sampleSize)
{
  srcAnalysis_.setSimulationsNumber(sampleSize);
}


void SensitivityAnalysisWizard::blockSizeChanged(double size)
{
  errorMessageLabel_->setText("");
  try
  {
    // total nb simu/iteration: N=blockSize*(nb_inputs+2)
    int nbSimu = size * (sobolAnalysis_.getPhysicalModel().getStochasticInputNames().getSize() + 2);
    totalNbSimuLabel_->setText(QString::number(nbSimu));
    sobolAnalysis_.setBlockSize(size);
  }
  catch (InvalidValueException exception)
  {
    // check in validateCurrentPage
  }
}


void SensitivityAnalysisWizard::seedChanged(int seed)
{
  QSpinBox * spinbox = qobject_cast<QSpinBox*>(sender());
  if (spinbox->property("type").toString() == "Sobol")
    sobolAnalysis_.setSeed(seed);
  else
    srcAnalysis_.setSeed(seed);
}


void SensitivityAnalysisWizard::setInterestVariables(QStringList outputsList)
{
  errorMessageLabel_->setText("");

  Description desc(outputsList.size());
  for (int i=0; i<outputsList.size(); ++i)
    desc[i] = outputsList[i].toUtf8().constData();

  try
  {
    sobolAnalysis_.setInterestVariables(desc);
    srcAnalysis_.setInterestVariables(desc);
  }
  catch (InvalidDimensionException exception)
  {
    // check in validateCurrentPage
  }
}


bool SensitivityAnalysisWizard::validateCurrentPage()
{
  errorMessageLabel_->setText("");
  if (!outputsGroupBox_->getSelectedOutputsNames().size())
  {
    errorMessageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(tr("At least one output must be selected")).arg("</font>"));
    return false;
  }
  if (sobolWidgets_->isVisible())
  {
    QString errorMessage = "";
    if (!stopCriteriaGroupBox_->isValid())
      errorMessage = tr("Please select at least one stop criteria");
    else
    {
      if (!stopCriteriaGroupBox_->isMaxElapsedTimeValid())
        errorMessage = tr("The maximum time must not be null");
      if (stopCriteriaGroupBox_->isMaxCallsRequired())
        if (sobolAnalysis_.getMaximumCalls() < (blockSizeGroupBox_->getBlockSizeValue()*(sobolAnalysis_.getPhysicalModel().getStochasticInputNames().getSize() + 2)))
          errorMessage = tr("The maximum calls can not be inferior to: block_size*(number_of_inputs + 2)");
    }
    errorMessageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(errorMessage).arg("</font>"));
    if (!errorMessage.isEmpty())
      return false;
    analysis_ = sobolAnalysis_;
  }
  else
    analysis_ = srcAnalysis_;

  return QWizard::validateCurrentPage();
}
}