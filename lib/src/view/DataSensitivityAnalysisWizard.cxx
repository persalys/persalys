//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a data sensitivity analysis
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/ComboBoxDelegate.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QTableView>
#include <QHeaderView>

using namespace OT;

namespace PERSALYS
{

/********************************* Wizard *************************************/

DataSensitivityAnalysisWizard::DataSensitivityAnalysisWizard(const Analysis& analysis, QWidget* parent)
: AnalysisWizard(analysis, parent)
{
  setWindowTitle(tr("Data sensitivity analysis"));
  docLink_ = "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#sensitivityAnalysis";

  analysis_ptr_ = dynamic_cast<DataSensitivityAnalysis *>(analysis.getImplementation().get());

  if (!analysis_ptr_)
    throw InvalidArgumentException(HERE) << "DataSensitivityAnalysisWizard received an analysis of type " << analysis.getClassName();
  
  introPage_ = new DataSensitivityAnalysisIntroPage(analysis_ptr_->getDesignOfExperiment().getType());
  introPage_->initialize(analysis_ptr_);
  setPage(Page::Intro, introPage_);

  const auto inDesc = analysis_ptr_->getDesignOfExperiment().getInputSample().getDescription();
  const auto outDesc = analysis_ptr_->getDesignOfExperiment().getOutputSample().getDescription();
  OT::Description variableNames;
  for (OT::UnsignedInteger i = 0; i < inDesc.getSize(); ++i)
    variableNames.add(inDesc[i]);
  for (OT::UnsignedInteger i = 0; i < outDesc.getSize(); ++i)
    variableNames.add(outDesc[i]);

  hsciparametersPage_ = new DataSensitivityAnalysisHSCIParametersPage(
    analysis_ptr_->getDesignOfExperiment().getSample().getSize(), variableNames);
  hsciparametersPage_->initialize(analysis_ptr_);
  setPage(Page::HSCIParameters, hsciparametersPage_);
}

Analysis DataSensitivityAnalysisWizard::getAnalysis() const
{
  const unsigned char type = introPage_->getType();

  OT::Collection<OT::CovarianceModel> covModels;
  if (type & DataSensitivityAnalysisResult::GlobalHSIC)
    covModels = hsciparametersPage_->getCovarianceModels();

  DataSensitivityAnalysis analysis(
    analysis_ptr_->getName(),
    analysis_ptr_->getDesignOfExperiment(),
    type,
    covModels
  );

  analysis.setHSICParameters(
    hsciparametersPage_->computeAsymptoticPValues(), 
    hsciparametersPage_->computePermutationPValues(), 
    hsciparametersPage_->useUStatistic()
  );

  analysis.setInterestVariables(introPage_->getInterestVariables());

  return analysis;
}

int DataSensitivityAnalysisWizard::nextId() const
{
  if (currentId() == Page::Intro)
    return introPage_->nextId();
  else
    return -1;
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
  auto * methodGroupBox = new QGroupBox(tr("Global sensitivity methods"));
  auto * methodLayout = new QVBoxLayout(methodGroupBox);

  methodGroup_ = new QButtonGroup(this);
  methodGroup_->setExclusive(false);

  // Sobol
  auto * SobolCB = new QCheckBox(tr("Sobol indices (rank sobol algorithm)"));
  methodGroup_->addButton(SobolCB, DataSensitivityAnalysisResult::RankSobol);
  SobolCB->setChecked(doeType == DesignOfExperiment::Type::MC);
  methodLayout->addWidget(SobolCB);

  // SRC
  auto * SRCCB = new QCheckBox(tr("Standard regression coefficients"));
  methodGroup_->addButton(SRCCB, DataSensitivityAnalysisResult::SRC);
  SRCCB->setChecked(true);
  methodLayout->addWidget(SRCCB);

  // Global HSIC
  auto * GlobalHSICCB = new QCheckBox(tr("Global HSIC"));
  GlobalHSICCB->setChecked(false);
  methodGroup_->addButton(GlobalHSICCB, DataSensitivityAnalysisResult::GlobalHSIC);
  // setFinalPage triggers QWizard::updateButtonLayout(), which re-evaluates
  // nextId() and updates Next/Finish button visibility accordingly.
  // completeChanged alone only updates button enabled state, not visibility.
  connect(GlobalHSICCB, &QCheckBox::toggled, this, [this](bool checked) {
    setFinalPage(!checked);
  });
  methodLayout->addWidget(GlobalHSICCB);

  pageLayout->addWidget(methodGroupBox);

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

  connect(outputsSelectionGroupBox_, &OutputsSelectionGroupBox::outputsSelectionChanged, this, [this, analysis_ptr]() {
    analysis_ptr->setInterestVariables(getInterestVariables());
  });

  methodGroup_->button(DataSensitivityAnalysisResult::RankSobol)->setChecked(analysis_ptr->computeRankSobol());
  methodGroup_->button(DataSensitivityAnalysisResult::SRC)->setChecked(analysis_ptr->computeSRC());
  methodGroup_->button(DataSensitivityAnalysisResult::GlobalHSIC)->setChecked(analysis_ptr->computeGlobalHSIC());
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
    return DataSensitivityAnalysisWizard::Page::HSCIParameters;
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

  return QWizardPage::validatePage();
}

/**************************** HSIC Parameters page ****************************/

DataSensitivityAnalysisHSCIParametersPage::DataSensitivityAnalysisHSCIParametersPage(OT::UnsignedInteger sampleSize, const OT::Description & variableNames, QWidget* parent)
: QWizardPage(parent)
{
  setTitle((tr("HSIC parameters")));
  auto * pageLayout = new QVBoxLayout(this);

  useUStatisticComboBox_ = new QComboBox;
  useUStatisticComboBox_->addItem(tr("Use V-statistic (biased but faster and asymptotically unbiased)"), false);
  useUStatisticComboBox_->addItem(tr("Use U-statistic (unbiased but more computationally expensive)"), true);
  pageLayout->addWidget(useUStatisticComboBox_);

  computePermutationPValuesCheckBox_ = new QCheckBox(tr("Compute permutation p-values"));
  computePermutationPValuesCheckBox_->setChecked(sampleSize < 100);
  pageLayout->addWidget(computePermutationPValuesCheckBox_);

  computeAsymptoticPValuesCheckBox_ = new QCheckBox(tr("Compute asymptotic p-values"));
  computeAsymptoticPValuesCheckBox_->setChecked(sampleSize >= 100);
  pageLayout->addWidget(computeAsymptoticPValuesCheckBox_);

  // Covariance models table
  auto * covGroupBox = new QGroupBox(tr("Covariance models"));
  auto * covLayout = new QVBoxLayout(covGroupBox);

  covarianceTableModel_ = new HSICCovarianceModelsTableModel(variableNames, this);

  covarianceTableView_ = new QTableView;
  covarianceTableView_->setModel(covarianceTableModel_);
  covarianceTableView_->setItemDelegateForColumn(1, new ComboBoxDelegate(covarianceTableView_));
  covarianceTableView_->setItemDelegateForColumn(2, new ComboBoxDelegate(covarianceTableView_));
  covarianceTableView_->verticalHeader()->hide();
  covarianceTableView_->horizontalHeader()->setStretchLastSection(true);
  covarianceTableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  // Open persistent editors so combo boxes are always active (single-click)
  auto openAllPersistentEditors = [this]() {
    for (int row = 0; row < covarianceTableModel_->rowCount(); ++row)
    {
      covarianceTableView_->openPersistentEditor(covarianceTableModel_->index(row, 1));
      covarianceTableView_->openPersistentEditor(covarianceTableModel_->index(row, 2));
    }
  };
  openAllPersistentEditors();

  // Re-open persistent editors after a model reset (e.g. setCovarianceModels)
  connect(covarianceTableModel_, &QAbstractItemModel::modelReset, this, openAllPersistentEditors);

  // Refresh persistent editors on column 2 when the covariance model changes,
  // since the ν combo is only enabled for Matérn
  connect(covarianceTableModel_, &QAbstractItemModel::dataChanged, covarianceTableView_, [this](const QModelIndex & topLeft, const QModelIndex & bottomRight) {
    for (int row = topLeft.row(); row <= bottomRight.row(); ++row)
    {
      const QModelIndex nuIdx = covarianceTableModel_->index(row, 2);
      covarianceTableView_->closePersistentEditor(nuIdx);
      covarianceTableView_->openPersistentEditor(nuIdx);
    }
  });

  covLayout->addWidget(covarianceTableView_);
  pageLayout->addWidget(covGroupBox);
}

void DataSensitivityAnalysisHSCIParametersPage::initialize(const DataSensitivityAnalysis * analysis_ptr)
{
  if (!analysis_ptr)
    return;

  if (analysis_ptr->defaultHSICParametersChanged())
  {
    computeAsymptoticPValuesCheckBox_->setChecked(analysis_ptr->computeAsymptoticPValues());
    computePermutationPValuesCheckBox_->setChecked(analysis_ptr->computePermutationPValues());
    useUStatisticComboBox_->setCurrentIndex(analysis_ptr->useUStatistic() ? 1 : 0);
  }
  if (!analysis_ptr->getCovarianceModels().isEmpty())
  {
    covarianceTableModel_->setCovarianceModels(analysis_ptr->getCovarianceModels());
  }
}

bool DataSensitivityAnalysisHSCIParametersPage::computeAsymptoticPValues() const
{
  return computeAsymptoticPValuesCheckBox_->isChecked();
}

bool DataSensitivityAnalysisHSCIParametersPage::computePermutationPValues() const
{
  return computePermutationPValuesCheckBox_->isChecked();
}

bool DataSensitivityAnalysisHSCIParametersPage::useUStatistic() const
{
  return useUStatisticComboBox_->currentData().toBool();
}

OT::Collection<OT::CovarianceModel> DataSensitivityAnalysisHSCIParametersPage::getCovarianceModels() const
{
  return covarianceTableModel_->getCovarianceModels();
}

} // namespace PERSALYS