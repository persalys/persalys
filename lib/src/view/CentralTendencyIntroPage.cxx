//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the method of central tendency analysis
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/CentralTendencyIntroPage.hxx"

#include "persalys/CentralTendencyWizard.hxx"
#include "persalys/TaylorExpansionMomentsAnalysis.hxx"

#include <QVBoxLayout>
#include <QRadioButton>

using namespace OT;

namespace PERSALYS
{

CentralTendencyIntroPage::CentralTendencyIntroPage(QWidget* parent)
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
  connect(methodGroup_, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(updateFinalPage()));

  // Chaos
  QRadioButton * buttonToChooseMethodMC = new QRadioButton(tr("Monte Carlo"));
  buttonToChooseMethodMC->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethodMC, CentralTendencyIntroPage::MonteCarlo);
  methodLayout->addWidget(buttonToChooseMethodMC);

  // Kriging
  QRadioButton * buttonToChooseMethodTaylor = new QRadioButton(tr("Taylor expansion"));
  methodGroup_->addButton(buttonToChooseMethodTaylor, CentralTendencyIntroPage::TaylorExpansionMoments);
  methodLayout->addWidget(buttonToChooseMethodTaylor);

  pageLayout->addWidget(methodBox);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  connect(outputsSelectionGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), errorMessageLabel_, SLOT(reset()));

  pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);
}


void CentralTendencyIntroPage::initialize(const Analysis& analysis)
{
  if (!dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get()))
    return;

  // method
  const String analysisName = analysis.getImplementation()->getClassName();

  if (analysisName == "MonteCarloAnalysis")
    methodGroup_->button(CentralTendencyIntroPage::MonteCarlo)->click();
  else if (analysisName == "TaylorExpansionMomentsAnalysis")
    methodGroup_->button(CentralTendencyIntroPage::TaylorExpansionMoments)->click();

  // update outputs list
  PhysicalModel model = dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get())->getPhysicalModel();
  outputsSelectionGroupBox_->updateComboBoxModel(model.getSelectedOutputsNames(), analysis.getImplementation()->getInterestVariables());
}


int CentralTendencyIntroPage::nextId() const
{
  if (methodGroup_->checkedId() == CentralTendencyIntroPage::MonteCarlo)
    return CentralTendencyWizard::Page_MonteCarlo;

  return -1;
}


void CentralTendencyIntroPage::updateFinalPage()
{
  setFinalPage(methodGroup_->checkedId() != CentralTendencyIntroPage::MonteCarlo);
}


Description CentralTendencyIntroPage::getInterestVariables() const
{
  const QStringList outputsList = outputsSelectionGroupBox_->getSelectedOutputsNames();

  Description outputNames(outputsList.size());
  for (int i = 0; i < outputsList.size(); ++i)
    outputNames[i] = outputsList[i].toUtf8().constData();

  return outputNames;
}


int CentralTendencyIntroPage::getMethodId() const
{
  return methodGroup_->checkedId();
}


bool CentralTendencyIntroPage::validatePage()
{
  if (!outputsSelectionGroupBox_->getSelectedOutputsNames().size())
  {
    errorMessageLabel_->setErrorMessage(tr("At least one output must be selected"));
    return false;
  }
  return QWizardPage::validatePage();
}
}
