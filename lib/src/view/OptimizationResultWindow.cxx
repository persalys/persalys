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
#include "persalys/OptimizationResultWindow.hxx"

#include "persalys/OptimizationAnalysis.hxx"
#include "persalys/CopyableTableView.hxx"
#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/ParametersWidget.hxx"
#include "persalys/ParametersTableView.hxx"
#include "persalys/WidgetBoundToDockWidget.hxx"
#include "persalys/GraphConfigurationWidget.hxx"
#include "persalys/TranslationManager.hxx"


#include <qwt_legend.h>
#include <qwt_scale_engine.h>

#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QScrollArea>
#include <QSplitter>

using namespace OT;

namespace PERSALYS
{

OptimizationResultWindow::OptimizationResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_()
  , titleLabel_(0)
{
  const OptimizationAnalysis * analysis = dynamic_cast<const OptimizationAnalysis*>(item->getAnalysis().getImplementation().get());
  if (!analysis)
    throw InvalidArgumentException(HERE) << "OptimizationResultWindow: the analysis is not a OptimizationAnalysis";

  result_ = analysis->getResult();

  // title
  const QString type = TranslationManager::GetTranslatedParameterName(item->getAnalysis().getImplementation()->getParameters()[3].second.c_str());
  const QString methodName = item->getAnalysis().getImplementation()->getParameters()[2].second.c_str();
  titleLabel_ = new TitleLabel(type + " - " + methodName, "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#optimresult");

  // parameters widget
  setParameters(item->getAnalysis(), tr("Optimization parameters"));

  buildInterface();
}


void OptimizationResultWindow::buildInterface()
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(titleLabel_);

  // get output info
  const QString outputName(QString::fromUtf8(result_.getProblem().getObjective().getOutputDescription()[0].c_str()));

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Output"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  VariablesListWidget * outputsListWidget = new VariablesListWidget;
  outputsListWidget->addItems(QStringList() << outputName);
  outputsListWidget->setCurrentRow(0);
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // tab widget
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  // optimal value
  QGroupBox * optimGroupBox = new QGroupBox(tr("Optimal point"));
  QVBoxLayout * optimGroupBoxLayout = new QVBoxLayout(optimGroupBox);

  // table view
  CopyableTableView * optimTableView = new CopyableTableView;
  optimTableView->verticalHeader()->hide();
  // table model
  const UnsignedInteger nbInputs = result_.getOptimalPoint().getSize();
  CustomStandardItemModel * optimTableModel = new CustomStandardItemModel(nbInputs + 1, 2, optimTableView);
  optimTableView->setModel(optimTableModel);

  // horizontal header
  optimTableModel->setHorizontalHeaderLabels(QStringList() << tr("")
      << tr("Variable")
      << tr("Value"));

  // output
  const UnsignedInteger nbOutputs = result_.getOptimalValue().getSize();
  if (nbOutputs > 1) {
    optimTableModel->setNotEditableHeaderItem(0, 0, tr("Outputs"));
    optimTableView->setSpan(0, 0, nbOutputs, 1);
  }
  else
    optimTableModel->setNotEditableHeaderItem(0, 0, tr("Output"));

  const OptimizationAnalysis * analysis(dynamic_cast<OptimizationAnalysis*>(dynamic_cast<AnalysisItem*>(getItem())->getAnalysis().getImplementation().get()));
  const Description outputNames = analysis->getPhysicalModel().getFunction().getOutputDescription();

  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    optimTableModel->setNotEditableItem(i, 1, QString(outputNames[i].c_str()));
    optimTableModel->setNotEditableItem(i, 2, result_.getOptimalValue()[i]);
  }

  // inputs
  const QString rowTitle = tr("Inputs");
  optimTableModel->setNotEditableHeaderItem(nbOutputs, 0, rowTitle);
  if (nbInputs > 1)
    optimTableView->setSpan(nbOutputs, 0, nbInputs, 1);

  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    optimTableModel->setNotEditableItem(i + nbOutputs, 1, QString::fromUtf8(analysis->getPhysicalModel().getInputNames()[i].c_str()));
    optimTableModel->setNotEditableItem(i + nbOutputs, 2, result_.getOptimalPoint()[i]);
  }

  // resize table
  optimTableView->resizeToContents();
  // 2nd row height correction because of the span
  if (nbInputs > 1)
  {
    const int h1 = optimTableView->verticalHeader()->sectionSize(2);
    const int h2 = optimTableView->verticalHeader()->sectionSize(1);
    optimTableView->verticalHeader()->resizeSection(1, optimTableView->verticalHeader()->sectionSize(2));
    optimTableView->setFixedSize(optimTableView->minimumSizeHint().width(), optimTableView->minimumSizeHint().height() - (h2 - h1));
  }

  optimGroupBoxLayout->addWidget(optimTableView);
  tabLayout->addWidget(optimGroupBox, 0, Qt::AlignTop);

  // resu
  QGroupBox * groupBox = new QGroupBox(tr("Optimization result"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);
  QStringList namesList;
  namesList << tr("Number of function evaluations")
            << tr("Absolute error")
            << tr("Relative error")
            << tr("Residual error")
            << tr("Constraint error");

  QStringList valuesList;
  valuesList << QString::number(result_.getProblem().getObjective().getCallsNumber())
             << QString::number(result_.getAbsoluteError())
             << QString::number(result_.getRelativeError())
             << QString::number(result_.getResidualError())
             << QString::number(result_.getConstraintError());

  ParametersTableView * table = new ParametersTableView(namesList, valuesList, true, true);

  groupBoxLayout->addWidget(table);
  tabLayout->addWidget(groupBox, 0, Qt::AlignTop);
  tabLayout->addStretch();

  scrollArea->setWidget(tab);

  tabWidget->addTab(scrollArea, tr("Result"));

  // tab : convergence
  QTabWidget * convTab = new QTabWidget;

  scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  // optimal point
  Graph graph = result_.drawOptimalValueHistory();
  PlotWidget * plot = new PlotWidget;
  plot->plotCurve(graph.getDrawables()[0].getData(), QPen(Qt::blue, 2));

  plot->setTitle(tr("Optimal value convergence graph"));
  plot->setAxisTitle(QwtPlot::yLeft, tr("Optimal value"));
  plot->setAxisTitle(QwtPlot::xBottom, tr("Number of evaluations"));

  SimpleGraphSetting * graphSettingWidget1 = new SimpleGraphSetting(plot, this);

  scrollArea->setWidget(new WidgetBoundToDockWidget(plot, graphSettingWidget1, this));
  convTab->addTab(scrollArea, tr("Optimal value"));

  // error
  scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  graph = result_.drawErrorHistory();
  plot = new PlotWidget;
  plot->plotCurve(graph.getDrawables()[0].getData(), QPen(Qt::red, 2), QwtPlotCurve::Lines, 0, tr("Absolute error"));
  plot->plotCurve(graph.getDrawables()[1].getData(), QPen(Qt::blue, 2), QwtPlotCurve::Lines, 0, tr("Relative error"));
  plot->plotCurve(graph.getDrawables()[2].getData(), QPen(Qt::green, 2), QwtPlotCurve::Lines, 0, tr("Residual error"));
  plot->plotCurve(graph.getDrawables()[3].getData(), QPen(Qt::magenta, 2), QwtPlotCurve::Lines, 0, tr("Constraint error"));

  plot->setTitle(tr("Error convergence graph"));
  plot->setAxisTitle(QwtPlot::yLeft, tr("Error"));
  plot->setAxisTitle(QwtPlot::xBottom, tr("Number of evaluations"));
  plot->insertLegend(new QwtLegend, QwtPlot::BottomLegend);

  SimpleGraphSetting * graphSettingWidget2 = new SimpleGraphSetting(plot, this);

  scrollArea->setWidget(new WidgetBoundToDockWidget(plot, graphSettingWidget2, this));
  convTab->addTab(scrollArea, tr("Error"));

  tabWidget->addTab(convTab, tr("Convergence"));


  // tab : PARAMETERS --------------------------------
  if (parametersWidget_)
    tabWidget->addTab(parametersWidget_, tr("Parameters"));

  // tab : model description --------------------------------
  if (modelDescriptionWidget_)
    tabWidget->addTab(modelDescriptionWidget_, tr("Model"));

  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);

  widgetLayout->addWidget(mainWidget, 1);
}
}
