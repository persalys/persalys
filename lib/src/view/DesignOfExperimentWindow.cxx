//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the designs of experiments
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
#include "otgui/DesignOfExperimentWindow.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/DataTableWidget.hxx"

#include <QVBoxLayout>
#include <QStackedLayout>
#include <QGroupBox>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {

DesignOfExperimentWindow::DesignOfExperimentWindow(DesignOfExperimentItem * item)
  : OTguiSubWindow(item)
  , designOfExperiment_(item->getDesignOfExperiment())
  , graphConfigurationWidget_(0)
{
  buildInterface();
  connect(this, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)), this, SLOT(showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates)));
}


void DesignOfExperimentWindow::buildInterface()
{
  tabWidget_ = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  tableView_ = new OTguiTableView;
  if (!designOfExperiment_.getResult().getOutputSample().getSize())
  {
    OTguiTableModel * model = new OTguiTableModel(designOfExperiment_.getInputSample());
    tableView_->setModel(model);
  }

  tabLayout->addWidget(tableView_);

  QHBoxLayout * layout = new QHBoxLayout;
  evaluateButton_ = new QPushButton(tr("Evaluate"));
  if (designOfExperiment_.getResult().getOutputSample().getSize())
    evaluateButton_->setEnabled(false);
  layout->addStretch();
  layout->addWidget(evaluateButton_);
  tabLayout->addLayout(layout);
  connect(evaluateButton_, SIGNAL(clicked(bool)), this, SLOT(evaluateOutputs()));

  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  tabLayout->addWidget(errorMessageLabel_);

  tabWidget_->addTab(tab, tr("Table"));

  updateWindowForOutputs();

  setWidget(tabWidget_);
}


void DesignOfExperimentWindow::evaluateOutputs()
{
  try
  {
    designOfExperiment_.evaluate();
  }
  catch (std::exception & ex)
  {
    setTemporaryErrorMessage(ex.what());
  }
}


void DesignOfExperimentWindow::updateWindowForOutputs()
{
  if (designOfExperiment_.getResult().getOutputSample().getSize() > 0)
  {
    NumericalSample sample = designOfExperiment_.getInputSample();
    sample.stack(designOfExperiment_.getResult().getOutputSample());
    OTguiTableModel * model = new OTguiTableModel(sample);
    tableView_->setModel(model);
    if (model->sampleIsValid())
      addTabsForOutputs();
    evaluateButton_->setEnabled(false);
  }
}


void DesignOfExperimentWindow::addTabsForOutputs()
{
  int nbInputs = designOfExperiment_.getInputVariableNames().getSize();
  Indices ind(nbInputs);
  for (int i=0; i<nbInputs; ++i)
    for (UnsignedInteger j=0; j<designOfExperiment_.getInputSample().getDimension(); ++j)
      if (designOfExperiment_.getInputVariableNames()[i] == designOfExperiment_.getInputSample().getDescription()[j])
      {
        ind[i] = j;
        break;
      }
  NumericalSample variableInputsSample = designOfExperiment_.getInputSample().getMarginal(ind);
  QStringList inputNames;
  for (int i=0; i<nbInputs; ++i)
    inputNames << QString::fromLocal8Bit(variableInputsSample.getDescription()[i].c_str());

  int nbOutputs = designOfExperiment_.getResult().getOutputSample().getDimension();
  QStringList outputNames;
  for (int i=0; i<nbOutputs; ++i)
    outputNames << QString::fromLocal8Bit(designOfExperiment_.getResult().getOutputSample().getDescription()[i].c_str());

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  // -- output name --
  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  QComboBox * outputsComboBoxFirstTab = new QComboBox;
  outputsComboBoxFirstTab->addItems(outputNames);
  connect(outputsComboBoxFirstTab, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLabelsText(int)));
  headLayout->addWidget(outputsComboBoxFirstTab);
  headLayout->addStretch();
  tabLayout->addLayout(headLayout);

  // -- results --

  // number of simulations
  QLabel * nbSimuLabel = new QLabel(tr("Size of the design of experiment: ") + QString::number(variableInputsSample.getSize()) + "\n");
  nbSimuLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  tabLayout->addWidget(nbSimuLabel);

  // min/max table
  QGroupBox * minMaxGroupBox = new QGroupBox(tr("Minimum and Maximum"));
  UnsignedInteger totalNbInputs = designOfExperiment_.getInputSample().getDimension();
  QVBoxLayout * minMaxVbox = new QVBoxLayout(minMaxGroupBox);
  minMaxTable_ = new DataTableWidget(totalNbInputs + 1, 4);
  minMaxTable_->setHorizontalHeaderLabels(QStringList() << tr("") << tr("Variable") << tr("Minimum") << tr("Maximum"));
  minMaxTable_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  minMaxTable_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  minMaxTable_->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);

  QTableWidgetItem * item = new QTableWidgetItem(tr("Output"));
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  item->setBackgroundColor(minMaxTable_->verticalHeader()->palette().color(QPalette::Active, QPalette::Background));
  minMaxTable_->setItem(0, 0, item);
  QString rowTitle = tr("Inputs");
  if (totalNbInputs == 1)
    rowTitle = tr("Input");
  item = new QTableWidgetItem(rowTitle);
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  item->setBackgroundColor(minMaxTable_->verticalHeader()->palette().color(QPalette::Active, QPalette::Background));
  minMaxTable_->setSpan(1, 0, totalNbInputs, 1);
  minMaxTable_->setItem(1, 0, item);
  minMaxTable_->resizeColumnToContents(0);

  for (UnsignedInteger i=0; i<totalNbInputs; ++i)
  {
    item = new QTableWidgetItem(QString::fromLocal8Bit(designOfExperiment_.getInputSample().getDescription()[i].c_str()));
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    minMaxTable_->setItem(i+1, 1, item);
  }

  minMaxVbox->addWidget(minMaxTable_);
  minMaxVbox->addStretch();
  tabLayout->addWidget(minMaxGroupBox);
  updateLabelsText();

  tabWidget_->addTab(tab, tr("Min/Max"));

  // second tab --------------------------------
  tab = new QWidget;
  QStackedLayout * plotLayout = new QStackedLayout(tab);

  QVector<PlotWidget*> listScatterPlotWidgets;
  OutputCollection outputs = designOfExperiment_.getPhysicalModel().getOutputs();

  for (int j=0; j<nbInputs; ++j)
  {
    for (int i=0; i<nbOutputs; ++i)
    {
      PlotWidget * plot = new PlotWidget;
      plot->plotScatter(variableInputsSample.getMarginal(j), designOfExperiment_.getResult().getOutputSample().getMarginal(i));
      plot->setTitle(tr("Scatter plot: ") + QString::fromLocal8Bit(outputs[i].getName().c_str()) + tr(" vs ") + inputNames[j]);
      String inputDescription = designOfExperiment_.getPhysicalModel().getInputByName(inputNames[j].toStdString()).getDescription();
      if (!inputDescription.empty())
        plot->setAxisTitle(QwtPlot::xBottom, QString::fromLocal8Bit(inputDescription.c_str()));
      else
        plot->setAxisTitle(QwtPlot::xBottom, inputNames[j]);
      if (outputs[i].getDescription().size())
        plot->setAxisTitle(QwtPlot::yLeft, QString::fromLocal8Bit(outputs[i].getDescription().c_str()));
      else
        plot->setAxisTitle(QwtPlot::yLeft, QString::fromLocal8Bit(outputs[i].getName().c_str()));
      plotLayout->addWidget(plot);
      listScatterPlotWidgets.append(plot);
    }
    for (int i=0; i<nbInputs; ++i)
    {
      PlotWidget * plot = new PlotWidget;
      plot->plotScatter(variableInputsSample.getMarginal(j),variableInputsSample.getMarginal(i));
      plot->setTitle(tr("Scatter plot: ") + inputNames[i] + tr(" vs ") + inputNames[j]);
      String inputDescription = designOfExperiment_.getPhysicalModel().getInputByName(inputNames[j].toStdString()).getDescription();
      if (!inputDescription.empty())
        plot->setAxisTitle(QwtPlot::xBottom, QString::fromLocal8Bit(inputDescription.c_str()));
      else
        plot->setAxisTitle(QwtPlot::xBottom, inputNames[j]);
      inputDescription = designOfExperiment_.getPhysicalModel().getInputByName(inputNames[i].toStdString()).getDescription();
      if (!inputDescription.empty())
        plot->setAxisTitle(QwtPlot::yLeft, QString::fromLocal8Bit(inputDescription.c_str()));
      else
        plot->setAxisTitle(QwtPlot::yLeft, inputNames[i]);
      plotLayout->addWidget(plot);
      listScatterPlotWidgets.append(plot);
    }
  }
  for (int j=0; j<nbOutputs; ++j)
    {
      for (int i=0; i<nbOutputs; ++i)
      {
        PlotWidget * plot = new PlotWidget;
        plot->plotScatter(designOfExperiment_.getResult().getOutputSample().getMarginal(j), designOfExperiment_.getResult().getOutputSample().getMarginal(i));
        plot->setTitle(tr("Scatter plot: ") + QString::fromLocal8Bit(outputs[i].getName().c_str()) + tr(" vs ") + QString::fromLocal8Bit(outputs[j].getName().c_str()));
        if (outputs[j].getDescription().size())
          plot->setAxisTitle(QwtPlot::xBottom, QString::fromLocal8Bit(outputs[j].getDescription().c_str()));
        else
          plot->setAxisTitle(QwtPlot::xBottom, QString::fromLocal8Bit(outputs[j].getName().c_str()));
        if (outputs[i].getDescription().size())
          plot->setAxisTitle(QwtPlot::yLeft, QString::fromLocal8Bit(outputs[i].getDescription().c_str()));
        else
          plot->setAxisTitle(QwtPlot::yLeft, QString::fromLocal8Bit(outputs[i].getName().c_str()));
        plotLayout->addWidget(plot);
        listScatterPlotWidgets.append(plot);
      }
      for (int i=0; i<nbInputs; ++i)
      {
        PlotWidget * plot = new PlotWidget;
        plot->plotScatter(designOfExperiment_.getResult().getOutputSample().getMarginal(j), variableInputsSample.getMarginal(i));
        plot->setTitle(tr("Scatter plot: ") + inputNames[i] + tr(" vs ") + QString::fromLocal8Bit(outputs[j].getName().c_str()));
        if (outputs[j].getDescription().size())
          plot->setAxisTitle(QwtPlot::xBottom, QString::fromLocal8Bit(outputs[j].getDescription().c_str()));
        else
          plot->setAxisTitle(QwtPlot::xBottom, QString::fromLocal8Bit(outputs[j].getName().c_str()));
        String inputDescription = designOfExperiment_.getPhysicalModel().getInputByName(inputNames[i].toStdString()).getDescription();
        if (!inputDescription.empty())
          plot->setAxisTitle(QwtPlot::yLeft, QString::fromLocal8Bit(inputDescription.c_str()));
        else
          plot->setAxisTitle(QwtPlot::yLeft, inputNames[i]);

        plotLayout->addWidget(plot);
        listScatterPlotWidgets.append(plot);
      }
    }

  graphConfigurationWidget_ = new GraphConfigurationWidget(listScatterPlotWidgets, inputNames, outputNames, GraphConfigurationWidget::Scatter);
  connect(graphConfigurationWidget_, SIGNAL(currentPlotChanged(int)), plotLayout, SLOT(setCurrentIndex(int)));

  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));

  tabWidget_->addTab(tab, tr("Scatter plots"));

  // third tab --------------------------------
  tab = new PlotMatrixWidget(variableInputsSample, designOfExperiment_.getResult().getOutputSample());
  plotMatrixConfigurationWidget_ = new PlotMatrixConfigurationWidget(dynamic_cast<PlotMatrixWidget*>(tab));

  tabWidget_->addTab(tab, tr("Plot matrix Y-X"));

  // fourth tab --------------------------------
  tab = new PlotMatrixWidget(variableInputsSample, variableInputsSample);
  plotMatrix_X_X_ConfigurationWidget_ = new PlotMatrixConfigurationWidget(dynamic_cast<PlotMatrixWidget*>(tab));

  tabWidget_->addTab(tab, tr("Plot matrix X-X"));
}


void DesignOfExperimentWindow::updateLabelsText(int indexOutput)
{
  // minMaxTable_
  QTableWidgetItem * item = new QTableWidgetItem(QString::fromLocal8Bit(designOfExperiment_.getResult().getOutputSample().getDescription()[indexOutput].c_str()));
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  minMaxTable_->setItem(0, 1, item);

  // min
  const double min = designOfExperiment_.getResult().getOutputSample().getMin()[indexOutput];
  item = new QTableWidgetItem(QString::number(min));
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  minMaxTable_->setItem(0, 2, item);

  // max
  const double max = designOfExperiment_.getResult().getOutputSample().getMax()[indexOutput];
  item = new QTableWidgetItem(QString::number(max));
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  minMaxTable_->setItem(0, 3, item);

  if (designOfExperiment_.getResult().getListXMin()[indexOutput].getSize() > 1)
  {
    minMaxTable_->horizontalHeaderItem(2)->setIcon(QIcon(":/images/task-attention.png"));
    minMaxTable_->horizontalHeaderItem(2)->setToolTip(tr("Information: The output is minimum at another point."));
  }
  if (designOfExperiment_.getResult().getListXMax()[indexOutput].getSize() > 1)
  {
    minMaxTable_->horizontalHeaderItem(3)->setIcon(QIcon(":/images/task-attention.png"));
    minMaxTable_->horizontalHeaderItem(3)->setToolTip(tr("Information: The output is maximum at another point."));
  }

  for (UnsignedInteger i=0; i<designOfExperiment_.getResult().getInputSample().getDimension(); ++i)
  {
    // XMin
    item = new QTableWidgetItem(QString::number(designOfExperiment_.getResult().getListXMin()[indexOutput][0][i]));
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    minMaxTable_->setItem(i+1, 2, item);

    // XMax
    item = new QTableWidgetItem(QString::number(designOfExperiment_.getResult().getListXMax()[indexOutput][0][i]));
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    minMaxTable_->setItem(i+1, 3, item);
  }

  QSize size(minMaxTable_->sizeHint());
  int width = 0;
  for (int i=0; i<minMaxTable_->columnCount(); ++i)
    width += minMaxTable_->columnWidth(i);
  size.setWidth(width);
  int height = minMaxTable_->horizontalHeader()->height();
  for (int i=0; i<minMaxTable_->rowCount(); ++i)
    height += minMaxTable_->rowHeight(i);
  size.setHeight(height);
  minMaxTable_->setMinimumSize(size);
  minMaxTable_->setMaximumSize(size);
}


void DesignOfExperimentWindow::showHideGraphConfigurationWidget(int indexTab)
{
  // if plotWidget is visible
  if (indexTab == 2)
    emit graphWindowActivated(graphConfigurationWidget_);
  else if (indexTab == 3)
    emit graphWindowActivated(plotMatrixConfigurationWidget_);
  else if (indexTab == 4)
    emit graphWindowActivated(plotMatrix_X_X_ConfigurationWidget_);
  // if no plotWidget is visible
  else
  {
    emit graphWindowDeactivated(graphConfigurationWidget_);
    emit graphWindowDeactivated(plotMatrixConfigurationWidget_);
    emit graphWindowDeactivated(plotMatrix_X_X_ConfigurationWidget_);
  }
}


void DesignOfExperimentWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (newState == 4 || newState == 8 || newState == 10)
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == 0 || newState == 1 || newState == 2 || newState == 9)
    showHideGraphConfigurationWidget(-1);
}
}
