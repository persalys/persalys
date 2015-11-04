// SensitivityAnalysisWizard.cxx

#include "otgui/SensitivityAnalysisWizard.hxx"

#include "otgui/SobolCalculus.hxx"
#include "otgui/SRCCalculus.hxx"

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
  buttonToChooseMethod = new QRadioButton(tr("SRC"));
  if (calculus_.getImplementation()->getClassName() == "SRCCalculus")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, SensitivityAnalysisWizard::SRC);
  methodLayout->addWidget(buttonToChooseMethod);
  connect(methodGroup_, SIGNAL(buttonClicked(int)), this, SLOT(updateMethodWidgets()));

  mainLayout->addWidget(methodBox);

  QGridLayout * methodParametersLayout = new QGridLayout;

  QLabel * nbSimuLabel = new QLabel(tr("Number of simulations"));
  nbSimuSpinbox_ = new QSpinBox;
  nbSimuSpinbox_->setMinimum(2);
  nbSimuSpinbox_->setMaximum(std::numeric_limits<int>::max());
  nbSimuSpinbox_->setValue(dynamic_cast<SimulationCalculus*>(&*calculus_.getImplementation())->getNbSimulations());
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
      if (calculus_.getImplementation()->getClassName() == "SRCCalculus")
      {
        calculus_ = SobolCalculus("aNameSobol", physicalModel_);
        emit calculusChanged(calculus_);
      }
      break;
    }
    case SensitivityAnalysisWizard::SRC:
    {
      if (calculus_.getImplementation()->getClassName() == "SobolCalculus")
      {
        calculus_ = SRCCalculus("aNameSRC", physicalModel_);
        emit calculusChanged(calculus_);
      }
      break;
    }
    default:
      break;
  }
}


void SensitivityAnalysisWizard::nbSimuChanged(int nbSimu)
{
  dynamic_cast<SimulationCalculus*>(&*calculus_.getImplementation())->setNbSimulations(nbSimu);
//   switch (SensitivityAnalysisWizard::Method(methodGroup_->checkedId()))
//   {
//     case SensitivityAnalysisWizard::Sobol:
//     {
//       dynamic_cast<SobolCalculus*>(&*calculus_.getImplementation())->setNbSimulations(nbSimu);
//       break;
//     }
//     case SensitivityAnalysisWizard::SRC:
//     {
//       dynamic_cast<SRCCalculus*>(&*calculus_.getImplementation())->setNbSimulations(nbSimu);
//       break;
//     }
//     default:
//       break;
//   }
}


void SensitivityAnalysisWizard::validate()
{
  OTStudy_->addCalculus(calculus_);
  calculus_.run();
}


}