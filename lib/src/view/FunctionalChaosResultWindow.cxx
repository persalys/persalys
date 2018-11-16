//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of functional chaos
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/FunctionalChaosResultWindow.hxx"

#include "otgui/FunctionalChaosAnalysis.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/ParametersTableView.hxx"
#include "otgui/MetaModelValidationWidget.hxx"
#include "otgui/SensitivityResultWidget.hxx"
#include "otgui/QtTools.hxx"
#include "otgui/TranslationManager.hxx"

#include <openturns/SpecFunc.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/KPermutationsDistribution.hxx>
#include <openturns/FunctionalChaosSobolIndices.hxx>

#include <QSplitter>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>

using namespace OT;

namespace OTGUI
{

FunctionalChaosResultWindow::FunctionalChaosResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_()
  , hasValidSobolResult_(true)
  , maxDegree_(0)
  , sparse_(false)
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
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  // get number of outputs
  const UnsignedInteger nbOutputs = result_.getOutputSample().getDescription().getSize();

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  VariablesListWidget * outputsListWidget = new VariablesListWidget;
  outputsListWidget->addItems(QtOT::DescriptionToStringList(result_.getOutputSample().getDescription()));
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // - tab widget
  QTabWidget * tabWidget = new QTabWidget;

  // first tab : METAMODEL GRAPH --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  ResizableStackedWidget * plotsStackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget, SIGNAL(currentRowChanged(int)), plotsStackedWidget, SLOT(setCurrentIndex(int)));
  MetaModelValidationResult fakeResu(result_.getMetaModelOutputSample(),
                                     result_.getFunctionalChaosResult().getRelativeErrors(),
                                     result_.getFunctionalChaosResult().getResiduals());
  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    MetaModelValidationWidget * validationWidget = new MetaModelValidationWidget(fakeResu,
        result_.getOutputSample(),
        i,
        tr("Relative error"),
        this);

    plotsStackedWidget->addWidget(validationWidget);
  }
  tabLayout->addWidget(plotsStackedWidget);

  tabWidget->addTab(tab, tr("Metamodel"));

  // second tab : MOMENTS --------------------------------
  if (result_.getMean().getSize() == nbOutputs && result_.getVariance().getSize() == nbOutputs)
  {
    QScrollArea * scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    QWidget * summaryWidget = new QWidget;
    QGridLayout * summaryWidgetLayout = new QGridLayout(summaryWidget);

    // moments estimates
    QGroupBox * momentsGroupBox = new QGroupBox(tr("Moments estimates"));
    QVBoxLayout * momentsGroupBoxLayout = new QVBoxLayout(momentsGroupBox);
    ResizableStackedWidget * momentsStackedWidget = new ResizableStackedWidget;

    for (UnsignedInteger outputIndex = 0; outputIndex < nbOutputs; ++outputIndex)
    {
      ResizableTableViewWithoutScrollBar * momentsEstimationsTableView = new ResizableTableViewWithoutScrollBar;
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

    // chaos result
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
      ResizableTableViewWithoutScrollBar * tableView = new ResizableTableViewWithoutScrollBar;
      tableView->verticalHeader()->hide();
      tableView->horizontalHeader()->hide();
      CustomStandardItemModel * tableModel = new CustomStandardItemModel(1, 4, tableView);
      tableView->setModel(tableModel);

      // row
      tableModel->setNotEditableHeaderItem(0, 0, tr("Index"));
      tableModel->setNotEditableHeaderItem(0, 1, tr("Multi-indice"));
      tableModel->setNotEditableHeaderItem(0, 2, tr("Coefficient"));
      tableModel->setNotEditableHeaderItem(0, 3, tr("Part of variance") + "\n(" + tr("Threshold") + QString(" : %1 %").arg(ResourceMap::GetAsScalar("FunctionalChaosSobolIndices-VariancePartThreshold") * 100) + ")");

      // first coefficient
      tableModel->setNotEditableItem(1, 0, indices[0]);
      tableModel->setNotEditableItem(1, 1, enumerateFunction(indices[0]).__str__().c_str());
      tableModel->setNotEditableItem(1, 2, coefficients(0, outputIndex));
      tableModel->setNotEditableItem(1, 3, "-");

      // part of variance
      Scalar varPartSum = 0;
      for (UnsignedInteger i = 0; i < basisSize - 1; ++ i)
      {
        // stop when the variance contribution becomes less than epsilon
        if (varianceOrder[i].second < ResourceMap::GetAsScalar("FunctionalChaosSobolIndices-VariancePartThreshold"))
          break;

        Indices multiIndices(enumerateFunction(indices[varianceOrder[i].first]));

        tableModel->setNotEditableItem(i+2, 0, varianceOrder[i].first);
        tableModel->setNotEditableItem(i+2, 1, multiIndices.__str__().c_str());
        tableModel->setNotEditableItem(i+2, 2, coefficients(i+1, outputIndex));
        tableModel->setNotEditableItem(i+2, 3, QString("%1 %").arg(varianceOrder[i].second * 100, 0, 'f', 2));
        varPartSum += varianceOrder[i].second * 100;
      }
      tableModel->setNotEditableHeaderItem(tableModel->rowCount(), 0, tr("Sum"));
      tableModel->setNotEditableItem(tableModel->rowCount()-1, 3, QString("%1 %").arg(varPartSum, 0, 'f', 2));

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

  // third tab : SOBOL INDICES --------------------------------
  if (hasValidSobolResult_ && result_.getSobolResult().getOutputNames().getSize() == nbOutputs)
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
    sobolScrollArea->setWidget(widget);
    tabWidget->addTab(sobolScrollArea, tr("Sobol indices"));
  }

  // fourth tab : VALIDATION --------------------------------
  if (result_.getValidations().size())
  {
    QTabWidget * validationTabWidget = new QTabWidget;

    for (UnsignedInteger i = 0; i < result_.getValidations().size(); ++i)
    {
      tab = new QWidget;
      tabLayout = new QVBoxLayout(tab);

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
            tr("Q2"),
            this);
        plotStackedWidget->addWidget(validationWidget);
      }
      tabLayout->addWidget(plotStackedWidget);

      validationTabWidget->addTab(plotStackedWidget, TranslationManager::GetTranslatedParameterName(result_.getValidations()[i].getName()));
    }

    tabWidget->addTab(validationTabWidget, tr("Validation"));
  }

  // tab : ERRORS --------------------------------
  if (!errorMessage_.isEmpty() || !hasValidSobolResult_)
  {
    QWidget * indicesWidget = new QWidget;
    QVBoxLayout * indicesWidgetLayout = new QVBoxLayout(indicesWidget);
    QLabel * errorLabel = new QLabel((!errorMessage_.isEmpty() ? errorMessage_ + "\n" : "") + tr("As the model has not an independent copula, the interpretation of the Sobol indices may be misleading. So they are not displayed."));
    errorLabel->setWordWrap(true);
    indicesWidgetLayout->addWidget(errorLabel);
    indicesWidgetLayout->addStretch();
    tabWidget->addTab(indicesWidget, tr("Error"));
  }

  // tab : PARAMETERS --------------------------------
  if (parametersWidget_)
    tabWidget->addTab(parametersWidget_, tr("Parameters"));

  // tab : model description --------------------------------
  if (modelDescriptionWidget_)
    tabWidget->addTab(modelDescriptionWidget_, tr("Model"));

  // set widgets
  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);
  outputsListWidget->setCurrentRow(0);

  widgetLayout->addWidget(mainWidget);
}
}
