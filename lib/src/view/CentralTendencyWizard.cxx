// CentralTendencyWizard.cxx

#include "otgui/CentralTendencyWizard.hxx"

#include "otgui/MonteCarloAnalysis.hxx"
#include "otgui/QuadraticCumulAnalysis.hxx"

#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <limits>

namespace OTGUI {

CentralTendencyWizard::CentralTendencyWizard(OTStudy * otStudy, const PhysicalModel & physicalModel)
 : QWizard()
 , analysis_(MonteCarloAnalysis("aNameFromFirstPage", physicalModel))
 , otStudy_(otStudy)
 , physicalModel_(physicalModel)
{
  buildInterface();
}


CentralTendencyWizard::CentralTendencyWizard(const Analysis & analysis)
 : QWizard()
 , analysis_(analysis)
 , physicalModel_(analysis_.getPhysicalModel())
{
  buildInterface();
}


void CentralTendencyWizard::buildInterface()
{
  setWindowTitle("Central tendency");

  // First Page: model

  // Second Page: methods
  QWizardPage * page = new QWizardPage(this);

  QVBoxLayout * mainLayout = new QVBoxLayout(page);

  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  methodGroup_ = new QButtonGroup;
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Monte Carlo"));
  if (analysis_.getImplementation()->getClassName() == "MonteCarloAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, CentralTendencyWizard::MonteCarlo);
  methodLayout->addWidget(buttonToChooseMethod);
  buttonToChooseMethod = new QRadioButton(tr("Quadratic cumul"));
  if (analysis_.getImplementation()->getClassName() == "QuadraticCumulAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, CentralTendencyWizard::QuadraticCumul);
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
  if (analysis_.getImplementation()->getClassName() == "MonteCarloAnalysis")
    nbSimuSpinbox_->setValue(dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->getNbSimulations());
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
  if (analysis_.getImplementation()->getClassName() == "MonteCarloAnalysis")
    confidenceIntervalCheckBox_->setChecked(dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->isConfidenceIntervalRequired());
  else
    confidenceIntervalCheckBox_->setChecked(true);
  confidenceIntervalCheckBox_->setText(tr("Compute confidence interval at"));
  connect(confidenceIntervalCheckBox_, SIGNAL(toggled(bool)), this, SLOT(confidenceIntervalRequired(bool)));
  advancedWidgetsLayout->addWidget(confidenceIntervalCheckBox_, 0, 0);
  levelConfidenceIntervalSpinbox_ = new QDoubleSpinBox;
  if (analysis_.getImplementation()->getClassName() == "MonteCarloAnalysis")
  {
    levelConfidenceIntervalSpinbox_->setValue(dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->getLevelConfidenceInterval());
    confidenceIntervalRequired(confidenceIntervalCheckBox_->isChecked());
  }
  else
  {
    levelConfidenceIntervalSpinbox_->setValue(0.95);
    levelConfidenceIntervalSpinbox_->setEnabled(true);
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

  /// Quadratic Cumul widgets
  quadraticCumulWidget_ = new QWidget;
  QVBoxLayout * taylorLayout = new QVBoxLayout(quadraticCumulWidget_);
  methodParametersLayout->addWidget(quadraticCumulWidget_);
  methodParametersLayout->addStretch();

  mainLayout->addLayout(methodParametersLayout);

  updateMethodWidgets();

  addPage(page);
}


void CentralTendencyWizard::updateMethodWidgets()
{
  switch (CentralTendencyWizard::Method(methodGroup_->checkedId()))
  {
    case CentralTendencyWizard::MonteCarlo:
    {
      monteCarloWidget_->show();
      quadraticCumulWidget_->hide();
      if (analysis_.getImplementation()->getClassName() == "QuadraticCumulAnalysis")
      {
        analysis_ = MonteCarloAnalysis(analysis_.getName(), physicalModel_);
        emit analysisChanged(analysis_);
      }
      nbSimuSpinbox_->setValue(dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->getNbSimulations());
      break;
    }
    case CentralTendencyWizard::QuadraticCumul:
    {
      monteCarloWidget_->hide();
      quadraticCumulWidget_->show();
      if (analysis_.getImplementation()->getClassName() == "MonteCarloAnalysis")
      {
        analysis_ = QuadraticCumulAnalysis(analysis_.getName(), physicalModel_);
        emit analysisChanged(analysis_);
      }
      break;
    }
    default:
      break;
  }
}


void CentralTendencyWizard::showHideAdvancedWidgets(bool show)
{
  if (show)
    advancedWidgets_->show();
  else
    advancedWidgets_->hide();
}


void CentralTendencyWizard::confidenceIntervalRequired(bool confidenceIntervalRequired)
{
  if (confidenceIntervalRequired)
  {
    levelConfidenceIntervalSpinbox_->setEnabled(true);
    dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->setIsConfidenceIntervalRequired(true);
  }
  else
  {
    levelConfidenceIntervalSpinbox_->setEnabled(false);
    dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->setIsConfidenceIntervalRequired(false);
  }
}


void CentralTendencyWizard::nbSimuChanged(int nbSimu)
{
  dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->setNbSimulations(nbSimu);
}


void CentralTendencyWizard::levelConfidenceIntervalChanged(double confidenceInterval)
{
  dynamic_cast<MonteCarloAnalysis*>(&*analysis_.getImplementation())->setLevelConfidenceInterval(confidenceInterval);
}


void CentralTendencyWizard::validate()
{
  otStudy_->addAnalysis(analysis_);
  analysis_.run();
}


}