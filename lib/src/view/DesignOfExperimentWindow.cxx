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
#include "otgui/SampleTableModel.hxx"
#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/ResizableTableViewWithoutScrollBar.hxx"

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
    SampleTableModel * model = new SampleTableModel(designOfExperiment_.getInputSample());
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
    SampleTableModel * model = new SampleTableModel(sample);
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
  // sample of the variable inputs
  NumericalSample inS = designOfExperiment_.getInputSample().getMarginal(ind);
  // input names/descriptions
  QStringList inputNames;
  QStringList inAxisTitles;
  for (int i=0; i<nbInputs; ++i)
  {
    String inputName = inS.getDescription()[i];
    inputNames << QString::fromUtf8(inputName.c_str());

    QString inputDescription = QString::fromUtf8(designOfExperiment_.getPhysicalModel().getInputByName(inputName).getDescription().c_str());

    if (!inputDescription.isEmpty())
      inAxisTitles << inputDescription;
    else
      inAxisTitles << inputNames.last();
  }

  // sample of the outputs
  NumericalSample outS = designOfExperiment_.getResult().getOutputSample();
  // output names/descriptions
  QStringList outputNames;
  QStringList outAxisTitles;
  for (int i=0; i<outS.getDimension(); ++i)
  {
    outputNames << QString::fromUtf8(outS.getDescription()[i].c_str());
    QString outputDescription = QString::fromUtf8(designOfExperiment_.getPhysicalModel().getOutputs()[i].getDescription().c_str());
    if (!outputDescription.isEmpty())
      outAxisTitles << outputDescription;
    else
      outAxisTitles << outputNames.last();
  }

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  // -- output name --
  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  outputsComboBoxFirstTab_ = new QComboBox;
  outputsComboBoxFirstTab_->addItems(outputNames);
  headLayout->addWidget(outputsComboBoxFirstTab_);
  headLayout->addStretch();
  tabLayout->addLayout(headLayout);

  // -- results --

  // number of simulations
  QLabel * nbSimuLabel = new QLabel(tr("Size of the design of experiment: ") + QString::number(inS.getSize()) + "\n");
  nbSimuLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  tabLayout->addWidget(nbSimuLabel);

  // min/max table
  tabLayout->addWidget(getMinMaxTableWidget());

  tabWidget_->addTab(tab, tr("Min/Max"));

  // second tab --------------------------------
  tab = new QWidget;
  QStackedLayout * plotLayout = new QStackedLayout(tab);
  QVector<PlotWidget*> listScatterPlotWidgets = GetListScatterPlots(inS, outS, inputNames, inAxisTitles, outputNames, outAxisTitles);
  for (int i=0; i<listScatterPlotWidgets.size(); ++i)
    plotLayout->addWidget(listScatterPlotWidgets[i]);

  graphConfigurationWidget_ = new GraphConfigurationWidget(listScatterPlotWidgets, inputNames, outputNames, GraphConfigurationWidget::Scatter);
  connect(graphConfigurationWidget_, SIGNAL(currentPlotChanged(int)), plotLayout, SLOT(setCurrentIndex(int)));

  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));

  tabWidget_->addTab(tab, tr("Scatter plots"));

  // third tab --------------------------------
  tab = new PlotMatrixWidget(inS, outS);
  plotMatrixConfigurationWidget_ = new PlotMatrixConfigurationWidget(dynamic_cast<PlotMatrixWidget*>(tab));

  tabWidget_->addTab(tab, tr("Plot matrix Y-X"));

  // fourth tab --------------------------------
  tab = new PlotMatrixWidget(inS, inS);
  plotMatrix_X_X_ConfigurationWidget_ = new PlotMatrixConfigurationWidget(dynamic_cast<PlotMatrixWidget*>(tab));

  tabWidget_->addTab(tab, tr("Plot matrix X-X"));
}


QWidget* DesignOfExperimentWindow::getMinMaxTableWidget()
{
  UnsignedInteger totalNbInputs = designOfExperiment_.getInputSample().getDimension();

  QGroupBox * minMaxGroupBox = new QGroupBox(tr("Minimum and Maximum"));
  QVBoxLayout * minMaxGroupBoxLayout = new QVBoxLayout(minMaxGroupBox);
  QStackedLayout * minMaxGroupBoxStackedLayout = new QStackedLayout;

  for (int indexOutput=0; indexOutput<designOfExperiment_.getResult().getOutputSample().getDimension(); ++indexOutput)
  {
    ResizableTableViewWithoutScrollBar * minMaxTableView = new ResizableTableViewWithoutScrollBar;
    minMaxTableView->verticalHeader()->hide();
    CustomStandardItemModel * minMaxTable = new CustomStandardItemModel(totalNbInputs+1, 4);
    minMaxTableView->setModel(minMaxTable);

    // horizontal header
    minMaxTable->setHorizontalHeaderLabels(QStringList() << tr("") << tr("Variable") << tr("Minimum") << tr("Maximum"));

    // vertical header
    minMaxTable->setNotEditableHeaderItem(0, 0, tr("Output"));
    QString rowTitle = tr("Inputs at\nextremum");
    if (totalNbInputs == 1)
      rowTitle = tr("Input at\nextremum");
    minMaxTable->setNotEditableHeaderItem(1, 0, rowTitle);
    minMaxTableView->setSpan(1, 0, totalNbInputs, 1);

    // inputs names
    for (int i=0; i<totalNbInputs; ++i)
      minMaxTable->setNotEditableItem(i+1, 1, QString::fromUtf8(designOfExperiment_.getInputSample().getDescription()[i].c_str()));

    // output name
    minMaxTable->setNotEditableItem(0, 1, QString::fromUtf8(designOfExperiment_.getResult().getOutputSample().getDescription()[indexOutput].c_str()));
    // min
    minMaxTable->setNotEditableItem(0, 2, designOfExperiment_.getResult().getOutputSample().getMin()[indexOutput]);
    // max
    minMaxTable->setNotEditableItem(0, 3, designOfExperiment_.getResult().getOutputSample().getMax()[indexOutput]);

    // Xmin/XMax
    if (designOfExperiment_.getResult().getListXMin()[indexOutput].getSize() > 1)
    {
      minMaxTable->setHeaderData(2, Qt::Horizontal, QIcon(":/images/task-attention.png"), Qt::DecorationRole);
      minMaxTable->setHeaderData(2, Qt::Horizontal, tr("Information: The output is minimum at another point."), Qt::ToolTipRole);
    }
    if (designOfExperiment_.getResult().getListXMax()[indexOutput].getSize() > 1)
    {
      minMaxTable->setHeaderData(3, Qt::Horizontal, QIcon(":/images/task-attention.png"), Qt::DecorationRole);
      minMaxTable->setHeaderData(3, Qt::Horizontal, tr("Information: The output is maximum at another point."), Qt::ToolTipRole);
    }
    for (UnsignedInteger i=0; i<designOfExperiment_.getResult().getInputSample().getDimension(); ++i)
    {
      // XMin
      minMaxTable->setNotEditableItem(i+1, 2, designOfExperiment_.getResult().getListXMin()[indexOutput][0][i]);
      // XMax
      minMaxTable->setNotEditableItem(i+1, 3, designOfExperiment_.getResult().getListXMax()[indexOutput][0][i]);
    }

    // resize table
    minMaxTableView->resizeToContents();

    minMaxGroupBoxStackedLayout->addWidget(minMaxTableView);
  }
  minMaxGroupBoxLayout->addLayout(minMaxGroupBoxStackedLayout);
  connect(outputsComboBoxFirstTab_, SIGNAL(currentIndexChanged(int)), minMaxGroupBoxStackedLayout, SLOT(setCurrentIndex(int)));

  return minMaxGroupBox;
}


QVector<PlotWidget*> DesignOfExperimentWindow::GetListScatterPlots(const OT::NumericalSample & inS, const OT::NumericalSample & outS,
                                                                   const QStringList inNames, const QStringList inAxisNames,
                                                                   const QStringList outNames, const QStringList outAxisNames)
{
  QVector<PlotWidget*> listScatterPlotWidgets;
  const int nbInputs = inS.getDimension();
  const int nbOutputs = outS.getDimension();
  QPen pen = QPen(Qt::blue, 4);

  for (int j=0; j<nbInputs; ++j)
  {
    for (int i=0; i<nbOutputs; ++i)
    {
      PlotWidget * plot = new PlotWidget;
      plot->plotScatter(inS.getMarginal(j), outS.getMarginal(i), pen, inAxisNames[j], outAxisNames[i]);
      plot->setTitle(tr("Scatter plot: ") + outNames[i] + tr(" vs ") + inNames[j]);
      listScatterPlotWidgets.append(plot);
    }
    for (int i=0; i<nbInputs; ++i)
    {
      PlotWidget * plot = new PlotWidget;
      plot->plotScatter(inS.getMarginal(j), inS.getMarginal(i), pen, inAxisNames[j], inAxisNames[i]);
      plot->setTitle(tr("Scatter plot: ") + inNames[i] + tr(" vs ") + inNames[j]);
      listScatterPlotWidgets.append(plot);
    }
  }
  for (int j=0; j<nbOutputs; ++j)
  {
    for (int i=0; i<nbOutputs; ++i)
    {
      PlotWidget * plot = new PlotWidget;
      plot->plotScatter(outS.getMarginal(j), outS.getMarginal(i), pen, outAxisNames[j], outAxisNames[i]);
      plot->setTitle(tr("Scatter plot: ") + outNames[i] + tr(" vs ") + outNames[j]);
      listScatterPlotWidgets.append(plot);
    }
    for (int i=0; i<nbInputs; ++i)
    {
      PlotWidget * plot = new PlotWidget;
      plot->plotScatter(outS.getMarginal(j), inS.getMarginal(i), pen, outAxisNames[j], inAxisNames[i]);
      plot->setTitle(tr("Scatter plot: ") + inNames[i] + tr(" vs ") + outNames[j]);
      listScatterPlotWidgets.append(plot);
    }
  }
  return listScatterPlotWidgets;
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
