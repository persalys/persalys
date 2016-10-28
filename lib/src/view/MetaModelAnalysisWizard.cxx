//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a metamodel analysis
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
#include "otgui/MetaModelAnalysisWizard.hxx"

#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QButtonGroup>
#include <QCheckBox>

using namespace OT;

namespace OTGUI {

MetaModelAnalysisWizard::MetaModelAnalysisWizard(const OTStudy& otStudy, const DesignOfExperiment & designOfExperiment)
  : AnalysisWizard(FunctionalChaosAnalysis(otStudy.getAvailableAnalysisName("chaos_"), designOfExperiment))
  , chaos_(*dynamic_cast<FunctionalChaosAnalysis*>(&*analysis_.getImplementation()))
  , errorMessageLabel_(new QLabel)
{
  buildInterface();
}


MetaModelAnalysisWizard::MetaModelAnalysisWizard(const Analysis & analysis)
  : AnalysisWizard(analysis)
  , errorMessageLabel_(new QLabel)
{
  if (analysis.getImplementation()->getClassName() == "FunctionalChaosAnalysis")
  {
    chaos_ = *dynamic_cast<const FunctionalChaosAnalysis*>(&*analysis.getImplementation());
  }
//   else // Kriging
//   {
// 
//   }

  buildInterface();
}


void MetaModelAnalysisWizard::buildInterface()
{
  setWindowTitle(tr("Metamodel"));

  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * mainLayout = new QVBoxLayout(page);

  // output selection
  outputsGroupBox_ = new OutputsSelectionGroupBox(chaos_.getDesignOfExperiment().getOutputSample().getDescription(), chaos_.getOutputsToAnalyse(), this);
  setOutputsToAnalyse(outputsGroupBox_->getSelectedOutputsNames());
  connect(outputsGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), this, SLOT(setOutputsToAnalyse(QStringList)));
  mainLayout->addWidget(outputsGroupBox_);

  // method selection
  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  QButtonGroup * methodGroup = new QButtonGroup;
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Functional Chaos"));
  if (analysis_.getImplementation()->getClassName() == "FunctionalChaosAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup->addButton(buttonToChooseMethod, MetaModelAnalysisWizard::chaos);
  methodLayout->addWidget(buttonToChooseMethod);
  // TODO
//   buttonToChooseMethod = new QRadioButton(tr("Kriging"));
//   if (analysis_.getImplementation()->getClassName() == "Kriging")
//     buttonToChooseMethod->setChecked(true);
//   methodGroup->addButton(buttonToChooseMethod, MetaModelAnalysisWizard::kriging);
  methodLayout->addWidget(buttonToChooseMethod);
  connect(methodGroup, SIGNAL(buttonClicked(int)), this, SLOT(updateMethodWidgets(int)));

  mainLayout->addWidget(methodBox);

  // Chaos parameters group
  chaosParametersBox_ = new QGroupBox(tr("Parameters"));
  QHBoxLayout * chaosParametersLayout = new QHBoxLayout(chaosParametersBox_);

  // -- Polynomial degree
  const UnsignedInteger chaosDegree = chaos_.getChaosDegree();

  chaosParametersLayout->addWidget(new QLabel(tr("Polynomial degree")));
  QSpinBox * chaosDegreeSpinbox = new QSpinBox;
  chaosDegreeSpinbox->setRange(1, 30);
  chaosDegreeSpinbox->setValue(chaosDegree);
  connect(chaosDegreeSpinbox, SIGNAL(valueChanged(int)), this, SLOT(chaosDegreeChanged(int)));
  chaosParametersLayout->addWidget(chaosDegreeSpinbox);

  mainLayout->addWidget(chaosParametersBox_);

  // Metamodel validation
  QGroupBox * validationGroupBox = new QGroupBox(tr("Validation"));
  QGridLayout * validationLayout = new QGridLayout(validationGroupBox);

  // -- LOO
  QCheckBox * leaveOneOutCheckBox = new QCheckBox;
  leaveOneOutCheckBox->setChecked(chaos_.isLeaveOneOutValidation());
  validationLayout->addWidget(leaveOneOutCheckBox, 0, 0);
  validationLayout->addWidget(new QLabel(tr("Compute Q2 by Leave-one-out")), 0, 1);
  connect(leaveOneOutCheckBox, SIGNAL(toggled(bool)), this, SLOT(looValidationChanged(bool)));
  validationLayout->setColumnStretch(2, 1);
  mainLayout->addWidget(validationGroupBox);

  // functional chaos advanced parameters
  const bool sparseChaos = chaos_.getSparseChaos();

  chaosAdvancedParamGroupBox_ = new CollapsibleGroupBox;
  chaosAdvancedParamGroupBox_->setTitle(tr("Advanced parameters"));
  QGridLayout * chaosAdvancedParamGroupBox_Layout = new QGridLayout(chaosAdvancedParamGroupBox_);

  QCheckBox * sparseCheckBox_ = new QCheckBox(tr("Sparse"));
  sparseCheckBox_->setChecked(sparseChaos);
  connect(sparseCheckBox_, SIGNAL(toggled(bool)), this, SLOT(sparseChaosChanged(bool)));
  chaosAdvancedParamGroupBox_Layout->addWidget(sparseCheckBox_, 0, 0);

  chaosAdvancedParamGroupBox_->setExpanded(false);
  mainLayout->addWidget(chaosAdvancedParamGroupBox_);


  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);

  mainLayout->addStretch();
  mainLayout->addWidget(errorMessageLabel_);

  updateMethodWidgets(methodGroup->checkedId());

  addPage(page);
}


void MetaModelAnalysisWizard::updateMethodWidgets(int id)
{
  chaosParametersBox_->setVisible(id == 0);
  chaosAdvancedParamGroupBox_->setVisible(id == 0);
  // TODO
//   krigingWidgets_->setVisible(id == 1);
}


void MetaModelAnalysisWizard::chaosDegreeChanged(int degree)
{
  chaos_.setChaosDegree(degree);
}


void MetaModelAnalysisWizard::sparseChaosChanged(bool sparse)
{
  chaos_.setSparseChaos(sparse);
}


void MetaModelAnalysisWizard::looValidationChanged(bool looValidation)
{
  chaos_.setLeaveOneOutValidation(looValidation);
}


void MetaModelAnalysisWizard::setOutputsToAnalyse(QStringList outputsList)
{
  errorMessageLabel_->setText("");

  Description desc(outputsList.size());
  for (int i=0; i<outputsList.size(); ++i)
    desc[i] = outputsList[i].toUtf8().constData();

  try
  {
    chaos_.setOutputsToAnalyse(desc);
  }
  catch (InvalidDimensionException exception)
  {
    // check in validateCurrentPage
  }

}


bool MetaModelAnalysisWizard::validateCurrentPage()
{
  errorMessageLabel_->setText("");
  if (!outputsGroupBox_->getSelectedOutputsNames().size())
  {
    errorMessageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(tr("At least one output must be selected")).arg("</font>"));
    return false;
  }
  if (chaosParametersBox_->isVisible())
  {
    const UnsignedInteger n = chaos_.getChaosDegree() + chaos_.getDesignOfExperiment().getInputSample().getDimension();
    // compute binomial coefficient
    const UnsignedInteger minimumSize  = FunctionalChaosAnalysis::BinomialCoefficient(n, chaos_.getChaosDegree());
    if (chaos_.getDesignOfExperiment().getInputSample().getSize() < minimumSize)
    {
      QString errorMessage = tr("Design of experiment size too small : %1. It must be superior or equal to C(degree+nbInputs, degree) = %2").arg(chaos_.getDesignOfExperiment().getInputSample().getSize()).arg(minimumSize);
      errorMessageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(errorMessage).arg("</font>"));
      return false;
    }
    analysis_ = chaos_;
  }
  // TODO
//   else
//     analysis_ = kriging_;

  return QWizard::validateCurrentPage();
}
}
