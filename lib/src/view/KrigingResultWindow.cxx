//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of functional chaos
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/KrigingResultWindow.hxx"

#include "persalys/KrigingAnalysis.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/MetaModelValidationWidget.hxx"
#include "persalys/ParametersTableView.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/TranslationManager.hxx"

#include <openturns/OTBase.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/KPermutationsDistribution.hxx>

#include <QGroupBox>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>
#include <QLabel>

using namespace OT;

namespace PERSALYS
{

KrigingResultWindow::KrigingResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_()
  , optimizeParameters_(true)
  , errorMessage_(item->getAnalysis().getWarningMessage().c_str())
{
  const KrigingAnalysis * kriging = dynamic_cast<const KrigingAnalysis*>(item->getAnalysis().getImplementation().get());
  if (!kriging)
    throw InvalidArgumentException(HERE) << "KrigingResultWindow: the analysis is not a KrigingAnalysis";

  result_ = kriging->getResult();
  optimizeParameters_ = kriging->getOptimizeParameters();

  // parameters widget
  setParameters(item->getAnalysis(), tr("Metamodel creation parameters"));

  buildInterface();
}


void KrigingResultWindow::buildInterface()
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(new TitleLabel(tr("Kriging"), "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#krigingresult"));

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

  // first tab : METAMODEL RESULT --------------------------------
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  ResizableStackedWidget * resultStackedWidget = new ResizableStackedWidget;

  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    QWidget * resultWidget = new QWidget;
    QGridLayout * resultWidgetLayout = new QGridLayout(resultWidget);

    // if covariance model parameters has been optimized
    if (optimizeParameters_)
    {
      // covariance model optimized parameters
      QStringList namesList;
      namesList << tr("Scale")
                << tr("Amplitude");

      QStringList valuesList;
      valuesList << QtOT::PointToString(result_.getKrigingResultCollection()[i].getCovarianceModel().getScale())
                 << QtOT::PointToString(result_.getKrigingResultCollection()[i].getCovarianceModel().getAmplitude());

      ParametersWidget * table = new ParametersWidget(tr("Optimized covariance model parameters"), namesList, valuesList, true, true);
      resultWidgetLayout->addWidget(table);
    }

    // trend coef
    QString trendCoefText = QtOT::PointToString(result_.getKrigingResultCollection()[i].getTrendCoefficients()[0]);
    ParametersWidget * trendCoefTable = new ParametersWidget(tr("Trend"), QStringList() << tr("Trend coefficients"), QStringList() << trendCoefText, true, true);
    resultWidgetLayout->addWidget(trendCoefTable);
    resultWidgetLayout->setRowStretch(optimizeParameters_ ? 2 : 1, 1);

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

  for (UnsignedInteger i = 0; i < nbOutputs; ++ i)
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
