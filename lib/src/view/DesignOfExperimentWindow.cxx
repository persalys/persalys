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
#include "otgui/MinMaxTableGroupBox.hxx"

#include <QVBoxLayout>
#include <QGroupBox>

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

  tableView_ = new ExportableTableView;
  if (!designOfExperiment_.getOutputSample().getSize())
  {
    SampleTableModel * model = new SampleTableModel(designOfExperiment_.getInputSample(), tableView_);
    tableView_->setModel(model);
  }

  tabLayout->addWidget(tableView_);

  QHBoxLayout * layout = new QHBoxLayout;
  evaluateButton_ = new QPushButton(tr("Evaluate"));
  if (designOfExperiment_.getOutputSample().getSize())
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
    designOfExperiment_.run();
  }
  catch (std::exception & ex)
  {
    setTemporaryErrorMessage(ex.what());
  }
}


void DesignOfExperimentWindow::updateWindowForOutputs()
{
  if (designOfExperiment_.getOutputSample().getSize() > 0)
  {
    SampleTableModel * model = new SampleTableModel(designOfExperiment_.getSample());
    tableView_->setModel(model);
    if (model->sampleIsValid())
      addTabsForOutputs();
    evaluateButton_->setEnabled(false);
  }
}


void DesignOfExperimentWindow::addTabsForOutputs()
{
  const UnsignedInteger nbInputs = designOfExperiment_.getVariableInputNames().getSize();
  Indices ind(nbInputs);
  for (UnsignedInteger i=0; i<nbInputs; ++i)
    for (UnsignedInteger j=0; j<designOfExperiment_.getInputSample().getDimension(); ++j)
      if (designOfExperiment_.getVariableInputNames()[i] == designOfExperiment_.getInputSample().getDescription()[j])
      {
        ind[i] = j;
        break;
      }
  // sample of the variable inputs
  const NumericalSample inS = designOfExperiment_.getInputSample().getMarginal(ind);
  // input names/descriptions
  QStringList inputNames;
  QStringList inAxisTitles;
  for (UnsignedInteger i=0; i<nbInputs; ++i)
  {
    const String inputName = inS.getDescription()[i];
    inputNames << QString::fromUtf8(inputName.c_str());

    const QString inputDescription = QString::fromUtf8(designOfExperiment_.getPhysicalModel().getInputByName(inputName).getDescription().c_str());

    if (!inputDescription.isEmpty())
      inAxisTitles << inputDescription;
    else
      inAxisTitles << inputNames.last();
  }

  // sample of the outputs
  const NumericalSample outS = designOfExperiment_.getOutputSample();
  // output names/descriptions
  QStringList outputNames;
  QStringList outAxisTitles;
  for (UnsignedInteger i=0; i<outS.getDimension(); ++i)
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
  QLabel * nbSimuLabel = new QLabel(tr("Size of the design of experiment:") + " " + QString::number(inS.getSize()) + "\n");
  nbSimuLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  tabLayout->addWidget(nbSimuLabel);

  // min/max table
  MinMaxTableGroupBox * minMaxTableGroupBox = new MinMaxTableGroupBox(*dynamic_cast<DataSample*>(&*designOfExperiment_.getImplementation()));
  tabLayout->addWidget(minMaxTableGroupBox);
  connect(outputsComboBoxFirstTab_, SIGNAL(currentIndexChanged(int)), minMaxTableGroupBox, SLOT(setCurrentIndexStackedWidget(int)));

  tabWidget_->addTab(tab, tr("Min/Max"));

  // second tab --------------------------------
  tab = new QWidget;
  QVBoxLayout * plotLayout = new QVBoxLayout(tab);
  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;
  QVector<PlotWidget*> listScatterPlotWidgets = GetListScatterPlots(inS, outS, inputNames, inAxisTitles, outputNames, outAxisTitles);
  for (int i=0; i<listScatterPlotWidgets.size(); ++i)
    stackedWidget->addWidget(listScatterPlotWidgets[i]);

  plotLayout->addWidget(stackedWidget);
  graphConfigurationWidget_ = new GraphConfigurationWidget(listScatterPlotWidgets, inputNames, outputNames, GraphConfigurationWidget::Scatter);
  connect(graphConfigurationWidget_, SIGNAL(currentPlotChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));

  tabWidget_->addTab(tab, tr("Scatter plots"));

  // third tab --------------------------------
  tab = new PlotMatrixWidget(inS, inS);
  plotMatrix_X_X_ConfigurationWidget_ = new PlotMatrixConfigurationWidget(dynamic_cast<PlotMatrixWidget*>(tab));

  tabWidget_->addTab(tab, tr("Plot matrix X-X"));

  // fourth tab --------------------------------
  tab = new PlotMatrixWidget(inS, outS);
  plotMatrixConfigurationWidget_ = new PlotMatrixConfigurationWidget(dynamic_cast<PlotMatrixWidget*>(tab));

  tabWidget_->addTab(tab, tr("Plot matrix Y-X"));
}


QVector<PlotWidget*> DesignOfExperimentWindow::GetListScatterPlots(const OT::NumericalSample & inS, const OT::NumericalSample & outS,
                                                                   const QStringList inNames, const QStringList inAxisNames,
                                                                   const QStringList outNames, const QStringList outAxisNames)
{
  QVector<PlotWidget*> listScatterPlotWidgets;
  const UnsignedInteger nbInputs = inS.getDimension();
  const UnsignedInteger nbOutputs = outS.getSize()? outS.getDimension() : 0;
  const QPen pen = QPen(Qt::blue, 4);

  for (UnsignedInteger j=0; j<nbInputs; ++j)
  {
    for (UnsignedInteger i=0; i<nbOutputs; ++i)
    {
      PlotWidget * plot = new PlotWidget("scatterplot");
      plot->plotScatter(inS.getMarginal(j), outS.getMarginal(i), pen, inAxisNames[j], outAxisNames[i]);
      plot->setTitle(tr("Scatter plot:") + " " + outNames[i] + " " + tr("vs") + " " + inNames[j]);
      listScatterPlotWidgets.append(plot);
    }
    for (UnsignedInteger i=0; i<nbInputs; ++i)
    {
      PlotWidget * plot = new PlotWidget("scatterplot");
      plot->plotScatter(inS.getMarginal(j), inS.getMarginal(i), pen, inAxisNames[j], inAxisNames[i]);
      plot->setTitle(tr("Scatter plot:") + " " + inNames[i] + " " + tr("vs") + " " + inNames[j]);
      listScatterPlotWidgets.append(plot);
    }
  }
  for (UnsignedInteger j=0; j<nbOutputs; ++j)
  {
    for (UnsignedInteger i=0; i<nbOutputs; ++i)
    {
      PlotWidget * plot = new PlotWidget("scatterplot");
      plot->plotScatter(outS.getMarginal(j), outS.getMarginal(i), pen, outAxisNames[j], outAxisNames[i]);
      plot->setTitle(tr("Scatter plot:") + " " + outNames[i] + " " + tr("vs") + " " + outNames[j]);
      listScatterPlotWidgets.append(plot);
    }
    for (UnsignedInteger i=0; i<nbInputs; ++i)
    {
      PlotWidget * plot = new PlotWidget("scatterplot");
      plot->plotScatter(outS.getMarginal(j), inS.getMarginal(i), pen, outAxisNames[j], inAxisNames[i]);
      plot->setTitle(tr("Scatter plot:") + " " + inNames[i] + " " + tr("vs") + " " + outNames[j]);
      listScatterPlotWidgets.append(plot);
    }
  }
  return listScatterPlotWidgets;
}


void DesignOfExperimentWindow::showHideGraphConfigurationWidget(int indexTab)
{
  switch (indexTab)
  {
    // if a plotWidget is visible
    case 2: // scatter plots
      if (!graphConfigurationWidget_->isVisible())
        emit graphWindowActivated(graphConfigurationWidget_);
      break;
    case 3: // plot matrix X-X
      if (!plotMatrix_X_X_ConfigurationWidget_->isVisible())
        emit graphWindowActivated(plotMatrix_X_X_ConfigurationWidget_);
      break;
    case 4: // plot matrix Y-X
      if (!plotMatrixConfigurationWidget_->isVisible())
        emit graphWindowActivated(plotMatrixConfigurationWidget_);
      break;
    // if no plotWidget is visible
    default:
    {
      emit graphWindowDeactivated();
      break;
    }
  }
}


void DesignOfExperimentWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (oldState == Qt::WindowMaximized)
    return;

  if (newState == Qt::WindowFullScreen || newState == (Qt::WindowActive|Qt::WindowMaximized))
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == Qt::WindowNoState || newState == Qt::WindowMinimized || newState == (Qt::WindowActive|Qt::WindowMinimized))
    showHideGraphConfigurationWidget(-1);
}
}
