//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define sobol analysis
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/SobolPage.hxx"

#include "persalys/SobolAnalysis.hxx"
#include "persalys/CollapsibleGroupBox.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace PERSALYS
{

SobolPage::SobolPage(QWidget* parent)
  : QWizardPage(parent)
  , numberStochasticVariables_(0)
  , stopCriteriaGroupBox_(0)
  , blockSizeGroupBox_(0)
  , totalNbSimuLabel_(0)
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

  // Stopping criteria
  stopCriteriaGroupBox_ = new StopCriteriaGroupBox(StopCriteriaGroupBox::Time_Calls_CILength);
  pageLayout->addWidget(stopCriteriaGroupBox_);

  // block size
  blockSizeGroupBox_ = new BlockSizeGroupBox(tr("Evaluation parameters"), true);
  connect(blockSizeGroupBox_, SIGNAL(replicationSizeChanged(double)), this, SLOT(updateNumberSimulations(double)));
  pageLayout->addWidget(blockSizeGroupBox_);

  // nb simu
  QHBoxLayout * nbSimuLayout = new QHBoxLayout;
  QLabel * totalNbSimuTitleLabel = new QLabel(tr("Number of calls by iteration"));
  totalNbSimuTitleLabel->setToolTip(tr("= replication_size * ( number_of_inputs + 2 )"));
  nbSimuLayout->addWidget(totalNbSimuTitleLabel);

  // total nb simu/iteration: N=blockSize*(nb_inputs+2)
  totalNbSimuLabel_ = new QLabel;
  nbSimuLayout->addWidget(totalNbSimuLabel_);

  pageLayout->addLayout(nbSimuLayout);

  //// advanced parameters
  CollapsibleGroupBox * advancedParamGroupBox = new CollapsibleGroupBox;
  advancedParamGroupBox->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedWidgetsLayout = new QGridLayout(advancedParamGroupBox);

  // confidence interval level
  advancedWidgetsLayout->addWidget(new QLabel(tr("Confidence level")), 0, 0);
  confidenceLevelSpinbox_ = new DoubleSpinBox;
  confidenceLevelSpinbox_->setRange(0.0, 0.99);
  confidenceLevelSpinbox_->setSingleStep(0.01);
  advancedWidgetsLayout->addWidget(confidenceLevelSpinbox_, 0, 1);

  // seed
  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedWidgetsLayout->addWidget(seedLabel, 1, 0);

  seedSpinbox_ = new QSpinBox;
  seedSpinbox_->setMaximum(std::numeric_limits<int>::max());
  seedLabel->setBuddy(seedSpinbox_);
  advancedWidgetsLayout->addWidget(seedSpinbox_, 1, 1);

  pageLayout->addWidget(advancedParamGroupBox);

  pageLayout->addStretch();

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  connect(stopCriteriaGroupBox_, SIGNAL(criteriaChanged()), errorMessageLabel_, SLOT(reset()));
  connect(blockSizeGroupBox_, SIGNAL(blockSizeChanged(double)), errorMessageLabel_, SLOT(reset()));
  pageLayout->addWidget(errorMessageLabel_);

  initialize(SobolAnalysis());
}


void SobolPage::initialize(const Analysis& analysis)
{
  const SobolAnalysis * analysis_ptr = dynamic_cast<const SobolAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  numberStochasticVariables_ = analysis_ptr->getPhysicalModel().getStochasticInputNames().getSize() + 2;
  stopCriteriaGroupBox_->setMaximumConfidenceIntervalLength(analysis_ptr->getMaximumConfidenceIntervalLength());
  stopCriteriaGroupBox_->setMaximumElapsedTime(analysis_ptr->getMaximumElapsedTime());
  stopCriteriaGroupBox_->setMaximumCalls(analysis_ptr->getMaximumCalls());
  blockSizeGroupBox_->setBlockSizeValue(analysis_ptr->getBlockSize());
  blockSizeGroupBox_->setReplicationSizeValue(analysis_ptr->getReplicationSize());
  confidenceLevelSpinbox_->setValue(analysis_ptr->getConfidenceLevel());
  seedSpinbox_->setValue(analysis_ptr->getSeed());

  updateNumberSimulations(analysis_ptr->getReplicationSize());
}


void SobolPage::updateNumberSimulations(double replicationSize)
{
  errorMessageLabel_->reset();
  // total nb simu/iteration: N=replicationSize*(nb_inputs+2)
  const int nbSimu = replicationSize * numberStochasticVariables_;
  totalNbSimuLabel_->setText(QString::number(nbSimu));
}


Analysis SobolPage::getAnalysis(const String& name, const PhysicalModel& physicalModel) const
{
  SobolAnalysis analysis(name, physicalModel);
  analysis.setMaximumCalls(stopCriteriaGroupBox_->getMaximumCalls());
  analysis.setMaximumConfidenceIntervalLength(stopCriteriaGroupBox_->getMaximumConfidenceIntervalLength());
  analysis.setMaximumElapsedTime(stopCriteriaGroupBox_->getMaximumElapsedTime());
  analysis.setBlockSize(blockSizeGroupBox_->getBlockSizeValue());
  analysis.setReplicationSize(blockSizeGroupBox_->getReplicationSizeValue());
  analysis.setConfidenceLevel(confidenceLevelSpinbox_->value());
  analysis.setSeed(seedSpinbox_->value());

  return analysis;
}


int SobolPage::nextId() const
{
  return -1;
}


bool SobolPage::validatePage()
{
  QString errorMessage;

  if (!stopCriteriaGroupBox_->isValid())
    errorMessage = tr("Select at least one stopping criterion");
  else
  {
    if (!stopCriteriaGroupBox_->isMaxElapsedTimeValid())
      errorMessage = tr("The maximum time must not be null");
    if (stopCriteriaGroupBox_->isMaxCallsRequired())
    {
      const UnsignedInteger maxSize = blockSizeGroupBox_->getReplicationSizeValue() * numberStochasticVariables_;
      if (stopCriteriaGroupBox_->getMaximumCalls() < maxSize)
        errorMessage = tr("The maximum calls (%1) cannot be less than: replication_size(%2)*(number_of_inputs (%3) + 2)=%4").arg(stopCriteriaGroupBox_->getMaximumCalls()).arg(blockSizeGroupBox_->getReplicationSizeValue()).arg(numberStochasticVariables_ - 2).arg(maxSize);
    }
  }

  errorMessageLabel_->setErrorMessage(errorMessage);
  if (!errorMessage.isEmpty())
    return false;

  return true;
}
}
