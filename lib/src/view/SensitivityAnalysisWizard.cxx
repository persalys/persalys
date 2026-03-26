//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a SensitivityAnalysis
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/SensitivityAnalysisWizard.hxx"

#include "persalys/PhysicalModelAnalysis.hxx"
#include "persalys/QtTools.hxx"

#include <QVBoxLayout>
#include <QRadioButton>

using namespace OT;

namespace PERSALYS
{

  SensitivityIntroPage::SensitivityIntroPage(QWidget* parent)
  : QWizardPage(parent)
{
  setTitle(tr("Outputs selection"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // output selection
  outputsSelectionGroupBox_ = new OutputsSelectionGroupBox(this);
  pageLayout->addWidget(outputsSelectionGroupBox_);

  // error message
  errorWidget_ = new ErrorWidget;
  connect(outputsSelectionGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), errorWidget_, SLOT(reset()));

  pageLayout->addStretch();
  pageLayout->addWidget(errorWidget_);
}


void SensitivityIntroPage::initialize(const Analysis& analysis)
{
  if (!dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get()))
    return;

  // update outputs list
  PhysicalModel model = dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get())->getPhysicalModel();
  outputsSelectionGroupBox_->updateComboBoxModel(model.getSelectedOutputsNames(), analysis.getImplementation()->getInterestVariables());
}


int SensitivityIntroPage::nextId() const
{
  return SensitivityAnalysisWizard::Page_Sobol;
}


Description SensitivityIntroPage::getInterestVariables() const
{
  return QtOT::StringListToDescription(outputsSelectionGroupBox_->getSelectedOutputsNames());
}

bool SensitivityIntroPage::validatePage()
{
  if (!outputsSelectionGroupBox_->getSelectedOutputsNames().size())
  {
    errorWidget_->setFramelessErrorMessage(tr("At least one output must be selected"));
    return false;
  }
  return QWizardPage::validatePage();
}

SensitivityAnalysisWizard::SensitivityAnalysisWizard(const Analysis& analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
{
  buildInterface();
}


void SensitivityAnalysisWizard::buildInterface()
{
  setWindowTitle(tr("Sensitivity analysis"));
  docLink_ = "user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#sensitivitywizard";

  // First Page: choose the method
  introPage_ = new SensitivityIntroPage(this);
  introPage_->initialize(analysis_);
  setPage(Page_Intro, introPage_);

  // Second Page: sobol page
  sobolPage_ = new SobolPage(this);
  sobolPage_->initialize(analysis_);
  setPage(Page_Sobol, sobolPage_);

  setStartId(Page_Intro);
}


int SensitivityAnalysisWizard::nextId() const
{
 return currentId() == Page_Intro ? Page_Sobol : -1;
}


Analysis SensitivityAnalysisWizard::getAnalysis() const
{
  // get the physical model
  PhysicalModel model = dynamic_cast<const PhysicalModelAnalysis*>(analysis_.getImplementation().get())->getPhysicalModel();

  Analysis analysis = sobolPage_->getAnalysis(analysis_.getName(), model);
  analysis.getImplementation()->setInterestVariables(introPage_->getInterestVariables());
  return analysis;
}
}
