// DistributionAnalysisWizard.cxx

#include "DistributionAnalysisWizard.hxx"

#include "MonteCarloCalculus.hxx"
#include "QuadraticCumulCalculus.hxx"

#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

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
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

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

  mainLayout->addWidget(methodBox);

  QVBoxLayout * methodParametersLayout = new QVBoxLayout;

  /// monte carlo widgets
  monteCarloWidget_ = new QWidget;
  QGridLayout *mclayout = new QGridLayout(monteCarloWidget_);

  QLabel * nbSimuLabel = new QLabel(tr("Number of simulations"));
  nbSimuSpinbox_ = new QSpinBox;
  nbSimuSpinbox_->setMinimum(2);
  nbSimuSpinbox_->setMaximum(std::numeric_limits<int>::max());
  if (calculus_.getImplementation()->getClassName() == "MonteCarloCalculus")
    nbSimuSpinbox_->setValue(dynamic_cast<MonteCarloCalculus*>(&*calculus_.getImplementation())->getNbSimulations());
  connect(nbSimuSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(nbSimuChanged(int)));

  nbSimuLabel->setBuddy(nbSimuSpinbox_);
  mclayout->addWidget(nbSimuLabel, 0, 0);
  mclayout->addWidget(nbSimuSpinbox_, 0, 1);

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

  confidenceIntervalCheckBox_ = new QCheckBox;
  if (calculus_.getImplementation()->getClassName() == "MonteCarloCalculus")
    confidenceIntervalCheckBox_->setChecked(dynamic_cast<MonteCarloCalculus*>(&*calculus_.getImplementation())->isConfidenceIntervalRequired());
  confidenceIntervalCheckBox_->setText(tr("Compute confidence interval at"));
  connect(confidenceIntervalCheckBox_, SIGNAL(toggled(bool)), this, SLOT(confidenceIntervalRequired(bool)));
  advancedWidgetsLayout->addWidget(confidenceIntervalCheckBox_, 0, 0);
  levelConfidenceIntervalSpinbox_ = new QDoubleSpinBox;
  if (calculus_.getImplementation()->getClassName() == "MonteCarloCalculus")
  {
    levelConfidenceIntervalSpinbox_->setValue(dynamic_cast<MonteCarloCalculus*>(&*calculus_.getImplementation())->getLevelConfidenceInterval());
    confidenceIntervalRequired(confidenceIntervalCheckBox_->isChecked());
  }
  connect(levelConfidenceIntervalSpinbox_, SIGNAL(valueChanged(double)), this, SLOT(levelConfidenceIntervalChanged(double)));
  advancedWidgetsLayout->addWidget(levelConfidenceIntervalSpinbox_, 0, 1);

  QLabel * seedLabel = new QLabel(tr("Seed"));
  advancedWidgetsLayout->addWidget(seedLabel, 2, 0);
  seedSpinbox_ = new QSpinBox;
  seedLabel->setBuddy(seedSpinbox_);
  advancedWidgetsLayout->addWidget(seedSpinbox_, 2, 1);

  advancedWidgets_->hide();
  advancedGroupLayout->addWidget(advancedWidgets_);

  mclayout->addWidget(advancedGroup, 2, 0, 1, -1);

  connect(advancedGroup, SIGNAL(toggled(bool)), this, SLOT(showHideAdvancedWidgets(bool)));

  methodParametersLayout->addWidget(monteCarloWidget_);

  /// taylor widgets
  taylorWidget_ = new QWidget;
  QVBoxLayout * taylorLayout = new QVBoxLayout(taylorWidget_);
  methodParametersLayout->addWidget(taylorWidget_);
  methodParametersLayout->addStretch();

  mainLayout->addLayout(methodParametersLayout);

  updateMethodWidgets();

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
      {
        calculus_ = MonteCarloCalculus("aNameMC", physicalModel_);
        emit calculusChanged(calculus_);
      }
      nbSimuSpinbox_->setValue(dynamic_cast<MonteCarloCalculus*>(&*calculus_.getImplementation())->getNbSimulations());
      break;
    }
    case DistributionAnalysisWizard::Taylor:
    {
      monteCarloWidget_->hide();
      taylorWidget_->show();
      if (calculus_.getImplementation()->getClassName() == "MonteCarloCalculus")
      {
        calculus_ = QuadraticCumulCalculus("aNameTaylor", physicalModel_);
        emit calculusChanged(calculus_);
      }
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


void DistributionAnalysisWizard::confidenceIntervalRequired(bool confidenceIntervalRequired)
{
  if (confidenceIntervalRequired)
  {
    levelConfidenceIntervalSpinbox_->setEnabled(true);
    dynamic_cast<MonteCarloCalculus*>(&*calculus_.getImplementation())->setIsConfidenceIntervalRequired(true);
  }
  else
  {
    levelConfidenceIntervalSpinbox_->setEnabled(false);
    dynamic_cast<MonteCarloCalculus*>(&*calculus_.getImplementation())->setIsConfidenceIntervalRequired(false);
  }
}


void DistributionAnalysisWizard::nbSimuChanged(int nbSimu)
{
  dynamic_cast<MonteCarloCalculus*>(&*calculus_.getImplementation())->setNbSimulations(nbSimu);
}


void DistributionAnalysisWizard::levelConfidenceIntervalChanged(double confidenceInterval)
{
  dynamic_cast<MonteCarloCalculus*>(&*calculus_.getImplementation())->setLevelConfidenceInterval(confidenceInterval);
}


void DistributionAnalysisWizard::validate()
{
  OTStudy_->addCalculus(calculus_);
  calculus_.run();
}


}