// ReliabilityAnalysisWizard.cxx

#include "otgui/ReliabilityAnalysisWizard.hxx"

#include "otgui/MonteCarloReliabilityAnalysis.hxx"

#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QVBoxLayout>

#include <limits>

namespace OTGUI {

ReliabilityAnalysisWizard::ReliabilityAnalysisWizard(OTStudy * otStudy, const LimitState & limitState)
 : QWizard()
 , analysis_(MonteCarloReliabilityAnalysis(otStudy->getAvailableAnalysisName("reliability_"), limitState))
 , otStudy_(otStudy)
//  , limitState_(limitState)
{
  buildInterface();
}


ReliabilityAnalysisWizard::ReliabilityAnalysisWizard(const Analysis & analysis)
 : QWizard()
 , analysis_(analysis)
//  , limitState_(dynamic_cast<ReliabilityAnalysis*>(&*analysis.getImplementation())->getLimitState())
{
  buildInterface();
}


void ReliabilityAnalysisWizard::buildInterface()
{
  setWindowTitle("Reliability analysis");
  setWindowIcon(QIcon(":/images/OT_icon16x16.png"));

  // First Page: model

  // Second Page: methods
  QWizardPage * page = new QWizardPage(this);

  QVBoxLayout * mainLayout = new QVBoxLayout(page);

  QGroupBox * methodBox = new QGroupBox(tr("Method"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  methodGroup_ = new QButtonGroup;
  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Monte Carlo"));
  if (analysis_.getImplementation()->getClassName() == "MonteCarloReliabilityAnalysis")
    buttonToChooseMethod->setChecked(true);
  methodGroup_->addButton(buttonToChooseMethod, ReliabilityAnalysisWizard::MonteCarlo);
  methodLayout->addWidget(buttonToChooseMethod);

//   connect(methodGroup_, SIGNAL(buttonClicked(int)), this, SLOT(updateMethodWidgets()));

  mainLayout->addWidget(methodBox);

  QVBoxLayout * methodParametersLayout = new QVBoxLayout;

  /// monte carlo widgets
  monteCarloWidget_ = new QWidget;
  QGridLayout *mclayout = new QGridLayout(monteCarloWidget_);

  QLabel * maxiOuterSamplingLabel = new QLabel(tr("Maximum outer sampling"));
  maxiOuterSamplingSpinbox_ = new QSpinBox;
  maxiOuterSamplingSpinbox_->setMinimum(2);
  maxiOuterSamplingSpinbox_->setMaximum(std::numeric_limits<int>::max());
  if (analysis_.getImplementation()->getClassName() == "MonteCarloReliabilityAnalysis")
    maxiOuterSamplingSpinbox_->setValue(dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->getMaximumOuterSampling());
  connect(maxiOuterSamplingSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(maxiOuterSamplingChanged(int)));

  maxiOuterSamplingLabel->setBuddy(maxiOuterSamplingSpinbox_);
  mclayout->addWidget(maxiOuterSamplingLabel, 0, 0);
  mclayout->addWidget(maxiOuterSamplingSpinbox_, 0, 1);

  QLabel * maxiCoefficientOfVariationLabel = new QLabel(tr("Maximum coefficient of variation"));
  maxiCoefficientOfVariationSpinbox_ = new QDoubleSpinBox;
  maxiCoefficientOfVariationSpinbox_->setMinimum(0.);
  maxiCoefficientOfVariationSpinbox_->setMaximum(1.);
  if (analysis_.getImplementation()->getClassName() == "MonteCarloReliabilityAnalysis")
    maxiCoefficientOfVariationSpinbox_->setValue(dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->getMaximumCoefficientOfVariation());
  connect(maxiCoefficientOfVariationSpinbox_, SIGNAL(valueChanged(double)), this, SLOT(maxiCoefficientOfVariationChanged(double)));

  maxiCoefficientOfVariationLabel->setBuddy(maxiCoefficientOfVariationSpinbox_);
  mclayout->addWidget(maxiCoefficientOfVariationLabel, 1, 0);
  mclayout->addWidget(maxiCoefficientOfVariationSpinbox_, 1, 1);

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
  advancedWidgetsLayout->addWidget(seedLabel, 0, 0);
  seedSpinbox_ = new QSpinBox;
  seedSpinbox_->setMaximum(std::numeric_limits<int>::max());
  seedLabel->setBuddy(seedSpinbox_);
  advancedWidgetsLayout->addWidget(seedSpinbox_, 0, 1);
  if (analysis_.getImplementation()->getClassName() == "MonteCarloReliabilityAnalysis")
    seedSpinbox_->setValue(dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->getSeed());
  connect(seedSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(seedChanged(int)));

  QLabel * blockSizeLabel = new QLabel(tr("Block size"));
  advancedWidgetsLayout->addWidget(blockSizeLabel, 1, 0);
  blockSizeSpinbox_ = new QSpinBox;
  blockSizeSpinbox_->setMinimum(1);
  blockSizeSpinbox_->setMaximum(std::numeric_limits<int>::max());
  blockSizeLabel->setBuddy(blockSizeSpinbox_);
  advancedWidgetsLayout->addWidget(blockSizeSpinbox_, 1, 1);
  if (analysis_.getImplementation()->getClassName() == "MonteCarloReliabilityAnalysis")
    blockSizeSpinbox_->setValue(dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->getBlockSize());
  connect(blockSizeSpinbox_, SIGNAL(valueChanged(int)), this, SLOT(blockSizeChanged(int)));

  advancedWidgets_->hide();
  advancedGroupLayout->addWidget(advancedWidgets_);

  mclayout->addWidget(advancedGroup, 2, 0, 1, -1);

  connect(advancedGroup, SIGNAL(toggled(bool)), this, SLOT(showHideAdvancedWidgets(bool)));

  methodParametersLayout->addWidget(monteCarloWidget_);

  mainLayout->addLayout(methodParametersLayout);

//   updateMethodWidgets();

  addPage(page);
}


void ReliabilityAnalysisWizard::showHideAdvancedWidgets(bool show)
{
  if (show)
    advancedWidgets_->show();
  else
    advancedWidgets_->hide();
}


void ReliabilityAnalysisWizard::maxiOuterSamplingChanged(int maxi)
{
  dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->setMaximumOuterSampling(maxi);
}


void ReliabilityAnalysisWizard::maxiCoefficientOfVariationChanged(double maxi)
{
  dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->setMaximumCoefficientOfVariation(maxi);
}


void ReliabilityAnalysisWizard::blockSizeChanged(int size)
{
  dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->setBlockSize(size);
}


void ReliabilityAnalysisWizard::seedChanged(int seed)
{
  dynamic_cast<MonteCarloReliabilityAnalysis*>(&*analysis_.getImplementation())->setSeed(seed);
}


void ReliabilityAnalysisWizard::validate()
{
  otStudy_->addAnalysis(analysis_);
  analysis_.run();
}
}