//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of linear regression
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/PolynomialRegressionResultWindow.hxx"
#include "persalys/PolynomialRegressionAnalysis.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/MetaModelValidationWidget.hxx"
#include "persalys/ParametersTableView.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/TranslationManager.hxx"
#include "persalys/PlotWidget.hxx"
#include "persalys/CopyableTableView.hxx"
#include "persalys/WidgetBoundToDockWidget.hxx"
#include "persalys/GraphConfigurationWidget.hxx"

#include <openturns/OTBase.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/KPermutationsDistribution.hxx>
#include <openturns/Normal.hxx>
#include <openturns/LinearModelAnalysis.hxx>

#include <QGroupBox>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>
#include <QLabel>
#include <QSortFilterProxyModel>

using namespace OT;

namespace PERSALYS
{

PolynomialRegressionResultWindow::PolynomialRegressionResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_()
  , errorMessage_(item->getAnalysis().getWarningMessage().c_str())
{
  const PolynomialRegressionAnalysis * analysis = dynamic_cast<const PolynomialRegressionAnalysis*>(item->getAnalysis().getImplementation().get());
  if (!analysis)
    throw InvalidArgumentException(HERE) << "PolynomialRegressionResultWindow: the analysis is not a PolynomialRegressionAnalysis";

  result_ = analysis->getResult();

  // parameters widget
  setParameters(item->getAnalysis(), tr("Metamodel creation parameters"));

  buildInterface();
}


void PolynomialRegressionResultWindow::buildInterface()
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(new TitleLabel(tr("Linear regression"), "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#linearregressionresult"));

  // get number of outputs
  const UnsignedInteger nbOutputs = result_.getOutputSample().getDimension();

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // list outputs
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  VariablesListWidget * outputsListWidget = new VariablesListWidget;
  outputsListWidget->addItems(QtOT::DescriptionToStringList(result_.getOutputSample().getDescription()));
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // tab widget
  QTabWidget * tabWidget = new QTabWidget;

  // first tab : general results --------------------------------
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  ResizableStackedWidget * resultStackedWidget = new ResizableStackedWidget;
  const PolynomialRegressionAnalysis * analysis(dynamic_cast<PolynomialRegressionAnalysis*>(dynamic_cast<AnalysisItem*>(getItem())->getAnalysis().getImplementation().get()));

  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    const Description coefficientNames(result_.getLinearModelResultCollection()[i].getCoefficientsNames());
    const Point coefficients(result_.getLinearModelResultCollection()[i].getCoefficients());

    QWidget * resultWidget = new QWidget;
    QGridLayout * resultWidgetLayout = new QGridLayout(resultWidget);

    // formula
    QStringList namesList;
    namesList << tr("Dimension") << tr("Degree") << tr("Interaction") << tr("Formula") << tr("R2") << tr("Adjusted R2");
    QStringList valuesList;
    valuesList << QString::number(analysis->getEffectiveInputSample().getDimension());
    valuesList << QString::number(analysis->getDegree());
    valuesList << QString::fromStdString(analysis->getInteraction() ? "yes" : "no");
    valuesList << QString::fromStdString(result_.getFormulas()[i]);
    valuesList << QString::number(result_.getLinearModelResultCollection()[i].getRSquared());
    valuesList << QString::number(result_.getLinearModelResultCollection()[i].getAdjustedRSquared());
    ParametersTableView * basisTableView = new ParametersTableView(namesList, valuesList, true, true);
    resultWidgetLayout->addWidget(basisTableView, 0, 0);

    // table view
    CopyableTableView * coeffTableView = new CopyableTableView;
    coeffTableView->horizontalHeader()->hide();
    coeffTableView->verticalHeader()->hide();

    // table model
    const int nbColumns = 3;
    const int nbRows = result_.getLinearModelResultCollection()[i].getCoefficients().getSize() + 1;
    CustomStandardItemModel * coeffTableModel = new CustomStandardItemModel(nbRows, nbColumns, coeffTableView);
    coeffTableView->setModel(coeffTableModel);

    const Scalar confidenceLevel = 0.9;

    // horizontal header
    coeffTableModel->setNotEditableHeaderItem(0, 0, tr("Term"));
    coeffTableModel->setNotEditableHeaderItem(0, 1, tr("Coefficient"));
    coeffTableModel->setNotEditableHeaderItem(0, 2, tr("Confidence Interval\nat") + " " + QString::number(100.0 * confidenceLevel) + "%");

    const UnsignedInteger inputDimension = result_.getLinearModelResultCollection()[i].getInputSample().getDimension();
    const Description inputDescription(result_.getLinearModelResultCollection()[i].getInputSample().getDescription());
    for (UnsignedInteger j = 0; j < coefficients.getSize(); ++ j)
    {
      // extract formula from: [x0,x1,x2]->[1], [x0,x1,x2]->[x0], [x0,x1,x2]->[x0*x1], [x0,x1,x2]->[x0^2]
      QRegularExpression rex("\\->\\[([x\\d\\^\\*]+)\\]");
      QString coeffName = QString::fromStdString(coefficientNames[j]);
      QRegularExpressionMatch match = rex.match(coeffName);
      if (match.hasMatch())
      {
        coeffName = match.captured(1);
        // replace xi with actual sample labels (backwards to avoid overlap)
        for (SignedInteger k = inputDimension - 1; k >= 0; -- k)
          coeffName = coeffName.replace(QString::fromStdString(OSS() << "x" << k), QString::fromStdString(inputDescription[k]));
      }
      if (coeffName == "1")
        coeffName = tr("intercept");
      coeffTableModel->setNotEditableItem(j + 1, 0, coeffName);

      coeffTableModel->setNotEditableItem(j + 1, 1, coefficients[j]);
      QString ciString;
      try
      {
        Normal coeffDistribution(coefficients[j], result_.getLinearModelResultCollection()[i].getCoefficientsStandardErrors()[j]);// may throw here
        const Interval ci(coeffDistribution.computeBilateralConfidenceInterval(confidenceLevel));
        ciString = QString::fromStdString(ci.__str__());
      }
      catch (const InvalidArgumentException &)
      {
        ciString = tr("undefined");
      }
      coeffTableModel->setNotEditableItem(j + 1, 2, ciString);
    }

    coeffTableView->resizeToContents();
    resultWidgetLayout->addWidget(coeffTableView, 1, 0);
    resultWidgetLayout->setRowStretch(2, 1);
    resultWidgetLayout->setColumnStretch(1, 1);
    resultStackedWidget->addWidget(resultWidget);
  }

  connect(outputsListWidget, SIGNAL(currentRowChanged(int)), resultStackedWidget, SLOT(setCurrentIndex(int)));

  scrollArea->setWidget(resultStackedWidget);
  tabWidget->addTab(scrollArea, tr("Results"));

  // second tab : METAMODEL GRAPH --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  ResizableStackedWidget * plotsStackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget, SIGNAL(currentRowChanged(int)), plotsStackedWidget, SLOT(setCurrentIndex(int)));

  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    MetaModelValidationResult fakeResu(result_.getMetaModelOutputSample(),
                                       Point(1, -1.),
                                       Point(1, -1.));
    MetaModelValidationWidget * validationWidget = new MetaModelValidationWidget(fakeResu, result_.getOutputSample(), i, "", this);
    plotsStackedWidget->addWidget(validationWidget);
  }
  tabLayout->addWidget(plotsStackedWidget);

  tabWidget->addTab(tab, tr("Adequation"));

  // third tab : VALIDATION --------------------------------
  if (result_.getValidations().size())
  {
    QTabWidget * validationTabWidget = new QTabWidget;

    // for each validation
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

  // fifth tab : residuals --------------------------------
  ResizableStackedWidget * residualStackedWidget = new ResizableStackedWidget;
  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    QTabWidget * residualTabWidget = new QTabWidget;

    QWidget * resultWidget = new QWidget;
    QGridLayout * residualWidgetLayout = new QGridLayout(resultWidget);

    const Sample observation(result_.getLinearModelResultCollection()[i].getOutputSample());
    const Function metamodel(result_.getLinearModelResultCollection()[i].getMetaModel());
    const Sample predicted(metamodel(result_.getLinearModelResultCollection()[i].getInputSample()));
    const Sample residuals(result_.getLinearModelResultCollection()[i].getSampleResiduals());
    const Point cook(result_.getLinearModelResultCollection()[i].getCookDistances());

    // table model
    const int nbColumns = 5;
    const int nbRows = residuals.getSize();
    CustomStandardItemModel * residualTableModel = new CustomStandardItemModel(nbRows, nbColumns);
    residualTableModel->setHeaderData(0, Qt::Horizontal, tr("Row ID"), Qt::DisplayRole);
    residualTableModel->setHeaderData(1, Qt::Horizontal, tr("Observation"), Qt::DisplayRole);
    residualTableModel->setHeaderData(2, Qt::Horizontal, tr("Predicted"), Qt::DisplayRole);
    residualTableModel->setHeaderData(3, Qt::Horizontal, tr("Residual"), Qt::DisplayRole);
    residualTableModel->setHeaderData(4, Qt::Horizontal, tr("Cook distances"), Qt::DisplayRole);

    for (UnsignedInteger j = 0; j < residuals.getSize(); ++ j)
    {
      residualTableModel->setNotEditableItem(j, 0, j);
      residualTableModel->setNotEditableItem(j, 1, observation(j, 0));
      residualTableModel->setNotEditableItem(j, 2, predicted(j, 0));
      residualTableModel->setNotEditableItem(j, 3, residuals(j, 0));
      residualTableModel->setNotEditableItem(j, 4, cook[j]);
    }

    QSortFilterProxyModel * residualProxyModel = new QSortFilterProxyModel;
    residualProxyModel->setSourceModel(residualTableModel);
    residualProxyModel->setSortRole(Qt::UserRole + 10);

    // table view
    CopyableTableView * residualTableView = new CopyableTableView;
    residualTableView->verticalHeader()->hide();
    residualTableView->setModel(residualProxyModel);
    residualTableView->setSortingEnabled(true);
    residualTableView->sortByColumn(0, Qt::AscendingOrder);
    residualTableView->resizeToContents();
    residualWidgetLayout->addWidget(residualTableView, 0, 0);
    residualWidgetLayout->setRowStretch(1, 1);
    residualWidgetLayout->setColumnStretch(1, 1);

    QScrollArea * residualTableScrollArea = new QScrollArea;
    residualTableScrollArea->setWidgetResizable(true);
    residualTableScrollArea->setWidget(resultWidget);

    residualTabWidget->addTab(residualTableScrollArea, tr("Table"));

    // residual distribution
    try
    {
      QGridLayout * resDistLayout = new QGridLayout;
      const Normal noiseDistribution(result_.getLinearModelResultCollection()[i].getNoiseDistribution());
      QStringList namesList(tr("Standard deviation"));
      QStringList valuesList(QString::number(noiseDistribution.getStandardDeviation()[0]));
      ParametersTableView * basisTableView = new ParametersTableView(namesList, valuesList, true, true);
      resDistLayout->addWidget(basisTableView, 0, 0);

      PlotWidget * pdfPlot = new PlotWidget(tr("Residual distribution"));
      pdfPlot->plotCurve(noiseDistribution.drawPDF().getDrawable(0).getData());
      pdfPlot->setTitle(tr("Residual PDF"));
      pdfPlot->setAxisTitle(QwtPlot::xBottom, tr("Residual"));
      pdfPlot->setAxisTitle(QwtPlot::yLeft, tr("Density"));
      SimpleGraphSetting * graphSetting = new SimpleGraphSetting(pdfPlot, this);
      resDistLayout->addWidget(new WidgetBoundToDockWidget(pdfPlot, graphSetting, this), 1, 0);
      QWidget * resDistWidget = new QWidget;
      resDistWidget->setLayout(resDistLayout);
      residualTabWidget->addTab(resDistWidget, tr("PDF"));
    }
    catch (const InvalidArgumentException &)
    {
      // pass
    }

    // residual vs fitted
    PlotWidget * resVsFitPlot = new PlotWidget(tr("Residual vs fitted"));
    const Graph resVsFitGraph(LinearModelAnalysis(result_.getLinearModelResultCollection()[i]).drawResidualsVsFitted());
    resVsFitPlot->plotScatter(resVsFitGraph.getDrawable(0).getData().getMarginal(0), resVsFitGraph.getDrawable(0).getData().getMarginal(1));
    resVsFitPlot->setTitle(tr("Residual vs fitted"));
    resVsFitPlot->setAxisTitle(QwtPlot::xBottom, tr("Fitted values"));
    resVsFitPlot->setAxisTitle(QwtPlot::yLeft, tr("Residual"));
    SimpleGraphSetting * resVsFitGraphSetting = new SimpleGraphSetting(resVsFitPlot, this);
    residualTabWidget->addTab(new WidgetBoundToDockWidget(resVsFitPlot, resVsFitGraphSetting, this), tr("Residual vs fitted"));

    // cook distances
    PlotWidget * cookPlot = new PlotWidget(tr("Cook distance"));
    Graph cookGraph(LinearModelAnalysis(result_.getLinearModelResultCollection()[i]).drawCookDistance());
    const UnsignedInteger size = cookGraph.getDrawables().getSize();
    for (UnsignedInteger j = 0; j < size; ++j)
      cookPlot->plotCurve(cookGraph.getDrawable(j).getData());
    cookPlot->setTitle(tr("Cook distance"));
    cookPlot->setAxisTitle(QwtPlot::xBottom, tr("Observation"));
    cookPlot->setAxisTitle(QwtPlot::yLeft, tr("Cook distance"));
    SimpleGraphSetting * cookGraphSetting = new SimpleGraphSetting(cookPlot, this);
    residualTabWidget->addTab(new WidgetBoundToDockWidget(cookPlot, cookGraphSetting, this), tr("Cook distance"));

    residualStackedWidget->addWidget(residualTabWidget);
  }

  connect(outputsListWidget, SIGNAL(currentRowChanged(int)), residualStackedWidget, SLOT(setCurrentIndex(int)));

//   residualScrollArea->setWidget(residualStackedWidget);
  tabWidget->addTab(residualStackedWidget, tr("Residual"));

  // tab : ERRORS --------------------------------
  if (!errorMessage_.isEmpty())
  {
    QWidget * indicesWidget = new QWidget;
    QVBoxLayout * indicesWidgetLayout = new QVBoxLayout(indicesWidget);
    QLabel * errorLabel = new QLabel(errorMessage_);
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

  widgetLayout->addWidget(mainWidget, 1);
}
}
