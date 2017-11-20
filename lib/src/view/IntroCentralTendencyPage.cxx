//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the method of central tendency analysis
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
#include "otgui/IntroCentralTendencyPage.hxx"

#include "otgui/CentralTendencyWizard.hxx"
#include "otgui/TaylorExpansionMomentsAnalysis.hxx"

#include <QVBoxLayout>
#include <QRadioButton>

using namespace OT;

namespace OTGUI
{

IntroCentralTendencyPage::IntroCentralTendencyPage(QWidget* parent)
  : QWizardPage(parent)
  , outputsSelectionGroupBox_(0)
  , methodGroup_(0)
  , errorMessageLabel_(0)
  , interestVariables_()
{
  setTitle(tr("Central tendency methods"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  // output selection
  outputsSelectionGroupBox_ = new OutputsSelectionGroupBox(this);
  pageLayout->addWidget(outputsSelectionGroupBox_);

  // choose the method
  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  methodGroup_ = new QButtonGroup(this);
  connect(methodGroup_, SIGNAL(buttonClicked(int)), this, SLOT(updateFinalPage()));

  // Chaos
  QRadioButton * buttonToChooseMethodMC = new QRadioButton(tr("Monte Carlo"));
  buttonToChooseMethodMC->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethodMC, IntroCentralTendencyPage::MonteCarlo);
  methodLayout->addWidget(buttonToChooseMethodMC);

  // Kriging
  QRadioButton * buttonToChooseMethodTaylor = new QRadioButton(tr("Taylor expansion"));
  methodGroup_->addButton(buttonToChooseMethodTaylor, IntroCentralTendencyPage::TaylorExpansionMoments);
  methodLayout->addWidget(buttonToChooseMethodTaylor);

  pageLayout->addWidget(methodBox);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);

  pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);
}


void IntroCentralTendencyPage::initialize(const Analysis& analysis)
{
  if (!dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get()))
    return;

  // method
  const String analysisName = analysis.getImplementation()->getClassName();

  if (analysisName == "MonteCarloAnalysis")
    methodGroup_->button(IntroCentralTendencyPage::MonteCarlo)->click();
  else if (analysisName == "TaylorExpansionMomentsAnalysis")
    methodGroup_->button(IntroCentralTendencyPage::TaylorExpansionMoments)->click();

  // update outputs list
  PhysicalModel model = dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get())->getPhysicalModel();
  outputsSelectionGroupBox_->updateComboBoxModel(model.getSelectedOutputsNames(), analysis.getImplementation()->getInterestVariables());
}


int IntroCentralTendencyPage::nextId() const
{
  if (methodGroup_->checkedId() == IntroCentralTendencyPage::MonteCarlo)
    return CentralTendencyWizard::Page_MonteCarlo;

  return -1;
}


void IntroCentralTendencyPage::updateFinalPage()
{
  setFinalPage(methodGroup_->checkedId() != IntroCentralTendencyPage::MonteCarlo);
}


Description IntroCentralTendencyPage::getInterestVariables() const
{
  const QStringList outputsList = outputsSelectionGroupBox_->getSelectedOutputsNames();

  Description outputNames(outputsList.size());
  for (int i = 0; i < outputsList.size(); ++i)
    outputNames[i] = outputsList[i].toUtf8().constData();

  return outputNames;
}


bool IntroCentralTendencyPage::validatePage()
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
