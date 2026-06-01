//                                               -*- C++ -*-
/**
 *  @brief QWizard to choose a global HSIC result
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
#include "persalys/HSICResultWizard.hxx"

#include "persalys/DataSensitivityAnalysis.hxx"
#include "persalys/QtTools.hxx"

#include <QGridLayout>
#include <QHeaderView>

using namespace OT;

namespace PERSALYS
{

HSICResultWizard::HSICResultWizard(const Study& study, const PhysicalModel& model, QWidget* parent)
  : Wizard(parent)
  , study_(study)
  , model_(model)
{
  buildInterface();
}


void HSICResultWizard::buildInterface()
{
  setWindowTitle(tr("Global HSIC analysis results"));

  auto * page = new QWizardPage(this);
  auto * mainLayout = new QGridLayout(page);

  // choose DataSensitivityAnalysis
  analysisComboBox_ = new QComboBox;
  mainLayout->addWidget(new QLabel(tr("HSIC analysis")), 0, 0);
  mainLayout->addWidget(analysisComboBox_, 0, 1);

  for (UnsignedInteger i = 0; i < study_.getAnalyses().getSize(); ++i)
  {
    if (study_.getAnalyses()[i].getImplementation()->getClassName() == "DataSensitivityAnalysis")
    {
      const auto * analysis = dynamic_cast<DataSensitivityAnalysis*>(study_.getAnalyses()[i].getImplementation().get());
      if (analysis->hasValidResult()
          && analysis->computeHSIC(DataSensitivityAnalysisResult::Global)
          && (analysis->computeAsymptoticPValues(DataSensitivityAnalysisResult::Global)
              || analysis->computePermutationPValues(DataSensitivityAnalysisResult::Global)))
      {
        analysisComboBox_->addItem(QString::fromUtf8(study_.getAnalyses()[i].getName().c_str()), (int)i);
      }
    }
  }

  // choose output variable
  variablesComboBox_ = new QComboBox;
  mainLayout->addWidget(new QLabel(tr("Output variable")), 1, 0);
  mainLayout->addWidget(variablesComboBox_, 1, 1);
  updateVariablesComboBox(0);

  // table
  tableView_ = new QTableView;
  tableView_->horizontalHeader()->setStretchLastSection(true);
  tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tableView_->setSelectionMode(QAbstractItemView::NoSelection);
  mainLayout->addWidget(tableView_, 2, 0, 1, 2);

  tableModel_ = new QStandardItemModel(tableView_);
  tableView_->setModel(tableModel_);

  // p-value threshold
  thresholdSpinBox_ = new DoubleSpinBox;
  thresholdSpinBox_->setRange(0.0, 1.0);
  thresholdSpinBox_->setSingleStep(0.01);
  thresholdSpinBox_->setDecimals(4);
  thresholdSpinBox_->setValue(0.05);
  mainLayout->addWidget(new QLabel(tr("p-value threshold")), 3, 0);
  mainLayout->addWidget(thresholdSpinBox_, 3, 1);

  // error message
  errorWidget_ = new ErrorWidget;
  mainLayout->addWidget(errorWidget_, 4, 0, 1, 2);

  // connections
  connect(variablesComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &HSICResultWizard::updateTable);
  connect(analysisComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &HSICResultWizard::updateVariablesComboBox);
  connect(thresholdSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double) {
    updateTable(variablesComboBox_ ? variablesComboBox_->currentIndex() : 0);
  });

  updateTable(0);

  addPage(page);
}


void HSICResultWizard::updateVariablesComboBox(int currentAnalysis)
{
  if (!(variablesComboBox_ && analysisComboBox_))
    return;

  variablesComboBox_->clear();

  if (analysisComboBox_->count())
  {
    const int analysisIndex = analysisComboBox_->itemData(currentAnalysis).toInt();
    const auto * analysis = dynamic_cast<DataSensitivityAnalysis*>(study_.getAnalyses()[analysisIndex].getImplementation().get());
    const DataSensitivityAnalysisResult& result = analysis->getResult();

    const Description interestVars = result.getInterestVariables();
    for (UnsignedInteger i = 0; i < interestVars.getSize(); ++i)
      variablesComboBox_->addItem(QString::fromUtf8(interestVars[i].c_str()));
  }
}


void HSICResultWizard::updateTable(int index)
{
  if (!analysisComboBox_ || !variablesComboBox_ || !tableModel_)
    return;

  clearErrorMessage();
  tableModel_->clear();

  if (!analysisComboBox_->count() || index < 0)
    return;

  const int analysisIndex = analysisComboBox_->itemData(analysisComboBox_->currentIndex()).toInt();
  const auto * analysis = dynamic_cast<DataSensitivityAnalysis*>(study_.getAnalyses()[analysisIndex].getImplementation().get());
  const DataSensitivityAnalysisResult& result = analysis->getResult();

  const bool hasAsymptotic = result.computeHSICPValuesAsymptotic(DataSensitivityAnalysisResult::Global);
  const bool hasPermutation = result.computeHSICPValuesPermutation(DataSensitivityAnalysisResult::Global);

  // build header
  QStringList headers;
  headers << tr("Variable");
  if (hasAsymptotic)
    headers << tr("p-value (asymptotic)");
  if (hasPermutation)
    headers << tr("p-value (permutation)");
  headers << tr("Selected");

  tableModel_->setHorizontalHeaderLabels(headers);

  // get input variable names from the design of experiment
  const Description inputNames = result.getDesignOfExperiment().getInputSample().getDescription();
  const UnsignedInteger nbInputs = inputNames.getSize();

  // get p-values for the selected output variable
  Collection<Point> pValuesAsymptotic;
  Collection<Point> pValuesPermutation;
  if (hasAsymptotic)
    pValuesAsymptotic = result.getPValuesAsymptotic(DataSensitivityAnalysisResult::Global);
  if (hasPermutation)
    pValuesPermutation = result.getPValuesPermutation(DataSensitivityAnalysisResult::Global);

  const auto outputIndex = static_cast<UnsignedInteger>(index);

  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    QList<QStandardItem*> row;

    // variable name
    auto * nameItem = new QStandardItem(QString::fromUtf8(inputNames[i].c_str()));
    row << nameItem;

    // determine min p-value for selection criterion
    double minPValue = 1.0;

    if (hasAsymptotic && outputIndex < pValuesAsymptotic.getSize())
    {
      double pval = pValuesAsymptotic[outputIndex][i];
      auto * item = new QStandardItem(QString::number(pval, 'g', 4));
      row << item;
      if (pval < minPValue) minPValue = pval;
    }

    if (hasPermutation && outputIndex < pValuesPermutation.getSize())
    {
      double pval = pValuesPermutation[outputIndex][i];
      auto * item = new QStandardItem(QString::number(pval, 'g', 4));
      row << item;
      if (pval < minPValue) minPValue = pval;
    }

    // selected: p-value below threshold means the variable is significant (should remain stochastic)
    const bool isSignificant = (minPValue < thresholdSpinBox_->value());
    auto * selectedItem = new QStandardItem(isSignificant ? tr("Yes") : tr("No"));
    row << selectedItem;

    tableModel_->appendRow(row);
  }

  tableView_->resizeColumnsToContents();
}


Indices HSICResultWizard::getInputsSelection() const
{
  if (!analysisComboBox_ || !variablesComboBox_ || !analysisComboBox_->count())
    return Indices();

  const int analysisIndex = analysisComboBox_->itemData(analysisComboBox_->currentIndex()).toInt();
  const auto * analysis = dynamic_cast<DataSensitivityAnalysis*>(study_.getAnalyses()[analysisIndex].getImplementation().get());
  const DataSensitivityAnalysisResult& result = analysis->getResult();

  const bool hasAsymptotic = result.computeHSICPValuesAsymptotic(DataSensitivityAnalysisResult::Global);
  const bool hasPermutation = result.computeHSICPValuesPermutation(DataSensitivityAnalysisResult::Global);

  const UnsignedInteger outputIndex = static_cast<UnsignedInteger>(variablesComboBox_->currentIndex());
  const UnsignedInteger nbInputs = result.getDesignOfExperiment().getInputSample().getDimension();

  Collection<Point> pValuesAsymptotic;
  Collection<Point> pValuesPermutation;
  if (hasAsymptotic)
    pValuesAsymptotic = result.getPValuesAsymptotic(DataSensitivityAnalysisResult::Global);
  if (hasPermutation)
    pValuesPermutation = result.getPValuesPermutation(DataSensitivityAnalysisResult::Global);

  // Build selection: 1 = significant (p < 0.05), 0 = not significant
  // This matches the Morris convention where selected inputs stay stochastic
  Indices selection(model_.getInputDimension(), 0);
  const Description modelInputNames = model_.getInputNames();
  const Description analysisInputNames = result.getDesignOfExperiment().getInputSample().getDescription();

  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    double minPValue = 1.0;
    if (hasAsymptotic && outputIndex < pValuesAsymptotic.getSize())
      minPValue = std::min(minPValue, pValuesAsymptotic[outputIndex][i]);
    if (hasPermutation && outputIndex < pValuesPermutation.getSize())
      minPValue = std::min(minPValue, pValuesPermutation[outputIndex][i]);

    const double threshold = thresholdSpinBox_ ? thresholdSpinBox_->value() : 0.05;
    // find the matching model input by name
    for (UnsignedInteger j = 0; j < modelInputNames.getSize(); ++j)
    {
      if (modelInputNames[j] == analysisInputNames[i])
      {
        selection[j] = (minPValue < threshold) ? 1 : 0;
        break;
      }
    }
  }

  return selection;
}


void HSICResultWizard::clearErrorMessage()
{
  if (errorWidget_)
    errorWidget_->reset();
}


bool HSICResultWizard::validateCurrentPage()
{
  if (!analysisComboBox_ || !analysisComboBox_->count())
  {
    errorWidget_->setMessage(tr("The current study has no global HSIC analysis result with p-values."));
    return false;
  }

  return QWizard::validateCurrentPage();
}
}
