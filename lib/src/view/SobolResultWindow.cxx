//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of SobolAnalysis
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
#include "otgui/SobolResultWindow.hxx"
#include "otgui/SobolAnalysis.hxx"
#include "otgui/CopyableTableView.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QScrollArea>
#include <QSplitter>
#include <QStackedWidget>

using namespace OT;

namespace OTGUI {
  
SobolResultWindow::SobolResultWindow(AnalysisItem * item)
  : ResultWindow(item)
  , result_(dynamic_cast<SobolAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
{
  setParameters(item->getAnalysis());
  buildInterface();
}


void SobolResultWindow::setParameters(const Analysis & analysis)
{
  const SobolAnalysis * sobolAnalysis = dynamic_cast<const SobolAnalysis*>(&*analysis.getImplementation());

  // add warning if the model has not an independent copula
  if (!sobolAnalysis->getPhysicalModel().getComposedDistribution().hasIndependentCopula())
  {
    warningMessage_ = tr("The model has not an independent copula, the result could be false.");
  }

  QStringList strList;
  strList << tr("Sensitivity analysis parameters :") + "\n";
  strList << tr("Algorithm : ") + tr("Sobol");
  strList << tr("Maximum coefficient of variation : ") + QString::number(sobolAnalysis->getMaximumCoefficientOfVariation());
  if (sobolAnalysis->getMaximumElapsedTime() < std::numeric_limits<int>::max())
    strList << tr("Maximum elapsed time : ") + QString::number(sobolAnalysis->getMaximumElapsedTime()) + "(s)";
  else
    strList << tr("Maximum elapsed time : ") + "- (s)";
  if (sobolAnalysis->getMaximumCalls() < std::numeric_limits<int>::max())
    strList << tr("Maximum calls : ") + QString::number(sobolAnalysis->getMaximumCalls());
  else
    strList << tr("Maximum calls : ") + "-";

  strList << tr("Block size : ") + QString::number(sobolAnalysis->getBlockSize());
  strList << tr("Seed : ") + QString::number(sobolAnalysis->getSeed());

  parameters_ = strList.join("\n");
}


void SobolResultWindow::buildInterface()
{
  tabWidget_ = new QTabWidget;

  // first tab --------------------------------
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  Description inputNames = result_.getInputNames();
  QStringList outputNames;
  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
    outputNames << QString::fromUtf8(result_.getOutputNames()[i].c_str());

  QSplitter * verticalSplitter = new QSplitter(Qt::Vertical);

  QStackedWidget * plotStackedWidget = new QStackedWidget;
  QStackedWidget * tableStackedWidget = new QStackedWidget;
  QStackedWidget * interactionStackedWidget = new QStackedWidget;

  QWidget * widget = new QWidget;
  QVBoxLayout * vbox = new QVBoxLayout(widget);

  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
  {
    // plot
    PlotWidget * plot = new PlotWidget("sensitivitySobol", true);
    plot->plotSensitivityIndices(result_.getFirstOrderIndices()[i], result_.getTotalOrderIndices()[i], inputNames);
    plot->setAxisTitle(QwtPlot::xBottom, tr("Inputs"));
    listPlotWidgets_.append(plot);
    plotStackedWidget->addWidget(plot);

    // table of first order en total order indices + interactions
    CopyableTableView * tableView = new CopyableTableView;
    tableView->verticalHeader()->hide();
#if QT_VERSION >= 0x050000
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    tableView->setSortingEnabled(true);

    CustomStandardItemModel * tableModel = new CustomStandardItemModel(inputNames.getSize(), 3);
    tableModel->setHorizontalHeaderLabels(QStringList() << tr("Input")
                                                        << tr("First order index")
                                                        << tr("Total order index"));

    // fill table
    NumericalScalar interactionsValue(0.);
    for (UnsignedInteger j=0; j<inputNames.getSize(); ++j)
    {
      tableModel->setNotEditableItem(j, 0, QString::fromUtf8(inputNames[j].c_str()));
      tableModel->setNotEditableItem(j, 1, result_.getFirstOrderIndices()[i][j]);
      tableModel->setNotEditableItem(j, 2, result_.getTotalOrderIndices()[i][j]);

      if (result_.getTotalOrderIndices()[i][j] < result_.getFirstOrderIndices()[i][j])
      {
        tableModel->setData(tableModel->index(j, 2), tr("Warning: The total order index is inferior to the first order index."), Qt::ToolTipRole);
        tableModel->setData(tableModel->index(j, 2), QIcon(":/images/task-attention.png"), Qt::DecorationRole);
      }

      // compute interactions for the ith output
      interactionsValue += (result_.getTotalOrderIndices()[i][j]-result_.getFirstOrderIndices()[i][j]);
    }
    tableView->setModel(tableModel);
    connect(tableView->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(updateIndicesPlot(int, Qt::SortOrder)));
    listTableModels_.append(tableModel);
    tableStackedWidget->addWidget(tableView);

    // Interactions widgets
    QWidget * interactionWidget = new QWidget;
    QHBoxLayout * hbox = new QHBoxLayout(interactionWidget);
    QLabel * interactionsLabel = new QLabel(tr("Interactions"));
    interactionsLabel->setStyleSheet("font: bold;");
    hbox->addWidget(interactionsLabel);
    interactionsLabel = new QLabel(QString::number(interactionsValue, 'g', 4));
    hbox->addWidget(interactionsLabel);
    if (interactionsValue < 0 || interactionsValue > 1)
    {
      interactionsLabel = new QLabel;
      interactionsLabel->setPixmap(QPixmap(":/images/task-attention.png"));
      interactionsLabel->setToolTip(tr("Warning: The sum of the second order indices is not in the range [0, 1]."));
      hbox->addWidget(interactionsLabel);
    }
    hbox->addStretch();
    interactionStackedWidget->addWidget(interactionWidget);
  }

  plotsConfigurationWidget_ = new GraphConfigurationWidget(listPlotWidgets_, QStringList(), outputNames, GraphConfigurationWidget::SensitivityIndices);

  connect(plotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), plotStackedWidget, SLOT(setCurrentIndex(int)));
  connect(plotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), tableStackedWidget, SLOT(setCurrentIndex(int)));
  connect(plotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), interactionStackedWidget, SLOT(setCurrentIndex(int)));

  vbox->addWidget(tableStackedWidget);
  vbox->addWidget(interactionStackedWidget);

  // add a warning if the model has not an independent copula
  if (!warningMessage_.isEmpty())
  {
    QLabel * warningLabel = new QLabel(QString("%1%2%3").arg("<font color=red>").arg(warningMessage_).arg("</font>"));
    vbox->addWidget(warningLabel);
  }

  verticalSplitter->addWidget(plotStackedWidget);
  verticalSplitter->setStretchFactor(0, 3);

  verticalSplitter->addWidget(widget);
  verticalSplitter->setStretchFactor(1, 1);

  scrollArea->setWidget(verticalSplitter);
  tabWidget_->addTab(scrollArea, tr("Result"));

  // second tab --------------------------------
  tabWidget_->addTab(buildParametersTextEdit(), tr("Parameters"));

  //
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));
  setWidget(tabWidget_);
}


void SobolResultWindow::updateIndicesPlot(int section, Qt::SortOrder order)
{
  int indexOutput = plotsConfigurationWidget_->getCurrentPlotIndex();
  NumericalPoint currentFirstOrderIndices(result_.getInputNames().getSize());
  NumericalPoint currentTotalOrderIndices(result_.getInputNames().getSize());
  Description sortedInputNames(result_.getInputNames().getSize());

  CustomStandardItemModel * model = listTableModels_[indexOutput];
  if (!model)
  {
    std::cerr << "can not update indices plot" << std::endl;
    return;
  }
  for (UnsignedInteger i=0; i<result_.getInputNames().getSize(); ++i)
  {
    sortedInputNames[i] = model->data(model->index(i, 0)).toString().toStdString();
    currentFirstOrderIndices[i] = model->data(model->index(i, 1)).toDouble();
    currentTotalOrderIndices[i] = model->data(model->index(i, 2)).toDouble();
  }

  listPlotWidgets_[indexOutput]->clear();
  listPlotWidgets_[indexOutput]->plotSensitivityIndices(currentFirstOrderIndices, currentTotalOrderIndices, sortedInputNames);
}


void SobolResultWindow::showHideGraphConfigurationWidget(int indexTab)
{
  switch (indexTab)
  {
    // if a plotWidget is visible
    case 0:
      if (plotsConfigurationWidget_)
        if (!plotsConfigurationWidget_->isVisible())
          emit graphWindowActivated(plotsConfigurationWidget_);
      break;
    // if no plotWidget is visible
    default:
    {
      emit graphWindowDeactivated();
      break;
    }
  }
}


void SobolResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (oldState == 2)
    return;

  if (newState == 4 || newState == 10)
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == 0 || newState == 1 || newState == 9)
    showHideGraphConfigurationWidget(-1);
}
}