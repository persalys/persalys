//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of functional chaos
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/KrigingResultWindow.hxx"

#include "otgui/KrigingAnalysis.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/MetaModelValidationWidget.hxx"
#include "otgui/ParametersTableView.hxx"
#include "otgui/QtTools.hxx"

#include <openturns/OTBase.hxx>

#include <QGroupBox>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>

using namespace OT;

namespace OTGUI
{

KrigingResultWindow::KrigingResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_()
  , optimizeParameters_(true)
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
  setWindowTitle(tr("Kriging results"));

  // get number of outputs
  const UnsignedInteger nbOutputs = result_.getOutputSample().getDimension();

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // list outputs
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  OTguiListWidget * outputsListWidget = new OTguiListWidget;
  outputsListWidget->addItems(QtOT::DescriptionToStringList(result_.getOutputSample().getDescription()));
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // tab widget
  QTabWidget * tabWidget = new QTabWidget;

  // first tab : METAMODEL GRAPH --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  ResizableStackedWidget * plotsStackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget, SIGNAL(currentRowChanged(int)), plotsStackedWidget, SLOT(setCurrentIndex(int)));

  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    MetaModelValidationWidget * validationWidget = new MetaModelValidationWidget(result_.getMetaModelOutputSample().getMarginal(i),
        result_.getOutputSample().getMarginal(i),
        -1.0,
        -1.0,
        "",
        this);

    plotsStackedWidget->addWidget(validationWidget);
  }
  tabLayout->addWidget(plotsStackedWidget);

  tabWidget->addTab(tab, tr("Metamodel"));

  // second tab : METAMODEL RESULT --------------------------------
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
      // scale
      QString scaleText;
      for (UnsignedInteger j = 0; j < result_.getKrigingResultCollection()[i].getCovarianceModel().getScale().getSize(); ++j)
      {
        scaleText += QString::number(result_.getKrigingResultCollection()[i].getCovarianceModel().getScale()[j]);
        if (j < result_.getKrigingResultCollection()[i].getCovarianceModel().getScale().getSize() - 1)
          scaleText += "; ";
      }
      valuesList << scaleText;

      // amplitude
      QString amplitudeText;
      for (UnsignedInteger j = 0; j < result_.getKrigingResultCollection()[i].getCovarianceModel().getAmplitude().getSize(); ++j)
      {
        amplitudeText += QString::number(result_.getKrigingResultCollection()[i].getCovarianceModel().getAmplitude()[j]);
        if (j < result_.getKrigingResultCollection()[i].getCovarianceModel().getAmplitude().getSize() - 1)
          amplitudeText += "; ";
      }
      valuesList << amplitudeText;

      ParametersWidget * table = new ParametersWidget(tr("Optimized covariance model parameters"), namesList, valuesList, true, true);
      resultWidgetLayout->addWidget(table);
    }

    QString trendCoefText;
    for (UnsignedInteger j = 0; j < result_.getKrigingResultCollection()[i].getTrendCoefficients()[0].getSize(); ++j)
    {
      trendCoefText += QString::number(result_.getKrigingResultCollection()[i].getTrendCoefficients()[0][j]);
      if (j < result_.getKrigingResultCollection()[i].getTrendCoefficients()[0].getSize() - 1)
        trendCoefText += "; ";
    }
    ParametersWidget * trendCoefTable = new ParametersWidget(tr("Trend"), QStringList() << tr("Trend coefficients"), QStringList() << trendCoefText, true, true);
    resultWidgetLayout->addWidget(trendCoefTable);
    resultWidgetLayout->setRowStretch(optimizeParameters_ ? 2 : 1, 1);

    resultStackedWidget->addWidget(resultWidget);
  }

  connect(outputsListWidget, SIGNAL(currentRowChanged(int)), resultStackedWidget, SLOT(setCurrentIndex(int)));

  scrollArea->setWidget(resultStackedWidget);
  tabWidget->addTab(scrollArea, tr("Results"));

  // third tab : GRAPH METAMODEL LOO --------------------------------
  if (result_.getMetaModelOutputSampleLeaveOneOut().getSize())
  {
    QTabWidget * validationTabWidget = new QTabWidget;

    tab = new QWidget;
    tabLayout = new QVBoxLayout(tab);

    ResizableStackedWidget * plotsLOOStackedWidget = new ResizableStackedWidget;
    connect(outputsListWidget, SIGNAL(currentRowChanged(int)), plotsLOOStackedWidget, SLOT(setCurrentIndex(int)));

    for (UnsignedInteger i = 0; i < nbOutputs; ++i)
    {
      MetaModelValidationWidget * validationWidget = new MetaModelValidationWidget(result_.getMetaModelOutputSampleLeaveOneOut().getMarginal(i),
          result_.getOutputSample().getMarginal(i),
          result_.getErrorQ2LeaveOneOut()[i],
          result_.getQ2LeaveOneOut()[i],
          tr("Q2"),
          this);
      plotsLOOStackedWidget->addWidget(validationWidget);
    }
    tabLayout->addWidget(plotsLOOStackedWidget);

    validationTabWidget->addTab(plotsLOOStackedWidget, tr("Leave-one-out"));
    tabWidget->addTab(validationTabWidget, tr("Validation"));
  }

  // fourth tab : PARAMETERS --------------------------------
  if (parametersWidget_)
    tabWidget->addTab(parametersWidget_, tr("Parameters"));

  // set widgets
  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);
  outputsListWidget->setCurrentRow(0);
  setWidget(mainWidget);
}
}
