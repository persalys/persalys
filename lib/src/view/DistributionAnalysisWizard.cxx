// DistributionAnalysisWizard.cxx

#include "DistributionAnalysisWizard.hxx"

#include "MonteCarloCalculus.hxx"
#include "QuadraticCumulCalculus.hxx"

#include <QTableView>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>
#include <QToolBox>
#include <QToolButton>

#include <limits>

namespace OTGUI {

DistributionAnalysisWizard::DistributionAnalysisWizard(OTStudy * study, const PhysicalModel & physicalModel)
 : QWizard()
 , calculus_(MonteCarloCalculus("aNameFromFirstPage", physicalModel))
 , OTStudy_(study)
 , physicalModel_(physicalModel)
{
  buildInterface();
}


DistributionAnalysisWizard::DistributionAnalysisWizard(const Calculus & calculus)
 : QWizard()
 , calculus_(calculus)
 , physicalModel_(calculus_.getPhysicalModel())
{
  buildInterface();
}


void DistributionAnalysisWizard::buildInterface()
{
  setWindowTitle("Distribution analysis");

  // First Page: model

  // Second Page: methods
  QWizardPage * page = new QWizardPage(this);

  QVBoxLayout * mainLayout = new QVBoxLayout(page);

  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout;

  methodGroup_ = new QButtonGroup;
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Monte Carlo"));
  if (calculus_.getImplementation()->getClassName() == "MonteCarloCalculus")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, DistributionAnalysisWizard::MonteCarlo);
  methodLayout->addWidget(buttonToChooseMethod);
  buttonToChooseMethod = new QRadioButton(tr("Taylor"));
  if (calculus_.getImplementation()->getClassName() == "QuadraticCumulCalculus")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, DistributionAnalysisWizard::Taylor);
  methodLayout->addWidget(buttonToChooseMethod);
  connect(methodGroup_, SIGNAL(buttonClicked(int)), this, SLOT(updateMethodWidgets()));
//   connect(methodGroup_, SIGNAL(buttonClicked(int)), this, SIGNAL(methodChanged(int)));

  methodBox->setLayout(methodLayout);
  mainLayout->addWidget(methodBox);

  methodParametersLayout_ = new QVBoxLayout;

  /// monte carlo widgets
  monteCarloWidget_ = new QWidget;
  QGridLayout *mclayout = new QGridLayout(monteCarloWidget_);

  QLabel * nbSimuLabel = new QLabel(tr("Number of simulations"));
  nbSimuSpin_ = new QSpinBox;
  nbSimuSpin_->setMinimum(2);
  nbSimuSpin_->setMaximum(std::numeric_limits<int>::max());
  if (calculus_.getImplementation()->getClassName() == "MonteCarloCalculus")
    nbSimuSpin_->setValue(dynamic_cast<MonteCarloCalculus*>(&*calculus_.getImplementation())->getNbSimulations());
  connect(nbSimuSpin_, SIGNAL(valueChanged(int)), this, SLOT(nbSimuChanged(int)));

  nbSimuLabel->setBuddy(nbSimuSpin_);
  mclayout->addWidget(nbSimuLabel, 0, 0);
  mclayout->addWidget(nbSimuSpin_, 0, 1);

  //// advanced parameters
  // TODO make a class advancedParametersGroupBox?
  QGroupBox * advancedGroup = new QGroupBox(tr("Advanced parameters"));
  QVBoxLayout * advancedGroupLayout = new QVBoxLayout(advancedGroup);
  advancedGroup->setCheckable(true);
  advancedGroup->setChecked(false);
  advancedGroup->setStyleSheet("QGroupBox::indicator::unchecked {image: url(:/images/down_arrow.png);}\
                                QGroupBox::indicator::checked {image: url(:/images/up_arrow.png);}");

  advancedWidgets_ = new QWidget;
  QGridLayout * advancedWidgetsLayout = new QGridLayout(advancedWidgets_);

  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedWidgetsLayout->addWidget(seedLabel, 2, 0);
  seedSpin_ = new QSpinBox;
  seedLabel->setBuddy(seedSpin_);
  advancedWidgetsLayout->addWidget(seedSpin_, 2, 1);

  advancedWidgets_->hide();
  advancedGroupLayout->addWidget(advancedWidgets_);

  mclayout->addWidget(advancedGroup, 2, 0, 1, -1);

  connect(advancedGroup, SIGNAL(toggled(bool)), this, SLOT(showHideAdvancedWidgets(bool)));

  methodParametersLayout_->addWidget(monteCarloWidget_);

  /// taylor widgets
  taylorWidget_ = new QWidget;
  QVBoxLayout * taylorLayout = new QVBoxLayout;
  taylorWidget_->setLayout(taylorLayout);
  methodParametersLayout_->addWidget(taylorWidget_);

  methodParametersLayout_->addStretch();
  mainLayout->addLayout(methodParametersLayout_);

  addPage(page);
}


void DistributionAnalysisWizard::updateMethodWidgets()
{
  switch (DistributionAnalysisWizard::Method(methodGroup_->checkedId()))
  {
    case DistributionAnalysisWizard::MonteCarlo:
    {
      monteCarloWidget_->show();
      taylorWidget_->hide();
      if (calculus_.getImplementation()->getClassName() == "QuadraticCumulCalculus")
        calculus_ = MonteCarloCalculus("aNameMC", physicalModel_);
      nbSimuSpin_->setValue(dynamic_cast<MonteCarloCalculus*>(&*calculus_.getImplementation())->getNbSimulations());
      break;
    }
    case DistributionAnalysisWizard::Taylor:
    {
      monteCarloWidget_->hide();
      taylorWidget_->show();
      if (calculus_.getImplementation()->getClassName() == "MonteCarloCalculus")
        calculus_ = QuadraticCumulCalculus("aNameTaylor", physicalModel_);
      break;
    }
    default:
      break;
  }
}


void DistributionAnalysisWizard::showHideAdvancedWidgets(bool show)
{
  if (show)
    advancedWidgets_->show();
  else
    advancedWidgets_->hide();

}


void DistributionAnalysisWizard::nbSimuChanged(int nbSimu)
{
  dynamic_cast<MonteCarloCalculus*>(&*calculus_.getImplementation())->setNbSimulations(nbSimu);
}


void DistributionAnalysisWizard::setCalculus(Calculus & calculus)
{
  calculus_ = calculus;
}


void DistributionAnalysisWizard::validate()
{
  OTStudy_->addCalculus(calculus_);
  calculus_.run();
}


}