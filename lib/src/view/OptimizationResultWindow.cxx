//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of an evaluation of the model
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
#include "otgui/OptimizationResultWindow.hxx"
#include "otgui/OptimizationAnalysis.hxx"
#include "otgui/ResizableTableViewWithoutScrollBar.hxx"
#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/StudyTreeViewModel.hxx"
#include "otgui/ParametersWidget.hxx"
#include "otgui/ParametersTableView.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/WidgetBoundToDockWidget.hxx"
#include "otgui/GraphConfigurationWidget.hxx"

#include <qwt_legend.h>
#include <qwt_scale_engine.h>

#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QScrollArea>

using namespace OT;

namespace OTGUI
{

OptimizationResultWindow::OptimizationResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_()
{
  const OptimizationAnalysis * analysis = dynamic_cast<const OptimizationAnalysis*>(item->getAnalysis().getImplementation().get());
  if (!analysis)
    throw InvalidArgumentException(HERE) << "OptimizationResultWindow: the analysis is not a OptimizationAnalysis";

  result_ = analysis->getResult();

  // parameters widget
  setParameters(item->getAnalysis(), tr("Optimization parameters"));

  buildInterface();
}


void OptimizationResultWindow::buildInterface()
{
  setWindowTitle(tr("Optimization result"));

  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  tabLayout->setSizeConstraint(QLayout::SetFixedSize);

  // optimal value
  QGroupBox * optimGroupBox = new QGroupBox(tr("Optimal point"));
  QVBoxLayout * optimGroupBoxLayout = new QVBoxLayout(optimGroupBox);

  // table view
  ResizableTableViewWithoutScrollBar * optimTableView = new ResizableTableViewWithoutScrollBar;
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
  optimTableModel->setNotEditableHeaderItem(0, 0, tr("Output"));
  optimTableModel->setNotEditableItem(0, 1, QString::fromUtf8(result_.getProblem().getObjective().getOutputDescription()[0].c_str()));
  optimTableModel->setNotEditableItem(0, 2, result_.getOptimalValue()[0]);

  // inputs
  const QString rowTitle = tr("Inputs");
  optimTableModel->setNotEditableHeaderItem(1, 0, rowTitle);
  if (nbInputs > 1)
    optimTableView->setSpan(1, 0, nbInputs, 1);

  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    optimTableModel->setNotEditableItem(i + 1, 1, QString::fromUtf8(result_.getProblem().getObjective().getInputDescription()[i].c_str()));
    optimTableModel->setNotEditableItem(i + 1, 2, result_.getOptimalPoint()[i]);
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
  tabLayout->addWidget(optimGroupBox);

  // resu
  QGroupBox * groupBox = new QGroupBox(tr("Optimization result"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);
  QStringList namesList;
  namesList << tr("Iterations number")
            << tr("Calls number")
            << tr("Absolute error")
            << tr("Relative error")
            << tr("Residual error")
            << tr("Constraint error");

  QStringList valuesList;
  valuesList << QString::number(result_.getIterationNumber())
             << QString::number(result_.getProblem().getObjective().getCallsNumber())
             << QString::number(result_.getAbsoluteError())
             << QString::number(result_.getRelativeError())
             << QString::number(result_.getResidualError())
             << QString::number(result_.getConstraintError());

  ParametersTableView * table = new ParametersTableView(namesList, valuesList, true, true);

  groupBoxLayout->addWidget(table);
  tabLayout->addWidget(groupBox);

  scrollArea->setWidget(tab);

  tabWidget->addTab(scrollArea, tr("Result"));

  // tab : convergence
  QTabWidget * convTab = new QTabWidget;

  WidgetBoundToDockWidget * plotWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * plotWidgetLayout = new QVBoxLayout(plotWidget);

  // optimal point
  QVector<PlotWidget*> listPlotWidget1;
  Graph graph = result_.drawOptimalValueHistory();
  PlotWidget * plot = new PlotWidget;
  plot->plotCurve(graph.getDrawables()[0].getData(), QPen(Qt::blue, 2));
  listPlotWidget1.append(plot);
  plotWidgetLayout->addWidget(plot);

  plot->setTitle(tr("Optimal value convergence graph"));
  plot->setAxisTitle(QwtPlot::yLeft, tr("Optimal value"));
  plot->setAxisTitle(QwtPlot::xBottom, tr("Iteration"));

  GraphConfigurationWidget * graphSettingWidget1 = new GraphConfigurationWidget(listPlotWidget1,
      QStringList(),
      QStringList(),
      GraphConfigurationWidget::NoType,
      this);
  plotWidget->setDockWidget(graphSettingWidget1);

  convTab->addTab(plotWidget, tr("Optimal value"));

  // error
  QVector<PlotWidget*> listPlotWidget2;
  plotWidget = new WidgetBoundToDockWidget(this);
  plotWidgetLayout = new QVBoxLayout(plotWidget);
  graph = result_.drawErrorHistory();
  plot = new PlotWidget;
  plot->plotCurve(graph.getDrawables()[0].getData(), QPen(Qt::red, 2), QwtPlotCurve::Lines, 0, tr("Absolute error"));
  plot->plotCurve(graph.getDrawables()[1].getData(), QPen(Qt::blue, 2), QwtPlotCurve::Lines, 0, tr("Relative error"));
  plot->plotCurve(graph.getDrawables()[2].getData(), QPen(Qt::green, 2), QwtPlotCurve::Lines, 0, tr("Residual error"));
  plot->plotCurve(graph.getDrawables()[3].getData(), QPen(Qt::magenta, 2), QwtPlotCurve::Lines, 0, tr("Constraint error"));

  plot->setTitle(tr("Error convergence graph"));
  plot->setAxisTitle(QwtPlot::yLeft, tr("Error"));
  plot->setAxisTitle(QwtPlot::xBottom, tr("Iteration"));
  plot->insertLegend(new QwtLegend, QwtPlot::BottomLegend);

  listPlotWidget2.append(plot);
  plotWidgetLayout->addWidget(plot);

  GraphConfigurationWidget * graphSettingWidget2 = new GraphConfigurationWidget(listPlotWidget2,
      QStringList(),
      QStringList(),
      GraphConfigurationWidget::NoType,
      this);
  plotWidget->setDockWidget(graphSettingWidget2);

  convTab->addTab(plotWidget, tr("Error"));

  tabWidget->addTab(convTab, tr("Convergence"));


  // tab : PARAMETERS --------------------------------
  if (parametersWidget_)
    tabWidget->addTab(parametersWidget_, tr("Parameters"));

  setWidget(tabWidget);
}
}
