// SensitivityAnalysisWizard.cxx

#include "SensitivityAnalysisWizard.hxx"

#include "SobolCalculus.hxx"

#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <limits>

namespace OTGUI {

SensitivityAnalysisWizard::SensitivityAnalysisWizard(OTStudy * study, const PhysicalModel & physicalModel)
 : QWizard()
 , calculus_(SobolCalculus("aNameFromFirstPage", physicalModel))
 , OTStudy_(study)
 , physicalModel_(physicalModel)
{
  buildInterface();
}


SensitivityAnalysisWizard::SensitivityAnalysisWizard(const Calculus & calculus)
 : QWizard()
 , calculus_(calculus)
 , physicalModel_(calculus_.getPhysicalModel())
{
  buildInterface();
}


void SensitivityAnalysisWizard::buildInterface()
{
  setWindowTitle("Sensitivity analysis");

  // First Page: model

  // Second Page: methods
  QWizardPage * page = new QWizardPage(this);

  QVBoxLayout * mainLayout = new QVBoxLayout(page);

  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  methodGroup_ = new QButtonGroup;
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Sobol"));
  if (calculus_.getImplementation()->getClassName() == "SobolCalculus")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, SensitivityAnalysisWizard::Sobol);
  methodLayout->addWidget(buttonToChooseMethod);
  buttonToChooseMethod = new QRadioButton(tr("SVR"));
  if (calculus_.getImplementation()->getClassName() == "SVRCalculus")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, SensitivityAnalysisWizard::SVR);
  methodLayout->addWidget(buttonToChooseMethod);
  connect(methodGroup_, SIGNAL(buttonClicked(int)), this, SLOT(updateMethodWidgets()));

  mainLayout->addWidget(methodBox);

  QGridLayout * methodParametersLayout = new QGridLayout;

  QLabel * nbSimuLabel = new QLabel(tr("Number of simulations"));
  nbSimuSpinbox_ = new QSpinBox;
  nbSimuSpinbox_->setMinimum(2);
  nbSimuSpinbox_->setMaximum(std::numeric_limits<int>::max());
  if (calculus_.getImplementation()->getClassName() == "SobolCalculus")
    nbSimuSpinbox_->setValue(dynamic_cast<SobolCalculus*>(&*calculus_.getImplementation())->getNbSimulations());
  connect(nbSimuSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(nbSimuChanged(int)));

  nbSimuLabel->setBuddy(nbSimuSpinbox_);
  methodParametersLayout->addWidget(nbSimuLabel, 0, 0);
  methodParametersLayout->addWidget(nbSimuSpinbox_, 0, 1);

 
  mainLayout->addLayout(methodParametersLayout);

  updateMethodWidgets();

  addPage(page);
}


void SensitivityAnalysisWizard::updateMethodWidgets()
{
  switch (SensitivityAnalysisWizard::Method(methodGroup_->checkedId()))
  {
    case SensitivityAnalysisWizard::Sobol:
    {
      if (calculus_.getImplementation()->getClassName() == "SVRCalculus")
      {
        calculus_ = SobolCalculus("aNameSobol", physicalModel_);
        emit calculusChanged(calculus_);
      }
      break;
    }
    case SensitivityAnalysisWizard::SVR:
    {
//       if (calculus_.getImplementation()->getClassName() == "SobolCalculus")
//         calculus_ = SVRCalculus("aNameSVR", physicalModel_);
      break;
    }
    default:
      break;
  }
}


void SensitivityAnalysisWizard::nbSimuChanged(int nbSimu)
{
  switch (SensitivityAnalysisWizard::Method(methodGroup_->checkedId()))
  {
    case SensitivityAnalysisWizard::Sobol:
    {
      dynamic_cast<SobolCalculus*>(&*calculus_.getImplementation())->setNbSimulations(nbSimu);
      break;
    }
    case SensitivityAnalysisWizard::SVR:
    {
//       dynamic_cast<SSVRCalculus*>(&*calculus_.getImplementation())->setNbSimulations(nbSimu);
      break;
    }
    default:
      break;
  }
}


void SensitivityAnalysisWizard::validate()
{
  OTStudy_->addCalculus(calculus_);
  calculus_.run();
}


}