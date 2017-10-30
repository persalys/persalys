//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define sobol analysis
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
#include "otgui/SobolPage.hxx"

#include "otgui/SobolAnalysis.hxx"
#include "otgui/CollapsibleGroupBox.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI
{

SobolPage::SobolPage(QWidget* parent)
  : QWizardPage(parent)
  , numberStochasticVariables_(0)
  , stopCriteriaGroupBox_(0)
  , blockSizeGroupBox_(0)
  , totalNbSimuLabel_(0)
  , bootstrapSizeSpinBox_(0)
  , confidenceLevelSpinbox_(0)
  , seedSpinbox_(0)
  , errorMessageLabel_(0)
{
  buildInterface();
}


void SobolPage::buildInterface()
{
  setTitle(tr("Sobol parameters"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  /// simulation widgets

  // stop criteria
  stopCriteriaGroupBox_ = new StopCriteriaGroupBox;
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCoefficientOfVariationChanged(double)), this, SLOT(clearErrorMessageLabel()));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiTimeChanged(int)), this, SLOT(clearErrorMessageLabel()));
  connect(stopCriteriaGroupBox_, SIGNAL(maxiCallsChanged(double)), this, SLOT(clearErrorMessageLabel()));
  pageLayout->addWidget(stopCriteriaGroupBox_);

  // block size
  blockSizeGroupBox_ = new BlockSizeGroupBox(tr("Evaluation parameter"));
  connect(blockSizeGroupBox_, SIGNAL(blockSizeChanged(double)), this, SLOT(updateNumberSimulations(double)));
  pageLayout->addWidget(blockSizeGroupBox_);

  // nb simu
  QHBoxLayout * nbSimuLayout = new QHBoxLayout;
  QLabel * totalNbSimuTitleLabel = new QLabel(tr("Number of calls by iteration"));
  totalNbSimuTitleLabel->setToolTip(tr("= block_size * ( number_of_inputs + 2 )"));
  nbSimuLayout->addWidget(totalNbSimuTitleLabel);

  // total nb simu/iteration: N=blockSize*(nb_inputs+2)
  totalNbSimuLabel_ = new QLabel;
  nbSimuLayout->addWidget(totalNbSimuLabel_);

  pageLayout->addLayout(nbSimuLayout);

  //// advanced parameters
  CollapsibleGroupBox * advancedParamGroupBox = new CollapsibleGroupBox;
  advancedParamGroupBox->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedWidgetsLayout = new QGridLayout(advancedParamGroupBox);

  // bootstrap size
  advancedWidgetsLayout->addWidget(new QLabel(tr("Bootstrap sampling size")), 0, 0);
  bootstrapSizeSpinBox_ = new UIntSpinBox;
  advancedWidgetsLayout->addWidget(bootstrapSizeSpinBox_, 0, 1);

  // confidence interval level
  advancedWidgetsLayout->addWidget(new QLabel(tr("Confidence level")), 1, 0);
  confidenceLevelSpinbox_ = new DoubleSpinBox;
  confidenceLevelSpinbox_->setRange(0.0, 0.99);
  confidenceLevelSpinbox_->setSingleStep(0.01);
  advancedWidgetsLayout->addWidget(confidenceLevelSpinbox_, 1, 1);

  // seed
  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedWidgetsLayout->addWidget(seedLabel, 2, 0);

  seedSpinbox_ = new QSpinBox;
  seedSpinbox_->setMaximum(std::numeric_limits<int>::max());
  seedLabel->setBuddy(seedSpinbox_);
  advancedWidgetsLayout->addWidget(seedSpinbox_, 2, 1);

  pageLayout->addWidget(advancedParamGroupBox);

  pageLayout->addStretch();

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  pageLayout->addWidget(errorMessageLabel_);

  initialize(SobolAnalysis());
}


void SobolPage::initialize(const Analysis& analysis)
{
  const SobolAnalysis * analysis_ptr = dynamic_cast<const SobolAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  numberStochasticVariables_ = analysis_ptr->getPhysicalModel().getStochasticInputNames().getSize() + 2;
  stopCriteriaGroupBox_->setMaximumCoefficientOfVariation(analysis_ptr->getMaximumCoefficientOfVariation());
  stopCriteriaGroupBox_->setMaximumElapsedTime(analysis_ptr->getMaximumElapsedTime());
  stopCriteriaGroupBox_->setMaximumCalls(analysis_ptr->getMaximumCalls());
  blockSizeGroupBox_->setBlockSizeValue(analysis_ptr->getBlockSize());
  bootstrapSizeSpinBox_->setValue(analysis_ptr->getBootstrapSize());
  confidenceLevelSpinbox_->setValue(analysis_ptr->getBootstrapConfidenceLevel());
  seedSpinbox_->setValue(analysis_ptr->getSeed());

  updateNumberSimulations(analysis_ptr->getBlockSize());
}


void SobolPage::updateNumberSimulations(double blockSize)
{
  errorMessageLabel_->setText("");
  // total nb simu/iteration: N=blockSize*(nb_inputs+2)
  const int nbSimu = blockSize * numberStochasticVariables_;
  totalNbSimuLabel_->setText(QString::number(nbSimu));
}


Analysis SobolPage::getAnalysis(const String& name, const PhysicalModel& physicalModel) const
{
  SobolAnalysis analysis(name, physicalModel);
  analysis.setMaximumCalls(stopCriteriaGroupBox_->getMaximumCalls());
  analysis.setMaximumCoefficientOfVariation(stopCriteriaGroupBox_->getMaximumCoefficientOfVariation());
  analysis.setMaximumElapsedTime(stopCriteriaGroupBox_->getMaximumElapsedTime());
  analysis.setBlockSize(blockSizeGroupBox_->getBlockSizeValue());
  analysis.setBootstrapSize(bootstrapSizeSpinBox_->value());
  analysis.setBootstrapConfidenceLevel(confidenceLevelSpinbox_->value());
  analysis.setSeed(seedSpinbox_->value());

  return analysis;
}


int SobolPage::nextId() const
{
  return -1;
}


void SobolPage::clearErrorMessageLabel()
{
  // the slot clear() of QLabel does not work...
  errorMessageLabel_->setText("");
}


bool SobolPage::validatePage()
{
  QString errorMessage;

  if (!stopCriteriaGroupBox_->isValid())
    errorMessage = tr("Please select at least one stop criteria");
  else
  {
    if (!stopCriteriaGroupBox_->isMaxElapsedTimeValid())
      errorMessage = tr("The maximum time must not be null");
    if (stopCriteriaGroupBox_->isMaxCallsRequired())
      if (stopCriteriaGroupBox_->getMaximumCalls() < (blockSizeGroupBox_->getBlockSizeValue()*numberStochasticVariables_))
        errorMessage = tr("The maximum calls can not be inferior to: block_size*(number_of_inputs + 2)");
  }

  errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(errorMessage));
  if (!errorMessage.isEmpty())
    return false;

  return true;
}
}
