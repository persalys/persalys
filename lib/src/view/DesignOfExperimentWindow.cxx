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
#include "otgui/DataTableModel.hxx"

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

  tableView_ = new DataTableView;
  if (!designOfExperiment_.getResult().getOutputSample().getSize())
  {
    DataTableModel * model = new DataTableModel(designOfExperiment_.getInputSample());
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
    DataTableModel * model = new DataTableModel(sample);
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
    inputNames << QString::fromUtf8(variableInputsSample.getDescription()[i].c_str());

  int nbOutputs = designOfExperiment_.getResult().getOutputSample().getDimension();
  QStringList outputNames;
  for (int i=0; i<nbOutputs; ++i)
    outputNames << QString::fromUtf8(designOfExperiment_.getResult().getOutputSample().getDescription()[i].c_str());

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

  minMaxTable_ = new NotEditableTableWidget(totalNbInputs + 1, 4);

  // horizontal header
  minMaxTable_->setHorizontalHeaderLabels(QStringList() << tr("") << tr("Variable") << tr("Minimum") << tr("Maximum"));

  // vertical header
  minMaxTable_->createHeaderItem(0, 0, tr("Output"));

  QString rowTitle = tr("Inputs at\nextremum");
  if (totalNbInputs == 1)
    rowTitle = tr("Input at\nextremum");
  minMaxTable_->createHeaderItem(1, 0, rowTitle);
  minMaxTable_->setSpan(1, 0, totalNbInputs, 1);

  for (UnsignedInteger i=0; i<totalNbInputs; ++i)
    minMaxTable_->createItem(i+1, 1, QString::fromUtf8(designOfExperiment_.getInputSample().getDescription()[i].c_str()));

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
      plot->setTitle(tr("Scatter plot: ") + QString::fromUtf8(outputs[i].getName().c_str()) + tr(" vs ") + inputNames[j]);
      String inputDescription = designOfExperiment_.getPhysicalModel().getInputByName(inputNames[j].toStdString()).getDescription();
      if (!inputDescription.empty())
        plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(inputDescription.c_str()));
      else
        plot->setAxisTitle(QwtPlot::xBottom, inputNames[j]);
      if (outputs[i].getDescription().size())
        plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(outputs[i].getDescription().c_str()));
      else
        plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(outputs[i].getName().c_str()));
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
        plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(inputDescription.c_str()));
      else
        plot->setAxisTitle(QwtPlot::xBottom, inputNames[j]);
      inputDescription = designOfExperiment_.getPhysicalModel().getInputByName(inputNames[i].toStdString()).getDescription();
      if (!inputDescription.empty())
        plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(inputDescription.c_str()));
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
        plot->setTitle(tr("Scatter plot: ") + QString::fromUtf8(outputs[i].getName().c_str()) + tr(" vs ") + QString::fromUtf8(outputs[j].getName().c_str()));
        if (outputs[j].getDescription().size())
          plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[j].getDescription().c_str()));
        else
          plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[j].getName().c_str()));
        if (outputs[i].getDescription().size())
          plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(outputs[i].getDescription().c_str()));
        else
          plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(outputs[i].getName().c_str()));
        plotLayout->addWidget(plot);
        listScatterPlotWidgets.append(plot);
      }
      for (int i=0; i<nbInputs; ++i)
      {
        PlotWidget * plot = new PlotWidget;
        plot->plotScatter(designOfExperiment_.getResult().getOutputSample().getMarginal(j), variableInputsSample.getMarginal(i));
        plot->setTitle(tr("Scatter plot: ") + inputNames[i] + tr(" vs ") + QString::fromUtf8(outputs[j].getName().c_str()));
        if (outputs[j].getDescription().size())
          plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[j].getDescription().c_str()));
        else
          plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[j].getName().c_str()));
        String inputDescription = designOfExperiment_.getPhysicalModel().getInputByName(inputNames[i].toStdString()).getDescription();
        if (!inputDescription.empty())
          plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(inputDescription.c_str()));
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
  minMaxTable_->createItem(0, 1, QString::fromUtf8(designOfExperiment_.getResult().getOutputSample().getDescription()[indexOutput].c_str()));

  // min
  const double min = designOfExperiment_.getResult().getOutputSample().getMin()[indexOutput];
  minMaxTable_->createItem(0, 2, min);

  // max
  const double max = designOfExperiment_.getResult().getOutputSample().getMax()[indexOutput];
  minMaxTable_->createItem(0, 3, max);

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
    minMaxTable_->createItem(i+1, 2, designOfExperiment_.getResult().getListXMin()[indexOutput][0][i]);
    // XMax
    minMaxTable_->createItem(i+1, 3, designOfExperiment_.getResult().getListXMax()[indexOutput][0][i]);
  }

  // resize table
  minMaxTable_->resizeToContents();
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
  else if (newState == 0 || newState == 1 || newState == 9)
    showHideGraphConfigurationWidget(-1);
}
}
