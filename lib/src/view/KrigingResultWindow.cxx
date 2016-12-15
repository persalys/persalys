//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of functional chaos
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/FunctionalChaosResultWindow.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/ResizableTableViewWithoutScrollBar.hxx"

#include "openturns/OTBase.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QScrollArea>

using namespace OT;

namespace OTGUI {

KrigingResultWindow::KrigingResultWindow(AnalysisItem * item)
  : ResultWindow(item)
  , result_(dynamic_cast<KrigingAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
  , metaModelPlotsConfigurationWidget_(0)
  , metaModelPlotsLOOConfigurationWidget_(0)
  , inSample_(dynamic_cast<KrigingAnalysis*>(&*item->getAnalysis().getImplementation())->getDesignOfExperiment().getInputSample())
{
  setParameters(item->getAnalysis());
  buildInterface();
}


KrigingResultWindow::~KrigingResultWindow()
{
  delete metaModelPlotsConfigurationWidget_;
  delete metaModelPlotsLOOConfigurationWidget_;
  metaModelPlotsConfigurationWidget_ = 0;
  metaModelPlotsLOOConfigurationWidget_ = 0;
}


void KrigingResultWindow::setParameters(const Analysis& analysis)
{
  const KrigingAnalysis kriging = *dynamic_cast<const KrigingAnalysis*>(&*analysis.getImplementation());

  // ParametersWidget
  QStringList namesList;
  namesList << tr("Algorithm");
  namesList << tr("Correlation model");
  namesList << tr("Scale");
  namesList << tr("Amplitude");
  if (kriging.getCovarianceModel().getImplementation()->getClassName() == "MaternModel")
    namesList << tr("nu");
  else if (kriging.getCovarianceModel().getImplementation()->getClassName() == "GeneralizedExponential")
    namesList << tr("p");
  namesList << tr("Trend basis");
  namesList << tr("Leave-one-out validation");

  QStringList valuesList;
  valuesList << tr("Kriging");
  valuesList << QString(kriging.getCovarianceModel().getImplementation()->getClassName().c_str());
  // scale
  QString scaleText;
  for (UnsignedInteger i=0; i<kriging.getCovarianceModel().getScale().getSize(); ++i)
  {
    scaleText += QString::number(kriging.getCovarianceModel().getScale()[i]);
    if (i < kriging.getCovarianceModel().getScale().getSize()-1)
      scaleText += "; ";
  }
  valuesList << scaleText;
  // amplitude
  QString amplitudeText;
  for (UnsignedInteger i=0; i<kriging.getCovarianceModel().getAmplitude().getSize(); ++i)
  {
    amplitudeText += QString::number(kriging.getCovarianceModel().getAmplitude()[i]);
    if (i < kriging.getCovarianceModel().getAmplitude().getSize()-1)
      amplitudeText += "; ";
  }
  valuesList << amplitudeText;
  // covariance model parameters
  if (kriging.getCovarianceModel().getImplementation()->getClassName() == "MaternModel")
  {
    double nu = dynamic_cast<MaternModel*>(&*kriging.getCovarianceModel().getImplementation())->getNu();
    valuesList << QString::number(nu);
  }
  else if (kriging.getCovarianceModel().getImplementation()->getClassName() == "GeneralizedExponential")
  {
    double p = dynamic_cast<GeneralizedExponential*>(&*kriging.getCovarianceModel().getImplementation())->getP();
    valuesList << QString::number(p);
  }
  // basis
  QString basisType(tr("Constant"));
  const UnsignedInteger dim = kriging.getBasis().getDimension();
  if (kriging.getBasis().getSize() == (dim+1))
    basisType = tr("Linear");
  else if (kriging.getBasis().getSize() == ((dim+1)*(dim+2)/2))
    basisType = tr("Quadratic");
  valuesList << basisType;

  valuesList << (kriging.isLeaveOneOutValidation()? tr("yes") : tr("no"));

  parametersWidget_ = new ParametersWidget(tr("Metamodel creation parameters:"), namesList, valuesList);
}


void KrigingResultWindow::buildInterface()
{
  tabWidget_ = new QTabWidget;

  const UnsignedInteger outputDimension = result_.getOutputSample().getDimension();
  QStringList outputNames;
  for (UnsignedInteger i=0; i<outputDimension; ++i)
    outputNames << QString::fromUtf8(result_.getOutputSample().getDescription()[i].c_str());

  // first tab : METAMODEL GRAPH --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * metaModelPlotLayout = new QVBoxLayout(tab);

  QVector<PlotWidget*> listMetaModels;
  ResizableStackedWidget * plotsStackedWidget = new ResizableStackedWidget;
  for (UnsignedInteger i=0; i<outputDimension; ++i)
  {
    FunctionalChaosResultWindow::MetaModelPlotWidget * plot = new FunctionalChaosResultWindow::MetaModelPlotWidget(result_.getMetaModelOutputSample().getMarginal(i), result_.getOutputSample().getMarginal(i));
    listMetaModels.append(plot);
    plotsStackedWidget->addWidget(plot);
  }
  metaModelPlotLayout->addWidget(plotsStackedWidget);
  metaModelPlotsConfigurationWidget_ = new GraphConfigurationWidget(listMetaModels, QStringList(), outputNames, GraphConfigurationWidget::MetaModel);
  connect(metaModelPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), plotsStackedWidget, SLOT(setCurrentIndex(int)));

  tabWidget_->addTab(tab, tr("Metamodel"));

  // second tab : RESULTS --------------------------------
  if (result_.getMetaModelOutputSampleLeaveOneOut().getSize())
  {
    QScrollArea * scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    tab = new QWidget;
    QVBoxLayout * resultsLayout = new QVBoxLayout(tab);
    resultsLayout->setSizeConstraint(QLayout::SetFixedSize);

    // -- output name --
    QHBoxLayout * headLayout = new QHBoxLayout;
    QLabel * outputName = new QLabel(tr("Output"));
    headLayout->addWidget(outputName);
    QComboBox * outputsComboBox = new QComboBox;
    outputsComboBox->addItems(outputNames);
    headLayout->addWidget(outputsComboBox);
    headLayout->addStretch();
    resultsLayout->addLayout(headLayout);

    // -- validation --
    QGroupBox * validationGroupBox = new QGroupBox(tr("Leave-one-out validation"));
    QVBoxLayout * validationGroupBoxLayout = new QVBoxLayout(validationGroupBox);
    ResizableStackedWidget * validationStackedWidget = new ResizableStackedWidget;

    for (UnsignedInteger outputIndex=0; outputIndex<result_.getMetaModelOutputSampleLeaveOneOut().getDimension(); ++outputIndex)
    {
      FunctionalChaosResultWindow::ValidationTableView * validationTableView = new FunctionalChaosResultWindow::ValidationTableView(result_.getErrorQ2LeaveOneOut()[outputIndex], result_.getQ2LeaveOneOut()[outputIndex]);
      validationStackedWidget->addWidget(validationTableView);
    }
    validationGroupBoxLayout->addWidget(validationStackedWidget);
    connect(outputsComboBox, SIGNAL(currentIndexChanged(int)), validationStackedWidget, SLOT(setCurrentIndex(int)));
    resultsLayout->addWidget(validationGroupBox);

    scrollArea->setWidget(tab);
    tabWidget_->addTab(scrollArea, tr("Summary"));
  }

  // third tab : GRAPH METAMODEL LOO --------------------------------
  if (result_.getMetaModelOutputSampleLeaveOneOut().getSize())
  {
    tab = new QWidget;
    metaModelPlotLayout = new QVBoxLayout(tab);

    QVector<PlotWidget*> listMetaModelsLOO;
    ResizableStackedWidget * plotsLOOStackedWidget = new ResizableStackedWidget;
    for (UnsignedInteger i=0; i<outputDimension; ++i)
    {
      FunctionalChaosResultWindow::MetaModelPlotWidget * plot = new FunctionalChaosResultWindow::MetaModelPlotWidget(result_.getMetaModelOutputSampleLeaveOneOut().getMarginal(i), result_.getOutputSample().getMarginal(i));
      listMetaModelsLOO.append(plot);
      plotsLOOStackedWidget->addWidget(plot);
    }
    metaModelPlotLayout->addWidget(plotsLOOStackedWidget);
    metaModelPlotsLOOConfigurationWidget_ = new GraphConfigurationWidget(listMetaModelsLOO, QStringList(), outputNames, GraphConfigurationWidget::MetaModel);
    connect(metaModelPlotsLOOConfigurationWidget_, SIGNAL(currentPlotChanged(int)), plotsLOOStackedWidget, SLOT(setCurrentIndex(int)));

    tabWidget_->addTab(tab, tr("Leave-one-out"));
  }

  // third/fourth tab : PARAMETERS --------------------------------
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  //
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));
  setWidget(tabWidget_);
}


void KrigingResultWindow::showHideGraphConfigurationWidget(int indexTab)
{
  switch (indexTab)
  {
    // if a plotWidget is visible
    case 0: // metamodel graph
      if (metaModelPlotsConfigurationWidget_)
        if (!metaModelPlotsConfigurationWidget_->isVisible())
          emit graphWindowActivated(metaModelPlotsConfigurationWidget_);
      break;
    case 2: // metamodel graph LOO
      if (metaModelPlotsLOOConfigurationWidget_)
        if (!metaModelPlotsLOOConfigurationWidget_->isVisible())
          emit graphWindowActivated(metaModelPlotsLOOConfigurationWidget_);
      break;
    // if no plotWidget is visible
    default:
    {
      emit graphWindowDeactivated();
      break;
    }
  }
}


void KrigingResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (oldState == Qt::WindowMaximized)
    return;

  if (newState == Qt::WindowFullScreen || newState == (Qt::WindowActive|Qt::WindowMaximized))
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == Qt::WindowNoState || newState == Qt::WindowMinimized || newState == (Qt::WindowActive|Qt::WindowMinimized))
    showHideGraphConfigurationWidget(-1);
}
}