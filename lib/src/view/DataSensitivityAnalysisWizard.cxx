//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a data sensitivity analysis
 *
 *  Copyright 2015-2026 EDF-Phimeca
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

#include "persalys/DataSensitivityAnalysisWizard.hxx"
#include "persalys/HSICCovarianceModelsTableModel.hxx"
#include "persalys/HSICAlphaTableModel.hxx"
#include "persalys/CriticalDomainTableModel.hxx"
#include "persalys/ComboBoxDelegate.hxx"
#include "persalys/SpinBoxDelegate.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>

using namespace OT;

namespace PERSALYS
{

/********************************* Wizard *************************************/

DataSensitivityAnalysisWizard::DataSensitivityAnalysisWizard(const Analysis& analysis, QWidget* parent)
: AnalysisWizard(analysis, parent)
{
  setWindowTitle(tr("Data sensitivity analysis"));
  docLink_ = "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#sensitivity-analysis";

  analysis_ptr_ = dynamic_cast<const DataSensitivityAnalysis *>(analysis.getImplementation().get());

  if (!analysis_ptr_)
    throw InvalidArgumentException(HERE) << "DataSensitivityAnalysisWizard received an analysis of type " << analysis.getClassName();
  
  auto * draft_analysis_ptr = analysis_ptr_->clone();
  const auto doe = analysis_ptr_->getDesignOfExperiment();

  // Intro page
  introPage_ = new DataSensitivityAnalysisIntroPage(doe.getType());
  introPage_->initialize(draft_analysis_ptr);
  setPage(Page::Intro, introPage_);  
  
  // Global HSIC parameters page
  globalHSICParametersPage_ = new DataSensitivityAnalysisHSICParametersPage(
    doe.getSample().getSize(), 
    doe.getSample().getDescription(), 
    DataSensitivityAnalysis::HSICType::Global
  );

  setPage(Page::GlobalHSICParameters, globalHSICParametersPage_);

  connect (introPage_, &DataSensitivityAnalysisIntroPage::pageValidated, this, [this, draft_analysis_ptr]() {
    draft_analysis_ptr->setType(introPage_->getType());
    globalHSICParametersPage_->initialize(draft_analysis_ptr);
  });

  // Target HSIC parameters page
  targetHSICParametersPage_ = new DataSensitivityAnalysisHSICParametersPage(
    doe.getSample().getSize(), 
    doe.getSample().getDescription(), 
    DataSensitivityAnalysis::HSICType::Target
  );

  setPage(Page::TargetHSICParameters, targetHSICParametersPage_);

  connect(introPage_, &DataSensitivityAnalysisIntroPage::pageValidated, this, [this, draft_analysis_ptr]() {
    targetHSICParametersPage_->initialize(draft_analysis_ptr);
  });

  connect(globalHSICParametersPage_, &DataSensitivityAnalysisHSICParametersPage::pageValidated, this, [this, draft_analysis_ptr]() {
    targetHSICParametersPage_->initialize(draft_analysis_ptr);
  });

  // Conditional HSIC parameters page
  conditionalHSICParametersPage_ = new DataSensitivityAnalysisHSICParametersPage(
    doe.getSample().getSize(), 
    doe.getSample().getDescription(), 
    DataSensitivityAnalysis::HSICType::Conditional
  );

  setPage(Page::ConditionalHSICParameters, conditionalHSICParametersPage_);

  connect(introPage_, &DataSensitivityAnalysisIntroPage::pageValidated, this, [this, draft_analysis_ptr]() {
    conditionalHSICParametersPage_->initialize(draft_analysis_ptr);
  });

  connect(globalHSICParametersPage_, &DataSensitivityAnalysisHSICParametersPage::pageValidated, this, [this, draft_analysis_ptr]() {
    conditionalHSICParametersPage_->initialize(draft_analysis_ptr);
  });

  connect(targetHSICParametersPage_, &DataSensitivityAnalysisHSICParametersPage::pageValidated, this, [this, draft_analysis_ptr]() {
    conditionalHSICParametersPage_->initialize(draft_analysis_ptr);
  });

  // Critical domain page (shared by target and conditional HSIC)
  criticalDomainPage_ = new DataSensitivityAnalysisCriticalDomainPage;
  setPage(Page::CriticalDomain, criticalDomainPage_);

  connect(introPage_, &DataSensitivityAnalysisIntroPage::pageValidated, this, [this, draft_analysis_ptr]() {
    criticalDomainPage_->initialize(draft_analysis_ptr);
  });

  connect(globalHSICParametersPage_, &DataSensitivityAnalysisHSICParametersPage::pageValidated, this, [this, draft_analysis_ptr]() {
    criticalDomainPage_->initialize(draft_analysis_ptr);
  });

  connect(targetHSICParametersPage_, &DataSensitivityAnalysisHSICParametersPage::pageValidated, this, [this, draft_analysis_ptr]() {
    criticalDomainPage_->initialize(draft_analysis_ptr);
  });

  connect(conditionalHSICParametersPage_, &DataSensitivityAnalysisHSICParametersPage::pageValidated, this, [this, draft_analysis_ptr]() {
    criticalDomainPage_->initialize(draft_analysis_ptr);
  });
}

Analysis DataSensitivityAnalysisWizard::getAnalysis() const
{
  const unsigned char type = introPage_->getType();

  DataSensitivityAnalysis analysis(
    analysis_ptr_->getName(),
    analysis_ptr_->getDesignOfExperiment(),
    type,
    introPage_->getInterestVariables()
  );

  auto HSICTypesTable = {
    std::make_tuple(DataSensitivityAnalysis::HSICType::Global, DataSensitivityAnalysis::Type::GlobalHSIC, globalHSICParametersPage_),
    std::make_tuple(DataSensitivityAnalysis::HSICType::Target, DataSensitivityAnalysis::Type::TargetHSIC, targetHSICParametersPage_),
    std::make_tuple(DataSensitivityAnalysis::HSICType::Conditional, DataSensitivityAnalysis::Type::ConditionalHSIC, conditionalHSICParametersPage_)
  };

  for (const auto& [HSICType, analysisType, page] : HSICTypesTable)
  {
    if (type & analysisType)
    {
      analysis.setHSICParameters(
        page->computePermutationPValues(),
        page->computeAsymptoticPValues(),
        page->useUStatistic(),
        HSICType
      );
      analysis.setCovarianceModels(page->getCovarianceModels(), HSICType);
    }
  }

  if ((type & DataSensitivityAnalysis::Type::TargetHSIC) || (type & DataSensitivityAnalysis::Type::ConditionalHSIC))
  {
    auto criticalDomain = criticalDomainPage_->getCriticalDomain();
    const Point alphas = criticalDomainPage_->getAlphas();
    if (type & DataSensitivityAnalysis::Type::TargetHSIC)
      analysis.setFilterAlphas(alphas, criticalDomain);
    if (type & DataSensitivityAnalysis::Type::ConditionalHSIC)
      analysis.setWeightAlphas(alphas, criticalDomain);
  }

  analysis.setInterestVariables(introPage_->getInterestVariables());

  return analysis;
}

int DataSensitivityAnalysisWizard::nextId() const
{
  switch (currentId())
  {
    case Page::Intro:
      return introPage_->nextId();
    case Page::GlobalHSICParameters:
      return globalHSICParametersPage_->nextId();
    case Page::TargetHSICParameters:
      return targetHSICParametersPage_->nextId();
    case Page::ConditionalHSICParameters:
      return conditionalHSICParametersPage_->nextId();
    case Page::CriticalDomain:
      return -1;
    default:
      return -1;
  }
}

/********************************* Intro Page *********************************/

DataSensitivityAnalysisIntroPage::DataSensitivityAnalysisIntroPage(DesignOfExperiment::Type doeType, QWidget* parent)
: QWizardPage(parent)
{
  setTitle(tr("Sensitivity analysis - methods"));
  auto * pageLayout = new QVBoxLayout(this);

  // output selection
  outputsSelectionGroupBox_ = new OutputsSelectionGroupBox(this);
  outputsSelectionGroupBox_->setTitle(tr("Variable(s) of interest"));
  pageLayout->addWidget(outputsSelectionGroupBox_);

  // method selection
  auto * GSAGroupBox = new QGroupBox(tr("Global sensitivity analysis methods"));
  auto * GSALayout = new QVBoxLayout(GSAGroupBox);

  methodGroup_ = new QButtonGroup(this);
  methodGroup_->setExclusive(false);

  // Sobol
  auto * SobolCB = new QCheckBox(tr("Sobol indices (rank sobol algorithm)"));
  methodGroup_->addButton(SobolCB, DataSensitivityAnalysisResult::RankSobol);
  SobolCB->setChecked(doeType == DesignOfExperiment::Type::MC);
  GSALayout->addWidget(SobolCB);

  // SRC
  auto * SRCCB = new QCheckBox(tr("Standard regression coefficients"));
  methodGroup_->addButton(SRCCB, DataSensitivityAnalysisResult::SRC);
  SRCCB->setChecked(true);
  GSALayout->addWidget(SRCCB);

  // Global HSIC
  auto * globalHSICCB = new QCheckBox(tr("Global HSIC"));
  methodGroup_->addButton(globalHSICCB, DataSensitivityAnalysisResult::GlobalHSIC);
  connect(globalHSICCB, &QCheckBox::toggled, this, [this]() {
    setFinalPage(nextId() == -1);
  });
  GSALayout->addWidget(globalHSICCB);

  pageLayout->addWidget(GSAGroupBox);

  auto * ROSAGroupBox = new QGroupBox(tr("Reliability oriented sensitivity analysis methods"));
  auto * ROSALayout = new QVBoxLayout(ROSAGroupBox);

  // Target HSIC
  auto * targetHSICCB = new QCheckBox(tr("Target HSIC"));
  methodGroup_->addButton(targetHSICCB, DataSensitivityAnalysisResult::TargetHSIC);
  connect(targetHSICCB, &QCheckBox::toggled, this, [this]() {
    setFinalPage(nextId() == -1);
  });
  ROSALayout->addWidget(targetHSICCB);

  // Conditional HSIC
  auto * conditionalHSICCB = new QCheckBox(tr("Conditional HSIC"));
  methodGroup_->addButton(conditionalHSICCB, DataSensitivityAnalysisResult::ConditionalHSIC);
  connect(conditionalHSICCB, &QCheckBox::toggled, this, [this]() {
    setFinalPage(nextId() == -1);
  });
  ROSALayout->addWidget(conditionalHSICCB);

  pageLayout->addWidget(ROSAGroupBox);

  // warning message
  if (doeType != DesignOfExperiment::Type::MC)
  {
    warningWidget_ = new ErrorWidget;
    warningWidget_->setMessage(tr("Warning: if the variables are not i.i.d, there is no theoretical guarantee for rank Sobol and for HSIC"), ErrorWidget::Warning);
    pageLayout->addWidget(warningWidget_);
  }

  // error message
  errorWidget_ = new ErrorWidget;
  connect(outputsSelectionGroupBox_, &OutputsSelectionGroupBox::outputsSelectionChanged, errorWidget_, &ErrorWidget::reset);
  connect(methodGroup_, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), errorWidget_, &ErrorWidget::reset);
  pageLayout->addStretch();
  pageLayout->addWidget(errorWidget_);
}

void DataSensitivityAnalysisIntroPage::initialize(DataSensitivityAnalysis * analysis_ptr)
{
  if (!analysis_ptr)
    return;

  const auto outputsNames = analysis_ptr->getDesignOfExperiment().getOutputSample().getDescription();
  outputsSelectionGroupBox_->updateComboBoxModel(outputsNames, analysis_ptr->getInterestVariables());

  if (analysis_ptr->getInterestVariables().isEmpty())
    analysis_ptr->setInterestVariables(outputsNames);
  
  outputsSelectionGroupBox_->updateComboBoxModel(outputsNames, analysis_ptr->getInterestVariables());

  connect(outputsSelectionGroupBox_, &OutputsSelectionGroupBox::outputsSelectionChanged, this, [this, analysis_ptr]() {
    analysis_ptr->setInterestVariables(getInterestVariables());
  });

  methodGroup_->button(DataSensitivityAnalysisResult::RankSobol)->setChecked(analysis_ptr->computeRankSobol());
  methodGroup_->button(DataSensitivityAnalysisResult::SRC)->setChecked(analysis_ptr->computeSRC());
  methodGroup_->button(DataSensitivityAnalysisResult::GlobalHSIC)->setChecked(analysis_ptr->computeHSIC(DataSensitivityAnalysisResult::Global));
  methodGroup_->button(DataSensitivityAnalysisResult::TargetHSIC)->setChecked(analysis_ptr->computeHSIC(DataSensitivityAnalysisResult::Target));
  methodGroup_->button(DataSensitivityAnalysisResult::ConditionalHSIC)->setChecked(analysis_ptr->computeHSIC(DataSensitivityAnalysisResult::Conditional));
}

unsigned char DataSensitivityAnalysisIntroPage::getType() const
{
  unsigned char type = 0;
  for (auto button : methodGroup_->buttons())
  {
    if (button->isChecked())
      type |= methodGroup_->id(button);
  }
  
  return type;
}

OT::Description DataSensitivityAnalysisIntroPage::getInterestVariables() const
{
  const QStringList outputsList = outputsSelectionGroupBox_->getSelectedOutputsNames();

  OT::Description outputNames;
  for (const auto & output : outputsList)
    outputNames.add(output.toStdString());
  
  return outputNames;
}

int DataSensitivityAnalysisIntroPage::nextId() const
{
  if (methodGroup_->button(DataSensitivityAnalysisResult::GlobalHSIC) -> isChecked())
    return DataSensitivityAnalysisWizard::Page::GlobalHSICParameters;
  if (methodGroup_->button(DataSensitivityAnalysisResult::TargetHSIC) -> isChecked())
    return DataSensitivityAnalysisWizard::Page::TargetHSICParameters;
  if (methodGroup_->button(DataSensitivityAnalysisResult::ConditionalHSIC) -> isChecked())
    return DataSensitivityAnalysisWizard::Page::ConditionalHSICParameters;
  else
    return -1;
}

bool DataSensitivityAnalysisIntroPage::validatePage()
{
  if (outputsSelectionGroupBox_->getSelectedOutputsNames().isEmpty())
  {
    errorWidget_->setMessage(tr("Please select at least one variable of interest"));
    return false;
  }
  
  if (!methodGroup_->checkedButton())
  {
    errorWidget_->setMessage(tr("Please select at least one method"));
    return false;
  }

  emit pageValidated();
  return QWizardPage::validatePage();
}

/************************* Critical Domain page *******************************/

DataSensitivityAnalysisCriticalDomainPage::DataSensitivityAnalysisCriticalDomainPage(QWidget* parent)
: QWizardPage(parent)
{
  setTitle(tr("Critical domain and filter/weight parameters"));

  auto * pageLayout = new QVBoxLayout(this);

  // Critical domain table
  auto * domainGroupBox = new QGroupBox(tr("Critical domain"));
  auto * domainLayout = new QVBoxLayout(domainGroupBox);

  criticalDomainTableView_ = new QTableView;
  criticalDomainTableView_->setEditTriggers(QTableView::AllEditTriggers);
  criticalDomainTableView_->verticalHeader()->hide();
  criticalDomainTableView_->horizontalHeader()->setStretchLastSection(true);
  criticalDomainTableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  domainLayout->addWidget(criticalDomainTableView_);
  pageLayout->addWidget(domainGroupBox);

  // Formula display
  auto * formulaLabel = new QLabel(
    tr("Filter/weight function: <i>\xcf\x86</i>(x) = exp( \xe2\x88\x92 d(x, D) / (\xce\xb1 \xc2\xb7 \xcf\x83) )   "
       "where D is the critical domain, \xce\xb1 is a tuning parameter and \xcf\x83 the standard deviation."));
  formulaLabel->setWordWrap(true);
  pageLayout->addWidget(formulaLabel);

  // Alpha table for filter/weight functions
  auto * alphaGroupBox = new QGroupBox(tr("Filter/weight functions"));
  auto * alphaLayout = new QVBoxLayout(alphaGroupBox);

  alphaTableView_ = new QTableView;
  alphaTableView_->setEditTriggers(QTableView::AllEditTriggers);
  alphaTableView_->verticalHeader()->hide();
  alphaTableView_->horizontalHeader()->setStretchLastSection(true);
  alphaTableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  alphaLayout->addWidget(alphaTableView_);
  pageLayout->addWidget(alphaGroupBox);

  // Error widget
  errorWidget_ = new ErrorWidget;
  pageLayout->addStretch();
  pageLayout->addWidget(errorWidget_);
}

void DataSensitivityAnalysisCriticalDomainPage::initialize(DataSensitivityAnalysis * analysis_ptr)
{
  if (!analysis_ptr)
    return;

  analysis_ptr_ = analysis_ptr;

  const Description interestVariables = analysis_ptr->getInterestVariables();

  // Critical domain table
  criticalDomainTableModel_ = new CriticalDomainTableModel(interestVariables, this);
  connect(criticalDomainTableModel_, &CriticalDomainTableModel::errorMessageChanged,
          errorWidget_, &ErrorWidget::setTemporaryFramelessErrorMessage);
  criticalDomainTableView_->setModel(criticalDomainTableModel_);

  // Set spinbox delegates for bound columns
  auto * lowerDelegate = new SpinBoxDelegate(criticalDomainTableView_);
  lowerDelegate->setSpinBoxType(SpinBoxDelegate::doubleValue);
  criticalDomainTableView_->setItemDelegateForColumn(1, lowerDelegate);

  auto * upperDelegate = new SpinBoxDelegate(criticalDomainTableView_);
  upperDelegate->setSpinBoxType(SpinBoxDelegate::doubleValue);
  criticalDomainTableView_->setItemDelegateForColumn(2, upperDelegate);

  if (analysis_ptr->getTargetCriticalDomain().getDimension() == interestVariables.getSize())
    criticalDomainTableModel_->setInterval(analysis_ptr->getTargetCriticalDomain());
  else if (analysis_ptr->getConditionalCriticalDomain().getDimension() == interestVariables.getSize())
    criticalDomainTableModel_->setInterval(analysis_ptr->getConditionalCriticalDomain());

  // Alpha table
  alphaTableModel_ = new HSICAlphaTableModel(interestVariables, this);
  connect(alphaTableModel_, &HSICAlphaTableModel::errorMessageChanged,
          errorWidget_, &ErrorWidget::setTemporaryFramelessErrorMessage);
  alphaTableView_->setModel(alphaTableModel_);

  // Set spinbox delegate for alpha column
  auto * alphaDelegate = new SpinBoxDelegate(alphaTableView_);
  alphaDelegate->setSpinBoxType(SpinBoxDelegate::doubleValue);
  alphaTableView_->setItemDelegateForColumn(1, alphaDelegate);

  // Use filter alphas if available, otherwise weight alphas
  if (!analysis_ptr->getFilterAlphas().isEmpty())
    alphaTableModel_->setAlphas(analysis_ptr->getFilterAlphas());
  else if (!analysis_ptr->getWeightAlphas().isEmpty())
    alphaTableModel_->setAlphas(analysis_ptr->getWeightAlphas());

  setFinalPage(nextId() == -1);
}

OT::Interval DataSensitivityAnalysisCriticalDomainPage::getCriticalDomain() const
{
  if (criticalDomainTableModel_)
    return criticalDomainTableModel_->getInterval();
  return OT::Interval();
}

OT::Point DataSensitivityAnalysisCriticalDomainPage::getAlphas() const
{
  if (alphaTableModel_)
    return alphaTableModel_->getAlphas();
  return OT::Point();
}

int DataSensitivityAnalysisCriticalDomainPage::nextId() const
{
  if (analysis_ptr_->computeHSIC(DataSensitivityAnalysis::HSICType::Target))
    return DataSensitivityAnalysisWizard::Page::TargetHSICParameters;
  if (analysis_ptr_->computeHSIC(DataSensitivityAnalysis::HSICType::Conditional))
    return DataSensitivityAnalysisWizard::Page::ConditionalHSICParameters;
  return -1;
}

bool DataSensitivityAnalysisCriticalDomainPage::validatePage()
{
  if (criticalDomainTableModel_ && criticalDomainTableModel_->hasErrors())
  {
    errorWidget_->setTemporaryFramelessErrorMessage(tr("Lower bound must be less than or equal to upper bound"));
    return false;
  }
  if (alphaTableModel_ && alphaTableModel_->hasErrors())
  {
    errorWidget_->setTemporaryFramelessErrorMessage(tr("Alpha must be strictly positive"));
    return false;
  }
  emit pageValidated();
  return QWizardPage::validatePage();
}

/**************************** HSIC Parameters page ****************************/

DataSensitivityAnalysisHSICParametersPage::DataSensitivityAnalysisHSICParametersPage(
  OT::UnsignedInteger sampleSize, 
  const OT::Description & variableNames, 
  DataSensitivityAnalysis::HSICType type, 
  QWidget* parent)
: QWizardPage(parent)
, type_(type)
{
  QString title;
  switch (type)
  {
    case DataSensitivityAnalysis::HSICType::Global:
      title = tr("Global HSIC parameters");
      break;
    case DataSensitivityAnalysis::HSICType::Target:
      title = tr("Target HSIC parameters");
      break;
    case DataSensitivityAnalysis::HSICType::Conditional:
      title = tr("Conditional HSIC parameters");
      break;
    default:
      throw InvalidArgumentException(HERE) << "Invalid HSIC Type";
  }
  setTitle(title);

  auto * pageLayout = new QVBoxLayout(this);

  if (type != DataSensitivityAnalysis::HSICType::Conditional)
  {
    useUStatisticComboBox_ = new QComboBox;
    useUStatisticComboBox_->addItem(tr("Use V-statistic (biased but faster and asymptotically unbiased)"), false);
    useUStatisticComboBox_->addItem(tr("Use U-statistic (unbiased but more computationally expensive)"), true);
    pageLayout->addWidget(useUStatisticComboBox_);
  }

  computePermutationPValuesCheckBox_ = new QCheckBox(tr("Compute permutation p-values"));
  computePermutationPValuesCheckBox_->setChecked(sampleSize < 100);
  pageLayout->addWidget(computePermutationPValuesCheckBox_);

  if (type != DataSensitivityAnalysis::HSICType::Conditional)
  {
    computeAsymptoticPValuesCheckBox_ = new QCheckBox(tr("Compute asymptotic p-values"));
    computeAsymptoticPValuesCheckBox_->setChecked(sampleSize >= 100);
    pageLayout->addWidget(computeAsymptoticPValuesCheckBox_);
  }

  // Covariance models table
  auto * covGroupBox = new QGroupBox(tr("Covariance models"));
  auto * covLayout = new QVBoxLayout(covGroupBox);

  covarianceTableModel_ = new HSICCovarianceModelsTableModel(variableNames, this);

  covarianceTableView_ = new QTableView;
  covarianceTableView_->setModel(covarianceTableModel_);
  covarianceTableView_->setItemDelegateForColumn(1, new ComboBoxDelegate(covarianceTableView_));
  covarianceTableView_->setItemDelegateForColumn(2, new ComboBoxDelegate(covarianceTableView_));
  // Set spinbox delegate for p parameter column
  auto * pDelegate = new SpinBoxDelegate(covarianceTableView_);
  pDelegate->setSpinBoxType(SpinBoxDelegate::doubleValue);
  covarianceTableView_->setItemDelegateForColumn(3, pDelegate);
  covarianceTableView_->verticalHeader()->hide();
  covarianceTableView_->horizontalHeader()->setStretchLastSection(false);
  covarianceTableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
  covarianceTableView_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  covarianceTableView_->horizontalHeader()->setMinimumSectionSize(40);
  covarianceTableView_->horizontalHeader()->resizeSection(1, 190);
  covarianceTableView_->horizontalHeader()->resizeSection(2, 65);
  covarianceTableView_->horizontalHeader()->resizeSection(3, 55);
  covarianceTableView_->horizontalHeader()->resizeSection(4, 110);

  // Open persistent editors so combo boxes are always active (single-click)
  auto openAllPersistentEditors = [this]() {
    for (int row = 0; row < covarianceTableModel_->rowCount(); ++row)
    {
      covarianceTableView_->openPersistentEditor(covarianceTableModel_->index(row, 1));
      covarianceTableView_->openPersistentEditor(covarianceTableModel_->index(row, 2));
    }
  };
  openAllPersistentEditors();

  // Set up "Apply to all" buttons in column 4
  auto setupButtonColumn = [this]() {
    for (int row = 0; row < covarianceTableModel_->rowCount(); ++row)
    {
      auto * btn = new QPushButton(tr("Apply to all"), covarianceTableView_);
      const int capturedRow = row;
      connect(btn, &QPushButton::clicked, covarianceTableModel_, [this, capturedRow]() {
        covarianceTableModel_->applyToAll(capturedRow);
      });
      covarianceTableView_->setIndexWidget(covarianceTableModel_->index(row, 4), btn);
    }
  };
  setupButtonColumn();

  // Re-open persistent editors after a model reset (e.g. setCovarianceModels)
  connect(covarianceTableModel_, &QAbstractItemModel::modelReset, this, openAllPersistentEditors);
  connect(covarianceTableModel_, &QAbstractItemModel::modelReset, this, setupButtonColumn);

  // Refresh the ν persistent editor widget's enabled state when the covariance model changes,
  // without closing/reopening it (which would require two clicks to activate the new widget)
  connect(covarianceTableModel_, &QAbstractItemModel::dataChanged, covarianceTableView_, [this](const QModelIndex & topLeft, const QModelIndex & bottomRight) {
    for (int row = topLeft.row(); row <= bottomRight.row(); ++row)
    {
      const QModelIndex nuIdx = covarianceTableModel_->index(row, 2);
      auto * widget = covarianceTableView_->indexWidget(nuIdx);
      if (widget)
        widget->setEnabled(covarianceTableModel_->flags(nuIdx) & Qt::ItemIsEnabled);
    }
  });

  covLayout->addWidget(covarianceTableView_);
  pageLayout->addWidget(covGroupBox);

  // Error widget
  errorWidget_ = new ErrorWidget;
  connect(covarianceTableModel_, &HSICCovarianceModelsTableModel::errorMessageChanged,
          errorWidget_, &ErrorWidget::setTemporaryFramelessErrorMessage);
  pageLayout->addStretch();
  pageLayout->addWidget(errorWidget_);
}

void DataSensitivityAnalysisHSICParametersPage::initialize(DataSensitivityAnalysis * analysis_ptr)
{
  if (!analysis_ptr)
    return;
  
  analysis_ptr_ = analysis_ptr;

  Description interestVariables = analysis_ptr->getInterestVariables();

  if (analysis_ptr->defaultHSICParametersChanged())
  {
    if (type_ != DataSensitivityAnalysis::HSICType::Conditional)
    {
      computeAsymptoticPValuesCheckBox_->setChecked(analysis_ptr->computeAsymptoticPValues(type_));
      useUStatisticComboBox_->setCurrentIndex(analysis_ptr->useUStatistic(type_) ? 1 : 0);
    }
    computePermutationPValuesCheckBox_->setChecked(analysis_ptr->computePermutationPValues(type_));
  }

  Description variableNames = analysis_ptr->getDesignOfExperiment().getInputSample().getDescription();
  
  variableNames.add(interestVariables);
  covarianceTableModel_->setVariablesNames(variableNames);

  if (!analysis_ptr->getCovarianceModels(type_).isEmpty())
  {
    covarianceTableModel_->setCovarianceModels(analysis_ptr->getCovarianceModels(type_));
  }
  else if (type_ != DataSensitivityAnalysis::HSICType::Global)
  {
    covarianceTableModel_->setCovarianceModels(analysis_ptr->getCovarianceModels(DataSensitivityAnalysis::HSICType::Global));
  }

  setFinalPage(nextId() == -1);
}

bool DataSensitivityAnalysisHSICParametersPage::computeAsymptoticPValues() const
{
  return type_ == DataSensitivityAnalysis::HSICType::Conditional ? false : computeAsymptoticPValuesCheckBox_->isChecked();
}

bool DataSensitivityAnalysisHSICParametersPage::computePermutationPValues() const
{
  return computePermutationPValuesCheckBox_->isChecked();
}

bool DataSensitivityAnalysisHSICParametersPage::useUStatistic() const
{
  return type_ == DataSensitivityAnalysis::HSICType::Conditional ? false : useUStatisticComboBox_->currentData().toBool();
}

OT::Collection<OT::CovarianceModel> DataSensitivityAnalysisHSICParametersPage::getCovarianceModels() const
{
  return covarianceTableModel_->getCovarianceModels();
}

int DataSensitivityAnalysisHSICParametersPage::nextId() const
{
  switch (type_)
  {
    case DataSensitivityAnalysis::HSICType::Global:
      if (analysis_ptr_->computeHSIC(DataSensitivityAnalysis::HSICType::Target))
        return DataSensitivityAnalysisWizard::Page::TargetHSICParameters;
      if (analysis_ptr_->computeHSIC(DataSensitivityAnalysis::HSICType::Conditional))
        return DataSensitivityAnalysisWizard::Page::ConditionalHSICParameters;
      return -1;
    case DataSensitivityAnalysis::HSICType::Target:
      if (analysis_ptr_->computeHSIC(DataSensitivityAnalysis::HSICType::Conditional))
        return DataSensitivityAnalysisWizard::Page::ConditionalHSICParameters;
      return DataSensitivityAnalysisWizard::Page::CriticalDomain;
    case DataSensitivityAnalysis::HSICType::Conditional:
      return DataSensitivityAnalysisWizard::Page::CriticalDomain;
    default:
      return -1;
  }
}

bool DataSensitivityAnalysisHSICParametersPage::validatePage()
{
  if (covarianceTableModel_->hasErrors())
  {
    errorWidget_->setTemporaryFramelessErrorMessage(tr("p must be in (0, 2]"));
    return false;
  }
  analysis_ptr_->setCovarianceModels(getCovarianceModels(), type_);
  emit pageValidated();
  return QWizardPage::validatePage();
}


} // namespace PERSALYS