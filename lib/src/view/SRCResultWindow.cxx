//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of SRCAnalysis
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
#include "otgui/SRCResultWindow.hxx"

#include "otgui/SRCAnalysis.hxx"
#include "otgui/CopyableTableView.hxx"
#include "otgui/CustomStandardItemModel.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QScrollArea>
#include <QSplitter>
#include <QTextEdit>

using namespace OT;

namespace OTGUI {
  
SRCResultWindow::SRCResultWindow(AnalysisItem * item)
  : ResultWindow(item)
  , result_(dynamic_cast<SRCAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
{
  setParameters(item->getAnalysis());
  buildInterface();
}


void SRCResultWindow::setParameters(const Analysis & analysis)
{
  const SRCAnalysis * SRCanalysis = dynamic_cast<const SRCAnalysis*>(&*analysis.getImplementation());
  QStringList strList;
  strList << tr("Sensitivity analysis parameters :") + "\n";
  strList << tr("Algorithm : ") + tr("Standardized Regression Coefficients");
  strList << tr("Sample size : ") + QString::number(SRCanalysis->getSimulationsNumber());
  strList << tr("Seed : ") + QString::number(SRCanalysis->getSeed());

  parameters_ = strList.join("\n");
}


void SRCResultWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  scrollAreaWidget_ = new QStackedWidget;

  Description inputNames = result_.getInputNames();
  QStringList outputNames;
  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
    outputNames << QString::fromUtf8(result_.getOutputNames()[i].c_str());

  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
  {
    QSplitter * verticalSplitter = new QSplitter(Qt::Vertical);

    // plot
    PlotWidget * plot = new PlotWidget("sensitivitySRC", true);
    plot->plotSensitivityIndices(result_.getIndices()[i], NumericalPoint(), inputNames);
    plot->setAxisTitle(QwtPlot::xBottom, tr("Inputs"));

    verticalSplitter->addWidget(plot);
    verticalSplitter->setStretchFactor(0, 3);
    listPlotWidgets_.append(plot);

    // table of indices
    CopyableTableView * table = new CopyableTableView;
    table->verticalHeader()->hide();
#if QT_VERSION >= 0x050000
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    table->setSortingEnabled(true);
    tableModel_ = new CustomStandardItemModel(inputNames.getSize(), 2);
    tableModel_->setHorizontalHeaderLabels(QStringList() << tr("Input") << tr("Index"));
    table->setModel(tableModel_);

    // fill table
    for (UnsignedInteger j=0; j<inputNames.getSize(); ++j)
    {
      tableModel_->setNotEditableItem(j, 0, QString::fromUtf8(inputNames[j].c_str()));
      tableModel_->setNotEditableItem(j, 1, result_.getIndices()[i][j]);
    }
    connect(table->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(updateIndicesPlot(int, Qt::SortOrder)));

    verticalSplitter->addWidget(table);
    verticalSplitter->setStretchFactor(1, 1);
    scrollAreaWidget_->addWidget(verticalSplitter);
  }

  plotsConfigurationWidget_ = new GraphConfigurationWidget(listPlotWidgets_, QStringList(), outputNames, GraphConfigurationWidget::SensitivityIndices);
  connect(plotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), scrollAreaWidget_, SLOT(setCurrentIndex(int)));

  scrollArea->setWidget(scrollAreaWidget_);
  tabWidget->addTab(scrollArea, tr("Result"));

  // second tab --------------------------------
  tabWidget->addTab(buildParametersTextEdit(), tr("Parameters"));

  //
  setWidget(tabWidget);
}


void SRCResultWindow::updateIndicesPlot(int section, Qt::SortOrder order)
{
  int indexOutput = scrollAreaWidget_->currentIndex();
  NumericalPoint currentIndices(result_.getInputNames().getSize());
  Description sortedInputNames(result_.getInputNames().getSize());
  for (int i=0; i<result_.getInputNames().getSize(); ++i)
  {
    sortedInputNames[i] = tableModel_->data(tableModel_->index(i, 0)).toString().toStdString();
    currentIndices[i] = tableModel_->data(tableModel_->index(i, 1)).toDouble();
  }

  listPlotWidgets_[indexOutput]->clear();
  listPlotWidgets_[indexOutput]->plotSensitivityIndices(currentIndices, NumericalPoint(), sortedInputNames);
}


void SRCResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (oldState == 2)
    return;

  if (newState == 4 || newState == 10)
    if (!plotsConfigurationWidget_->isVisible())
      emit graphWindowActivated(plotsConfigurationWidget_);
  else if (newState == 0 || newState == 1 || newState == 9)
    emit graphWindowDeactivated();
}
}
