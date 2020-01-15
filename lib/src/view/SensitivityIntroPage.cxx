//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the method of sensitivity analysis
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#include "persalys/SensitivityIntroPage.hxx"

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
  , outputsSelectionGroupBox_(0)
  , methodGroup_(0)
  , errorMessageLabel_(0)
{
  setTitle(tr("Sensitivity methods"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // output selection
  outputsSelectionGroupBox_ = new OutputsSelectionGroupBox(this);
  pageLayout->addWidget(outputsSelectionGroupBox_);

  // choose the method
  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  methodGroup_ = new QButtonGroup(this);

  // Sobol
  QRadioButton * buttonToChooseMethodSobol = new QRadioButton(tr("Sobol"));
  buttonToChooseMethodSobol->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethodSobol, SensitivityIntroPage::Sobol);
  methodLayout->addWidget(buttonToChooseMethodSobol);

  // SRC
  QRadioButton * buttonToChooseMethodSRC = new QRadioButton(tr("Standard Regression Coefficient (SRC)"));
  methodGroup_->addButton(buttonToChooseMethodSRC, SensitivityIntroPage::SRC);
  methodLayout->addWidget(buttonToChooseMethodSRC);

  pageLayout->addWidget(methodBox);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  connect(outputsSelectionGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), errorMessageLabel_, SLOT(reset()));

  pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);
}


void SensitivityIntroPage::initialize(const Analysis& analysis)
{
  if (!dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get()))
    return;

  // method
  const String analysisName = analysis.getImplementation()->getClassName();

  if (analysisName == "SobolAnalysis")
    methodGroup_->button(SensitivityIntroPage::Sobol)->click();
  else if (analysisName == "SRCAnalysis")
    methodGroup_->button(SensitivityIntroPage::SRC)->click();

  // update outputs list
  PhysicalModel model = dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get())->getPhysicalModel();
  outputsSelectionGroupBox_->updateComboBoxModel(model.getSelectedOutputsNames(), analysis.getImplementation()->getInterestVariables());
}


int SensitivityIntroPage::nextId() const
{
  if (methodGroup_->checkedId() == SensitivityIntroPage::Sobol)
    return SensitivityAnalysisWizard::Page_Sobol;
  else if (methodGroup_->checkedId() == SensitivityIntroPage::SRC)
    return SensitivityAnalysisWizard::Page_SRC;

  return -1;
}


Description SensitivityIntroPage::getInterestVariables() const
{
  return QtOT::StringListToDescription(outputsSelectionGroupBox_->getSelectedOutputsNames());
}


int SensitivityIntroPage::getMethodId() const
{
  return methodGroup_->checkedId();
}


bool SensitivityIntroPage::validatePage()
{
  if (!outputsSelectionGroupBox_->getSelectedOutputsNames().size())
  {
    errorMessageLabel_->setErrorMessage(tr("At least one output must be selected"));
    return false;
  }
  return QWizardPage::validatePage();
}
}
