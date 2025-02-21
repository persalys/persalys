//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of an evaluation of the model
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
#include "persalys/MultiObjectiveOptimizationResultWindow.hxx"

#include "persalys/MultiObjectiveOptimizationAnalysis.hxx"
#include "persalys/CopyableTableView.hxx"
#include "persalys/ExportableTableView.hxx"
#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/ParametersWidget.hxx"
#include "persalys/ParametersTableView.hxx"
#include "persalys/WidgetBoundToDockWidget.hxx"
#include "persalys/GraphConfigurationWidget.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/TranslationManager.hxx"


#ifdef PERSALYS_HAVE_PARAVIEW
#include "persalys/PVServerManagerInterface.hxx"
#include "persalys/PVServerManagerSingleton.hxx"
#include "persalys/PVParCooViewWidget.hxx"
#include "persalys/PVMatrixPlotViewWidget.hxx"
#include "persalys/PVPlotSettingWidget.hxx"
#include "persalys/PVXYChartViewWidget.hxx"
#include "persalys/PVXYChartSettingWidget.hxx"

#include <pqLinksModel.h>
#include <pqApplicationCore.h>
#endif


#include <qwt_legend.h>
#include <qwt_scale_engine.h>

#include <QHBoxLayout>
#include <QGroupBox>
#include <QVector>
#include <QHeaderView>
#include <QScrollArea>
#include <QSplitter>

using namespace OT;

namespace PERSALYS
{

MultiObjectiveOptimizationResultWindow::MultiObjectiveOptimizationResultWindow(AnalysisItem * item, QWidget * parent)
  : DataAnalysisWindow(item, parent)
  , result_()
  , titleLabel_(0)
{
  initialize(item);
}


void MultiObjectiveOptimizationResultWindow::initialize(AnalysisItem * item)
{
  const MultiObjectiveOptimizationAnalysis * analysis = dynamic_cast<const MultiObjectiveOptimizationAnalysis*>(item->getAnalysis().getImplementation().get());
  if (!analysis)
    throw InvalidArgumentException(HERE) << "MultiObjectiveOptimizationResultWindow: the analysis is not a MultiObjectiveOptimizationAnalysis";

  result_ = analysis->getResult();

  // title
  const QString type = TranslationManager::GetTranslatedParameterName(analysis->getParameters()[0].second.c_str());
  const QString methodName = analysis->getParameters()[2].second.c_str();
  titleLabel_ = new TitleLabel(type + " - " + methodName, "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#optimresult");

  inputNames_ = QtOT::DescriptionToStringList(analysis->getVariableInputs());
  outputNames_ = QtOT::DescriptionToStringList(analysis->getInterestVariables());
  inAxisTitles_ = QtOT::GetVariableAxisLabels(analysis->getPhysicalModel(), analysis->getVariableInputs());
  outAxisTitles_ = QtOT::GetVariableAxisLabels(analysis->getPhysicalModel(), analysis->getInterestVariables());

  // parameters widget
  setParameters(item->getAnalysis().getImplementation(), tr("Optimization parameters"));

  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(titleLabel_);

  // tab widget
  tabWidget_ = new QTabWidget;

  // tab : fronts
  QTabWidget * resultsTab = new QTabWidget;

  QScrollArea* scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  QVector<PlotWidget*> listPlot;
  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;
  for(UnsignedInteger i = 0; i < analysis->getInterestVariables().getSize(); ++i)
  {
    for(UnsignedInteger j = 0; j < analysis->getInterestVariables().getSize(); ++j)
    {
      if (i != j)
      {
        PlotWidget * plotWidget = new PlotWidget;
        plotWidget->plotFronts(result_.getFronts(), i, j);
        listPlot.append(plotWidget);
        stackedWidget->addWidget(plotWidget);
      }
    }
  }

  FrontsGraphSetting * settingsWidget = new FrontsGraphSetting(listPlot, outputNames_, this);
  connect(settingsWidget, SIGNAL(currentPlotChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

  scrollArea->setWidget(new WidgetBoundToDockWidget(stackedWidget, settingsWidget, this));
  resultsTab->addTab(scrollArea, tr("Fronts"));
  tabWidget_->addTab(resultsTab, tr("Results"));

  bool canUseParaview = false;
#ifdef PERSALYS_HAVE_PARAVIEW
  if (SubWindow::SupportsOpenGL_3_2())
  {
    addParaviewWidgetsTabs();
    canUseParaview = true;
  }
#endif
  if (!canUseParaview)
  {
    // tab : Table
    resultsTab->addTab(ExportableTableView::GetSampleTableViewWidget(item, result_.getFinalPop()), tr("Table"));
  }

  // tab : PARAMETERS --------------------------------
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  // tab : model description --------------------------------
  if (modelDescriptionWidget_)
    tabWidget_->addTab(modelDescriptionWidget_, tr("Model"));

  widgetLayout->addWidget(tabWidget_, 1);
}

#ifdef PERSALYS_HAVE_PARAVIEW
void MultiObjectiveOptimizationResultWindow::addParaviewWidgetsTabs()
{
  // 1- table tab
  // with paraview the table is always shown in order to use the selection behavior
  PVSpreadSheetViewWidget * pvSpreadSheet = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
  tabWidget_->addTab(PVSpreadSheetViewWidget::GetSpreadSheetViewWidget(pvSpreadSheet, result_.getFinalPop(), getItem()), tr("Table"));

  // 2- cobweb tab --------------------------------
  PVParCooViewWidget * cobwebWidget = new PVParCooViewWidget(this, PVServerManagerSingleton::Get());
  const Sample sampleRank(result_.getFinalPop().rank() / result_.getFinalPop().getSize());
  cobwebWidget->setData(sampleRank);
  // the variables are automatically sorted : use setAxisToShow with the order of the sample
  cobwebWidget->setAxisToShow(result_.getFinalPop().getDescription());

  // setting widget
  MultiPlotSettingWidget * cobwebSettingWidget = new MultiPlotSettingWidget(cobwebWidget, result_.getFinalPop(), sampleRank, this);

  tabWidget_->addTab(new WidgetBoundToDockWidget(cobwebWidget, cobwebSettingWidget, this), tr("Parallel coordinates plot"));

  // 3- plot matrix tab --------------------------------
  PVMatrixPlotViewWidget * pvmatrixWidget = new PVMatrixPlotViewWidget(this, PVServerManagerSingleton::Get());
  pvmatrixWidget->setData(sampleRank);
  // the variables are automatically sorted : use setAxisToShow with the order of the sample
  pvmatrixWidget->setAxisToShow(result_.getFinalPop().getDescription());

  // setting widget
  MultiPlotSettingWidget * matrixSettingWidget = new MultiPlotSettingWidget(pvmatrixWidget, result_.getFinalPop(), sampleRank, this);

  tabWidget_->addTab(new WidgetBoundToDockWidget(pvmatrixWidget, matrixSettingWidget, this), tr("Plot matrix"));

  // 4- scatter plots tab --------------------------------
  // sample
  PVXYChartViewWidget * scatterPlotWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get());
  scatterPlotWidget->PVViewWidget::setData(result_.getFinalPop());
  scatterPlotWidget->setAxisTitles(inputNames_ + outputNames_, inAxisTitles_ + outAxisTitles_);

  ScatterSettingWidget * scatterSettingWidget = new ScatterSettingWidget(scatterPlotWidget,
      designOfExperiment_.getSample(),
      sampleRank,
      inputNames_,
      outputNames_,
      this);
  tabWidget_->addTab(new WidgetBoundToDockWidget(scatterPlotWidget, scatterSettingWidget, this), tr("Scatter plot"));

  // 5- links model --------------------------------
  pqLinksModel * linksModel = pqApplicationCore::instance()->getLinksModel();

  // There are selection behavior errors if windows use the same links names: a link name must be unique.
  // The pointers are uniques, so we use them to create an unique name...find a better and easier way.
  String aStr = (OSS() << pvSpreadSheet->getProxy() << pvmatrixWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), pvSpreadSheet->getProxy(), pvmatrixWidget->getProxy());
  aStr = (OSS() << cobwebWidget->getProxy() << pvSpreadSheet->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), cobwebWidget->getProxy(), pvSpreadSheet->getProxy());
  aStr = (OSS() << scatterPlotWidget->getProxy() << pvSpreadSheet->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), scatterPlotWidget->getProxy(), pvSpreadSheet->getProxy());
}
#endif

}
