//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of functional chaos
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
#include "persalys/FunctionalChaosResultWindow.hxx"

#include "persalys/FunctionalChaosAnalysis.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/ParametersTableView.hxx"
#include "persalys/MetaModelValidationWidget.hxx"
#include "persalys/SensitivityResultWidget.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/TranslationManager.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <openturns/SpecFunc.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/KPermutationsDistribution.hxx>
#include <openturns/FunctionalChaosSobolIndices.hxx>
#include <openturns/MetaModelValidation.hxx>

#include <QSplitter>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QDebug>
#include <QTextEdit>

using namespace OT;

namespace PERSALYS
{

FunctionalChaosResultWindow::FunctionalChaosResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , errorMessage_(item->getAnalysis().getWarningMessage().c_str())
{
  FunctionalChaosAnalysis * chaos(dynamic_cast<FunctionalChaosAnalysis*>(item->getAnalysis().getImplementation().get()));
  if (!chaos)
    throw InvalidArgumentException(HERE) << "FunctionalChaosResultWindow: the analysis is not a FunctionalChaosAnalysis";

  result_ = chaos->getResult();
  hasValidSobolResult_ = chaos->getDistribution().hasIndependentCopula();
  maxDegree_ = chaos->getChaosDegree();
  sparse_ = chaos->getSparseChaos();

  // parameters widget
  setParameters(item->getAnalysis(), tr("Metamodel creation parameters"));

  buildInterface();
}


inline bool varianceComparison(const std::pair<UnsignedInteger, Scalar> &a, const std::pair<UnsignedInteger, Scalar> &b)
{
  return a.second > b.second;
}


void FunctionalChaosResultWindow::buildInterface()
{
  auto * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(new TitleLabel(tr("Functional chaos"), "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#functionalchaosresult"));

  // get number of outputs
  const UnsignedInteger nbOutputs = result_.getOutputSample().getDescription().getSize();

  // main splitter
  auto * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  auto * outputsGroupBox = new QGroupBox(tr("Outputs"));
  auto * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  auto * outputsListWidget = new VariablesListWidget;
  outputsListWidget->addItems(QtOT::DescriptionToStringList(result_.getOutputSample().getDescription()));
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  auto * tabWidget = new QTabWidget;

  addMomentsTab(tabWidget, outputsListWidget, nbOutputs);
  addAdequationTab(tabWidget, outputsListWidget, nbOutputs);
  if (result_.getSobolResult().getOutputNames().getSize() == nbOutputs)
    addSobolTab(tabWidget, outputsListWidget, nbOutputs);
  if (result_.getValidations().size())
    addValidationTab(tabWidget, outputsListWidget, nbOutputs);
  if (!errorMessage_.isEmpty())
    addErrorTab(tabWidget);
  if (parametersWidget_)
    tabWidget->addTab(parametersWidget_, tr("Parameters"));
  if (modelDescriptionWidget_)
    tabWidget->addTab(modelDescriptionWidget_, tr("Model"));

  // set widgets
  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);
  outputsListWidget->setCurrentRow(0);

  widgetLayout->addWidget(mainWidget, 1);
}

void FunctionalChaosResultWindow::addMomentsTab(QTabWidget * tabWidget, const VariablesListWidget * outputsListWidget, const OT::UnsignedInteger nbOutputs)
{
  if (result_.getMean().getSize() == nbOutputs && result_.getVariance().getSize() == nbOutputs && result_.getMeanSquaredError().getSize() == nbOutputs)
  {
    QScrollArea * scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    QWidget * summaryWidget = new QWidget;
    QGridLayout * summaryWidgetLayout = new QGridLayout(summaryWidget);

    // chaos result
    ResizableStackedWidget * generalStackedWidget = new ResizableStackedWidget;
    for (UnsignedInteger outputIndex = 0; outputIndex < nbOutputs; ++outputIndex)
    {
      QStringList namesList;
      namesList << tr("MSE") << tr("R2");
      QStringList valuesList;
      valuesList << QString::number(result_.getMeanSquaredError()[outputIndex]);
      valuesList << QString::number(result_.getR2Score()[outputIndex]);
      ParametersTableView * generalTableView = new ParametersTableView(namesList, valuesList, true, true);
      generalStackedWidget->addWidget(generalTableView);
    }
    summaryWidgetLayout->addWidget(generalStackedWidget);

    QGroupBox * basisGroupBox = new QGroupBox(tr("Polynomial basis"));
    QVBoxLayout * basisGroupBoxLayout = new QVBoxLayout(basisGroupBox);
    ResizableStackedWidget * basisStackedWidget = new ResizableStackedWidget;

    if (result_.getFunctionalChaosResult().getCoefficients().getDimension() != nbOutputs)
      qDebug() << "Error: FunctionalChaosResultWindow chaos coefficients sample has not a dimension equal to the number of outputs\n";

    for (UnsignedInteger outputIndex = 0; outputIndex < nbOutputs; ++outputIndex)
    {
      // parameters names
      QStringList namesList;
      namesList << tr("Dimension")
                << tr("Maximum degree");
      namesList << (sparse_ ? tr("Full basis size") : tr("Basis size"));

      // parameters values
      const UnsignedInteger dim = result_.getFunctionalChaosResult().getDistribution().getDimension();
      const UnsignedInteger maxSize = SpecFunc::BinomialCoefficient(dim + maxDegree_, maxDegree_);

      QStringList valuesList;
      valuesList << QString::number(dim)
                 << QString::number(maxDegree_)
                 << QString::number(maxSize);

      if (sparse_)
      {
        namesList << tr("Truncated basis size");

        UnsignedInteger notNullCoefCounter = 0;
        for (UnsignedInteger coefIndex = 0; coefIndex < result_.getFunctionalChaosResult().getCoefficients().getSize(); ++coefIndex)
          if (result_.getFunctionalChaosResult().getCoefficients()(coefIndex, outputIndex) != 0.0)
            ++notNullCoefCounter;
        valuesList << QString::number(notNullCoefCounter);
      }

      // table view
      ParametersTableView * basisTableView = new ParametersTableView(namesList, valuesList, true, true);
      basisStackedWidget->addWidget(basisTableView);
    }
    basisGroupBoxLayout->addWidget(basisStackedWidget);
    connect(outputsListWidget, SIGNAL(currentRowChanged(int)), basisStackedWidget, SLOT(setCurrentIndex(int)));
    summaryWidgetLayout->addWidget(basisGroupBox);

    // moments estimates
    QGroupBox * momentsGroupBox = new QGroupBox(tr("Moments estimates"));
    QVBoxLayout * momentsGroupBoxLayout = new QVBoxLayout(momentsGroupBox);
    ResizableStackedWidget * momentsStackedWidget = new ResizableStackedWidget;

    for (UnsignedInteger outputIndex = 0; outputIndex < nbOutputs; ++outputIndex)
    {
      CopyableTableView * momentsEstimationsTableView = new CopyableTableView;
      momentsEstimationsTableView->horizontalHeader()->hide();
      momentsEstimationsTableView->verticalHeader()->hide();
      CustomStandardItemModel * momentsEstimationsTable = new CustomStandardItemModel(3, 2, momentsEstimationsTableView);
      momentsEstimationsTableView->setModel(momentsEstimationsTable);
      // - vertical header
      momentsEstimationsTable->setNotEditableHeaderItem(0, 0, tr("Estimate"));
      momentsEstimationsTable->setNotEditableHeaderItem(1, 0, tr("Mean"));
      momentsEstimationsTable->setNotEditableHeaderItem(2, 0, tr("Variance"));
      momentsEstimationsTable->setNotEditableHeaderItem(3, 0, tr("Standard deviation"));
      // - horizontal header
      momentsEstimationsTable->setNotEditableHeaderItem(0, 1, tr("Value"));
      // - moments values
      momentsEstimationsTable->setNotEditableItem(1, 1, result_.getMean()[outputIndex]);
      momentsEstimationsTable->setNotEditableItem(2, 1, result_.getVariance()[outputIndex]);
      momentsEstimationsTable->setNotEditableItem(3, 1, std::sqrt(result_.getVariance()[outputIndex]));

      momentsEstimationsTableView->resizeToContents();

      momentsStackedWidget->addWidget(momentsEstimationsTableView);
    }
    momentsGroupBoxLayout->addWidget(momentsStackedWidget);

    connect(outputsListWidget, SIGNAL(currentRowChanged(int)), momentsStackedWidget, SLOT(setCurrentIndex(int)));

    summaryWidgetLayout->addWidget(momentsGroupBox);

    // Part of variance
    // reuse of OT::FunctionalChaosSobolIndices::summary() content
    QGroupBox * varGroupBox = new QGroupBox(tr("Part of variance"));
    QVBoxLayout * varGroupBoxLayout = new QVBoxLayout(varGroupBox);
    ResizableStackedWidget * varStackedWidget = new ResizableStackedWidget;

    EnumerateFunction enumerateFunction(result_.getFunctionalChaosResult().getOrthogonalBasis().getEnumerateFunction());
    const Indices indices(result_.getFunctionalChaosResult().getIndices());
    const Sample coefficients(result_.getFunctionalChaosResult().getCoefficients());
    const UnsignedInteger basisSize = indices.getSize();

    for (UnsignedInteger outputIndex = 0; outputIndex < nbOutputs; ++outputIndex)
    {
      UnsignedInteger maxdegree = 0;

      std::vector< std::pair<UnsignedInteger, Scalar > > varianceOrder;

      // compute part of contribution of each basis term
      for (UnsignedInteger i = 1; i < basisSize; ++ i)
      {
        const Scalar coefI = coefficients(i, outputIndex);
        Indices multiIndices(enumerateFunction(indices[i]));
        UnsignedInteger degreeI = 0;
        for (UnsignedInteger k = 0; k < multiIndices.getSize(); ++ k)
          degreeI += multiIndices[k];

        maxdegree = std::max(maxdegree, degreeI);
        const Scalar varianceRatio = coefI * coefI / result_.getVariance()[outputIndex];
        varianceOrder.push_back(std::pair<UnsignedInteger, Scalar >(i, varianceRatio));
      }

      // sort basis terms by descending variance contribution
      std::sort(varianceOrder.begin(), varianceOrder.end(), varianceComparison);

      // table of part of variance for each basis term
      CopyableTableView * tableView = new CopyableTableView;
      tableView->verticalHeader()->hide();
      tableView->horizontalHeader()->hide();
      CustomStandardItemModel * tableModel = new CustomStandardItemModel(1, 3, tableView);
      tableView->setModel(tableModel);

      // row
      tableModel->setNotEditableHeaderItem(0, 0, tr("Index"));
      tableModel->setNotEditableHeaderItem(0, 1, tr("Multi-indice"));
      tableModel->setNotEditableHeaderItem(0, 2, tr("Part of variance") + "\n(" + tr("Threshold") + QString(" : %1 %").arg(ResourceMap::GetAsScalar("FunctionalChaosSobolIndices-VariancePartThreshold") * 100) + ")");

      // first coefficient
      tableModel->setNotEditableItem(1, 0, indices[0]);
      tableModel->setNotEditableItem(1, 1, enumerateFunction(indices[0]).__str__().c_str());
      tableModel->setNotEditableItem(1, 2, "-");

      // part of variance
      Scalar varPartSum = 0;
      for (UnsignedInteger i = 0; i < basisSize - 1; ++ i)
      {
        // stop when the variance contribution becomes less than epsilon
        if (varianceOrder[i].second < ResourceMap::GetAsScalar("FunctionalChaosSobolIndices-VariancePartThreshold"))
          break;

        Indices multiIndices(enumerateFunction(indices[varianceOrder[i].first]));

        tableModel->setNotEditableItem(i + 2, 0, varianceOrder[i].first);
        tableModel->setNotEditableItem(i + 2, 1, multiIndices.__str__().c_str());
        tableModel->setNotEditableItem(i + 2, 2, QString("%1 %").arg(varianceOrder[i].second * 100, 0, 'f', 2));
        varPartSum += varianceOrder[i].second * 100;
      }
      tableModel->setNotEditableHeaderItem(tableModel->rowCount(), 0, tr("Sum"));
      tableModel->setNotEditableItem(tableModel->rowCount() - 1, 2, QString("%1 %").arg(varPartSum, 0, 'f', 2));

      tableView->resizeToContents();
      varStackedWidget->addWidget(tableView);
    }
    varGroupBoxLayout->addWidget(varStackedWidget);
    connect(outputsListWidget, SIGNAL(currentRowChanged(int)), varStackedWidget, SLOT(setCurrentIndex(int)));
    summaryWidgetLayout->addWidget(varGroupBox);

    summaryWidgetLayout->setRowStretch(3, 1);

    scrollArea->setWidget(summaryWidget);
    tabWidget->addTab(scrollArea, tr("Results"));
  }
  else
  {
    if (!errorMessage_.isEmpty())
    {
      QWidget * summaryWidget = new QWidget;
      QVBoxLayout * summaryWidgetLayout = new QVBoxLayout(summaryWidget);
      QLabel * errorLabel = new QLabel(errorMessage_);
      errorLabel->setWordWrap(true);
      summaryWidgetLayout->addWidget(errorLabel);
      summaryWidgetLayout->addStretch();
      tabWidget->addTab(summaryWidget, tr("Results"));
    }
  }
}

void FunctionalChaosResultWindow::addAdequationTab(QTabWidget * tabWidget, const VariablesListWidget * outputsListWidget, const OT::UnsignedInteger nbOutputs)
{
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  ResizableStackedWidget * plotsStackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget, SIGNAL(currentRowChanged(int)), plotsStackedWidget, SLOT(setCurrentIndex(int)));
  MetaModelValidationResult fakeResu(result_.getMetaModelOutputSample(),
                                     result_.getMeanSquaredError(),
                                     result_.getR2Score());
  for (UnsignedInteger i = 0; i < nbOutputs; ++ i)
  {
    MetaModelValidationWidget * validationWidget = new MetaModelValidationWidget(fakeResu, result_.getOutputSample(), i, "", this);
    plotsStackedWidget->addWidget(validationWidget);
  }
  tabLayout->addWidget(plotsStackedWidget);

  tabWidget->addTab(plotsStackedWidget, tr("Adequation"));
}

void FunctionalChaosResultWindow::addSobolTab(QTabWidget * tabWidget, const VariablesListWidget * outputsListWidget, const OT::UnsignedInteger nbOutputs)
{
  QScrollArea * sobolScrollArea = new QScrollArea;
  sobolScrollArea->setWidgetResizable(true);
  QWidget * widget = new QWidget;
  QVBoxLayout * vbox = new QVBoxLayout(widget);
  ResizableStackedWidget * sobolStackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget, SIGNAL(currentRowChanged(int)), sobolStackedWidget, SLOT(setCurrentIndex(int)));

  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    SensitivityResultWidget * sobolResultWidget = new SensitivityResultWidget(result_.getSobolResult().getFirstOrderIndices()[i],
        Interval(),
        result_.getSobolResult().getTotalIndices()[i],
        Interval(),
        result_.getSobolResult().getInputNames(),
        result_.getSobolResult().getOutputNames()[i],
        SensitivityResultWidget::Sobol,
        this);
    sobolStackedWidget->addWidget(sobolResultWidget);
  }
  vbox->addWidget(sobolStackedWidget);

  if (!hasValidSobolResult_)
  {
    QString message{tr("The data distribution does not have an independent copula, be careful with your interpreation of the Sobol' indices")};
    auto warningTextEdit = new QTextEdit();
    warningTextEdit->setPlainText(message);
    warningTextEdit->setReadOnly(true);
    warningTextEdit->setStyleSheet("QTextEdit { color : orange; font-weight: bold; background-color: transparent; border: 1px solid orange; }");
    warningTextEdit->setFixedHeight(45);
    vbox->addWidget(warningTextEdit);
  }

  sobolScrollArea->setWidget(widget);
  tabWidget->addTab(sobolScrollArea, tr("Sobol indices"));
}

void FunctionalChaosResultWindow::addValidationTab(QTabWidget * tabWidget, const VariablesListWidget * outputsListWidget, const OT::UnsignedInteger nbOutputs)
{
  QTabWidget * validationTabWidget = new QTabWidget;

  for (UnsignedInteger i = 0; i < result_.getValidations().size(); ++i)
  {
    QWidget * tab = new QWidget;
    QVBoxLayout * tabLayout = new QVBoxLayout(tab);

    ResizableStackedWidget * plotStackedWidget = new ResizableStackedWidget;
    connect(outputsListWidget, SIGNAL(currentRowChanged(int)), plotStackedWidget, SLOT(setCurrentIndex(int)));

    // retrieve the output sample
    Sample outputSample(result_.getOutputSample());
    if (result_.getValidations()[i].getName() == "Test sample")
    {
      // search seed: we know the index of the seed but this method is more robust
      UnsignedInteger seed = 0;
      bool parameterFound = false;
      for (UnsignedInteger j = 0; j < result_.getValidations()[i].getParameters().getSize(); ++j)
      {
        if (result_.getValidations()[i].getParameters().getDescription()[j] == "Seed")
        {
          seed = result_.getValidations()[i].getParameters()[j];
          parameterFound = true;
        }
      }
      Q_ASSERT(parameterFound);
      RandomGenerator::SetSeed(seed);
      const UnsignedInteger testSampleSize = result_.getValidations()[i].getMetaModelOutputSample().getSize();
      Point indicesTestSample(KPermutationsDistribution(testSampleSize, outputSample.getSize()).getRealization());
      outputSample = Sample(testSampleSize, nbOutputs);
      outputSample.setDescription(result_.getOutputSample().getDescription());
      std::sort(indicesTestSample.begin(), indicesTestSample.end());

      for (UnsignedInteger j = 0; j < testSampleSize; ++j)
      {
        outputSample[j] = result_.getOutputSample()[indicesTestSample[j]];
      }
    }
    // validation widget
    for (UnsignedInteger j = 0; j < nbOutputs; ++j)
    {
      MetaModelValidationWidget * validationWidget = new MetaModelValidationWidget(result_.getValidations()[i],
          outputSample,
          j,
          tr("Q2 LOO"),
          this);
      plotStackedWidget->addWidget(validationWidget);
    }
    tabLayout->addWidget(plotStackedWidget);

    validationTabWidget->addTab(plotStackedWidget, TranslationManager::GetTranslatedParameterName(result_.getValidations()[i].getName()));
  }

  tabWidget->addTab(validationTabWidget, tr("Validation"));
}

void FunctionalChaosResultWindow::addErrorTab(QTabWidget * tabWidget) const
{
  QWidget * indicesWidget = new QWidget;
  QVBoxLayout * indicesWidgetLayout = new QVBoxLayout(indicesWidget);
  TemporaryLabel * errorLabel = new TemporaryLabel;
  errorLabel->setErrorMessage(errorMessage_);
  indicesWidgetLayout->addWidget(errorLabel);
  indicesWidgetLayout->addStretch();
  tabWidget->addTab(indicesWidget, tr("Error"));
}


}
