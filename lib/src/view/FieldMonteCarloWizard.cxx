//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a central tendency analysis
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/FieldMonteCarloWizard.hxx"

#include "otgui/FieldMonteCarloAnalysis.hxx"
#include "otgui/OutputsSelectionGroupBox.hxx"
#include "otgui/CollapsibleGroupBox.hxx"
#include "otgui/StopCriteriaGroupBox.hxx"
#include "otgui/BlockSizeGroupBox.hxx"
#include "otgui/QtTools.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI
{

FieldMonteCarloWizard::FieldMonteCarloWizard(const Analysis& analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
{
  buildInterface();
}


void FieldMonteCarloWizard::buildInterface()
{
  setWindowTitle(tr("Central tendency"));
  // get data
  const FieldMonteCarloAnalysis * analysis_ptr = dynamic_cast<const FieldMonteCarloAnalysis*>(analysis_.getImplementation().get());

  const PhysicalModel model(analysis_ptr->getPhysicalModel());

  // build page
  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * pageLayout = new QVBoxLayout(page);

  // output selection
  outputsGroupBox_ = new OutputsSelectionGroupBox(model.getSelectedOutputsNames(), analysis_ptr->getInterestVariables(), this);
  pageLayout->addWidget(outputsGroupBox_, 0, Qt::AlignTop);

  // Stopping criteria
  stopCriteriaGroupBox_ = new StopCriteriaGroupBox(StopCriteriaGroupBox::Time_Calls);
  stopCriteriaGroupBox_->setMaximumElapsedTime(analysis_ptr->getMaximumElapsedTime());
  stopCriteriaGroupBox_->setMaximumCalls(analysis_ptr->getMaximumCalls());
  pageLayout->addWidget(stopCriteriaGroupBox_);

  // block size
  blockSizeGroupBox_ = new BlockSizeGroupBox(tr("Evaluation parameter"));
  blockSizeGroupBox_->setBlockSizeValue(analysis_ptr->getBlockSize());
  pageLayout->addWidget(blockSizeGroupBox_);

  //// advanced parameters
  CollapsibleGroupBox * advancedParamGroupBox = new CollapsibleGroupBox;
  advancedParamGroupBox->setTitle(tr("Advanced parameters"));
  QGridLayout * advancedWidgetsLayout = new QGridLayout(advancedParamGroupBox);

  // KL threshold
  QLabel * klLabel = new QLabel(tr("Karhunen-Loeve threshold"));
  advancedWidgetsLayout->addWidget(klLabel, 0, 0);

  klSpinbox_ = new DoubleSpinBox;
  klSpinbox_->setRange(0.0, std::numeric_limits<int>::max());
  klSpinbox_->setSingleStep(1.);
  klSpinbox_->setValue(analysis_ptr->getKarhunenLoeveThreshold());
  advancedWidgetsLayout->addWidget(klSpinbox_, 0, 1);

  // seed
  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedWidgetsLayout->addWidget(seedLabel, 1, 0);

  seedSpinbox_ = new QSpinBox;
  seedSpinbox_->setMaximum(std::numeric_limits<int>::max());
  seedSpinbox_->setValue(analysis_ptr->getSeed());
  seedLabel->setBuddy(seedSpinbox_);
  advancedWidgetsLayout->addWidget(seedSpinbox_, 1, 1);

  pageLayout->addWidget(advancedParamGroupBox);

  pageLayout->addStretch();

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  connect(outputsGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), errorMessageLabel_, SLOT(reset()));
  connect(stopCriteriaGroupBox_, SIGNAL(criteriaChanged()), errorMessageLabel_, SLOT(reset()));
  connect(blockSizeGroupBox_, SIGNAL(blockSizeChanged(double)), errorMessageLabel_, SLOT(reset()));
  pageLayout->addWidget(errorMessageLabel_, 0, Qt::AlignBottom);

  addPage(page);
}


Analysis FieldMonteCarloWizard::getAnalysis() const
{
  // get the physical model
  PhysicalModel model = dynamic_cast<const PhysicalModelAnalysis*>(analysis_.getImplementation().get())->getPhysicalModel();

  FieldMonteCarloAnalysis analysis(analysis_.getName(), model);
  analysis.setInterestVariables(QtOT::StringListToDescription(outputsGroupBox_->getSelectedOutputsNames()));
  analysis.setMaximumCalls(stopCriteriaGroupBox_->getMaximumCalls());
  analysis.setMaximumElapsedTime(stopCriteriaGroupBox_->getMaximumElapsedTime());
  analysis.setBlockSize(blockSizeGroupBox_->getBlockSizeValue());
  analysis.setKarhunenLoeveThreshold(klSpinbox_->value());
  analysis.setSeed(seedSpinbox_->value());

  return analysis;
}


bool FieldMonteCarloWizard::validateCurrentPage()
{
  QString errorMessage;

  if (!outputsGroupBox_->getSelectedOutputsNames().size())
    errorMessage = tr("At least one output must be selected");

  if (!stopCriteriaGroupBox_->isValid())
    errorMessage = tr("Select at least a stopping criterion");
  else
  {
    if (!stopCriteriaGroupBox_->isMaxElapsedTimeValid())
      errorMessage = tr("The maximum time must not be null");

    if (stopCriteriaGroupBox_->isMaxCallsRequired())
    {
      if (stopCriteriaGroupBox_->getMaximumCalls() < blockSizeGroupBox_->getBlockSizeValue())
        errorMessage = tr("The maximum calls can not be less than the block size");
    }
  }

  errorMessageLabel_->setErrorMessage(errorMessage);

  return errorMessage.isEmpty() ? QWizard::validateCurrentPage() : false;
}
}
