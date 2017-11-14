//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the method of sensitivity analysis
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
#include "otgui/IntroSensitivityPage.hxx"

#include "otgui/SensitivityAnalysisWizard.hxx"
#include "otgui/PhysicalModelAnalysis.hxx"
#include "otgui/QtTools.hxx"

#include <QVBoxLayout>
#include <QRadioButton>

using namespace OT;

namespace OTGUI
{

IntroSensitivityPage::IntroSensitivityPage(QWidget* parent)
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

  methodGroup_ = new QButtonGroup;

  // Sobol
  QRadioButton * buttonToChooseMethodSobol = new QRadioButton(tr("Sobol"));
  buttonToChooseMethodSobol->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethodSobol, IntroSensitivityPage::Sobol);
  methodLayout->addWidget(buttonToChooseMethodSobol);

  // SRC
  QRadioButton * buttonToChooseMethodSRC = new QRadioButton(tr("Standard Regression Coefficient (SRC)"));
  methodGroup_->addButton(buttonToChooseMethodSRC, IntroSensitivityPage::SRC);
  methodLayout->addWidget(buttonToChooseMethodSRC);

  pageLayout->addWidget(methodBox);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);

  pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);
}


void IntroSensitivityPage::initialize(const Analysis& analysis)
{
  if (!dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get()))
    return;

  // method
  const String analysisName = analysis.getImplementation()->getClassName();

  if (analysisName == "SobolAnalysis")
    methodGroup_->button(IntroSensitivityPage::Sobol)->click();
  else if (analysisName == "SRCAnalysis")
    methodGroup_->button(IntroSensitivityPage::SRC)->click();

  // update outputs list
  PhysicalModel model = dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get())->getPhysicalModel();
  outputsSelectionGroupBox_->updateComboBoxModel(model.getSelectedOutputsNames(), analysis.getImplementation()->getInterestVariables());
}


int IntroSensitivityPage::nextId() const
{
  if (methodGroup_->checkedId() == IntroSensitivityPage::Sobol)
    return SensitivityAnalysisWizard::Page_Sobol;
  else if (methodGroup_->checkedId() == IntroSensitivityPage::SRC)
    return SensitivityAnalysisWizard::Page_SRC;

  return -1;
}


Description IntroSensitivityPage::getInterestVariables() const
{
  return QtOT::StringListToDescription(outputsSelectionGroupBox_->getSelectedOutputsNames());
}


bool IntroSensitivityPage::validatePage()
{
  errorMessageLabel_->setText("");

  if (!outputsSelectionGroupBox_->getSelectedOutputsNames().size())
  {
    errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(tr("At least one output must be selected")));
    return false;
  }
  return QWizardPage::validatePage();
}
}
