// SensitivityAnalysisWizard.cxx

#include "otgui/SensitivityAnalysisWizard.hxx"

#include "otgui/SobolAnalysis.hxx"
#include "otgui/SRCAnalysis.hxx"

#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <limits>

namespace OTGUI {

SensitivityAnalysisWizard::SensitivityAnalysisWizard(OTStudy * otStudy, const PhysicalModel & physicalModel)
 : QWizard()
 , analysis_(SobolAnalysis("aNameFromFirstPage", physicalModel))
 , otStudy_(otStudy)
 , physicalModel_(physicalModel)
{
  buildInterface();
}


SensitivityAnalysisWizard::SensitivityAnalysisWizard(const Analysis & analysis)
 : QWizard()
 , analysis_(analysis)
 , physicalModel_(analysis_.getPhysicalModel())
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
  if (analysis_.getImplementation()->getClassName() == "SobolAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, SensitivityAnalysisWizard::Sobol);
  methodLayout->addWidget(buttonToChooseMethod);
  buttonToChooseMethod = new QRadioButton(tr("SRC"));
  if (analysis_.getImplementation()->getClassName() == "SRCAnalysis")
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
  nbSimuSpinbox_->setValue(dynamic_cast<SimulationAnalysis*>(&*analysis_.getImplementation())->getNbSimulations());
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
      if (analysis_.getImplementation()->getClassName() == "SRCAnalysis")
      {
        analysis_ = SobolAnalysis("aNameSobol", physicalModel_);
        emit analysisChanged(analysis_);
      }
      break;
    }
    case SensitivityAnalysisWizard::SRC:
    {
      if (analysis_.getImplementation()->getClassName() == "SobolAnalysis")
      {
        analysis_ = SRCAnalysis("aNameSRC", physicalModel_);
        emit analysisChanged(analysis_);
      }
      break;
    }
    default:
      break;
  }
}


void SensitivityAnalysisWizard::nbSimuChanged(int nbSimu)
{
  dynamic_cast<SimulationAnalysis*>(&*analysis_.getImplementation())->setNbSimulations(nbSimu);
}


void SensitivityAnalysisWizard::validate()
{
  otStudy_->addAnalysis(analysis_);
  analysis_.run();
}


}