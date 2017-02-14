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

#include "otgui/MinMaxTableGroupBox.hxx"
#include "otgui/ParametersTableView.hxx"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QSplitter>

using namespace OT;

namespace OTGUI {

DesignOfExperimentWindow::DesignOfExperimentWindow(DesignOfExperimentItem * item)
  : OTguiSubWindow(item)
  , designOfExperiment_(item->getDesignOfExperiment())
  , variablesGroupBox_(0)
  , variablesListWidget_(0)
  , tabWidget_(0)
  , tablesTabWidget_(0)
  , scatterPlotsTabWidget_(0)
  , tableView_(0)
  , tableModel_(0)
  , failedPointsTableView_(0)
  , failedPointsTableModel_(0)
  , notEvaluatedTableView_(0)
  , notEvaluatedTableModel_(0)
  , scatterPlotsConfigurationWidget_(0)
  , plotMatrix_X_X_ConfigurationWidget_(0)
  , plotMatrixConfigurationWidget_(0)
{
  buildInterface();
  connect(this, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)), this, SLOT(showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates)));
}


DesignOfExperimentWindow::~DesignOfExperimentWindow()
{
  delete scatterPlotsConfigurationWidget_;
  delete plotMatrix_X_X_ConfigurationWidget_;
  delete plotMatrixConfigurationWidget_;
  scatterPlotsConfigurationWidget_ = 0;
  plotMatrix_X_X_ConfigurationWidget_ = 0;
  plotMatrixConfigurationWidget_ = 0;
}


void DesignOfExperimentWindow::buildInterface()
{
  setWindowTitle(tr("Design of experiment"));

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  variablesGroupBox_ = new QGroupBox(tr("Variables"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(variablesGroupBox_);

  variablesListWidget_ = new QListWidget;
  outputsLayoutGroupBox->addWidget(variablesListWidget_);

  mainWidget->addWidget(variablesGroupBox_);
  variablesGroupBox_->hide();
  mainWidget->setStretchFactor(0, 1);

  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  tabWidget_ = new QTabWidget;

  // first tab --------------------------------
  tablesTabWidget_ = new QTabWidget;
  tableView_ = new ExportableTableView;
  tablesTabWidget_->addTab(tableView_, tr("DOE"));

  tabWidget_->addTab(tablesTabWidget_, tr("Table"));
  tabLayout->addWidget(tabWidget_);

  // error message label
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  tabLayout->addWidget(errorMessageLabel_);

  mainWidget->addWidget(tab);
  mainWidget->setStretchFactor(1, 10);

  // if needed: fill other tabs
  updateTable();

  setWidget(mainWidget);
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


void DesignOfExperimentWindow::updateTable()
{
  if (tableModel_)
    delete tableModel_;
  if (failedPointsTableModel_)
    delete failedPointsTableModel_;
  if (notEvaluatedTableModel_)
    delete notEvaluatedTableModel_;

  tableModel_ = new SampleTableModel(designOfExperiment_.getSample(), tableView_);
  tableView_->setModel(tableModel_);

  if (designOfExperiment_.getOutputSample().getSize())
  {
    // tab with failed points
    if (designOfExperiment_.getFailedInputSample().getSize())
    {
      failedPointsTableModel_ = new SampleTableModel(designOfExperiment_.getFailedInputSample(), failedPointsTableView_);
      failedPointsTableView_ = new ExportableTableView;
      failedPointsTableView_->setModel(failedPointsTableModel_);
      tablesTabWidget_->addTab(failedPointsTableView_, tr("Failed points"));
    }
    // tab with not evaluated points
    if (designOfExperiment_.getNotEvaluatedInputSample().getSize())
    {
      notEvaluatedTableModel_ = new SampleTableModel(designOfExperiment_.getNotEvaluatedInputSample(), notEvaluatedTableView_);
      notEvaluatedTableView_ = new ExportableTableView;
      notEvaluatedTableView_->setModel(notEvaluatedTableModel_);
      tablesTabWidget_->addTab(notEvaluatedTableView_, tr("Not evaluated points"));
    }

    if (tableModel_->sampleIsValid())
      addTabsForOutputs();
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

    QString inputDescription;
    try
    {
      inputDescription = QString::fromUtf8(designOfExperiment_.getPhysicalModel().getInputByName(inputName).getDescription().c_str());
    }
    catch (std::exception) // maybe the variable does not exist anymore
    {
      //do nothing
    }
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
    const String outputName = outS.getDescription()[i];
    outputNames << QString::fromUtf8(outputName.c_str());
    QString outputDescription;
    try
    {
      outputDescription = QString::fromUtf8(designOfExperiment_.getPhysicalModel().getOutputByName(outputName).getDescription().c_str());
    }
    catch (std::exception) // maybe the variable does not exist anymore
    {
      //do nothing
    }
    if (!outputDescription.isEmpty())
      outAxisTitles << outputDescription;
    else
      outAxisTitles << outputNames.last();
  }

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  tabLayout->setSizeConstraint(QLayout::SetFixedSize);

  // -- outputs names --
  variablesListWidget_->addItems(outputNames);
  variablesListWidget_->setCurrentRow(0);

  // -- results --

  // number of simulations
  QGroupBox * aGroupBox = new QGroupBox;
  QVBoxLayout * aGroupBoxLayout = new QVBoxLayout(aGroupBox);

  ParametersTableView * table = new ParametersTableView(QStringList() << tr("Sample size"), QStringList() << QString::number(inS.getSize()), true, true);
  aGroupBoxLayout->addWidget(table);
  tabLayout->addWidget(aGroupBox);

  // min/max table
  MinMaxTableGroupBox * minMaxTableGroupBox = new MinMaxTableGroupBox(*dynamic_cast<DataSample*>(&*designOfExperiment_.getImplementation()));
  tabLayout->addWidget(minMaxTableGroupBox);
  connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), minMaxTableGroupBox, SLOT(setCurrentIndexStackedWidget(int)));

  scrollArea->setWidget(tab);
  tabWidget_->addTab(scrollArea, tr("Min/Max"));

  // second tab --------------------------------
  scatterPlotsTabWidget_ = new QTabWidget;

  // - scatter plots
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);
  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;
  QVector<PlotWidget*> listScatterPlotWidgets = GetListScatterPlots(inS, outS, designOfExperiment_.getFailedInputSample(),
                                                                    inputNames, inAxisTitles, outputNames, outAxisTitles);
  for (int i=0; i<listScatterPlotWidgets.size(); ++i)
    stackedWidget->addWidget(listScatterPlotWidgets[i]);

  tabLayout->addWidget(stackedWidget);
  scatterPlotsConfigurationWidget_ = new GraphConfigurationWidget(listScatterPlotWidgets, inputNames, outputNames, GraphConfigurationWidget::Scatter);
  connect(scatterPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));

  scatterPlotsTabWidget_->addTab(tab, tr("Scatter plots"));

  // - plot matrix X-X --------------------------------
  tab = new PlotMatrixWidget(inS, inS);
  plotMatrix_X_X_ConfigurationWidget_ = new PlotMatrixConfigurationWidget(dynamic_cast<PlotMatrixWidget*>(tab));

  scatterPlotsTabWidget_->addTab(tab, tr("Plot matrix X-X"));

  // - plot matrix Y-X --------------------------------
  tab = new PlotMatrixWidget(inS, outS);
  plotMatrixConfigurationWidget_ = new PlotMatrixConfigurationWidget(dynamic_cast<PlotMatrixWidget*>(tab));

  scatterPlotsTabWidget_->addTab(tab, tr("Plot matrix Y-X"));

  connect(scatterPlotsTabWidget_, SIGNAL(currentChanged(int)), this, SLOT(scatterPlotsTabWidgetIndexChanged()));
  tabWidget_->addTab(scatterPlotsTabWidget_, tr("Scatter plots"));
}


QVector<PlotWidget*> DesignOfExperimentWindow::GetListScatterPlots(const NumericalSample& inS,
                                                                   const NumericalSample& outS,
                                                                   const NumericalSample& notValidInS,
                                                                   const QStringList inNames,
                                                                   const QStringList inAxisNames,
                                                                   const QStringList outNames,
                                                                   const QStringList outAxisNames
                                                                  )
{
  QVector<PlotWidget*> listScatterPlotWidgets;
  const UnsignedInteger nbInputs = inS.getDimension();
  const UnsignedInteger nbOutputs = outS.getSize()? outS.getDimension() : 0;
  const QPen pen = QPen(Qt::blue, 4);
  const QPen notValidPen = QPen(Qt::red, 4);

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
      if (notValidInS.getSize())
        plot->plotScatter(notValidInS.getMarginal(j), notValidInS.getMarginal(i), notValidPen, inAxisNames[j], inAxisNames[i]);
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


void DesignOfExperimentWindow::scatterPlotsTabWidgetIndexChanged()
{
  showHideGraphConfigurationWidget(tabWidget_->currentIndex());
}


void DesignOfExperimentWindow::showHideGraphConfigurationWidget(int indexTab)
{
  // if a plotWidget is visible
  if (indexTab == 2) // scatter plots
  {
    if (scatterPlotsTabWidget_->currentIndex() == 0) // scatter plots
    {
      if (scatterPlotsConfigurationWidget_)
        if (!scatterPlotsConfigurationWidget_->isVisible())
          emit graphWindowActivated(scatterPlotsConfigurationWidget_);
    }
    else if (scatterPlotsTabWidget_->currentIndex() == 1) // plot matrix X-X
    {
      if (plotMatrix_X_X_ConfigurationWidget_)
        if (!plotMatrix_X_X_ConfigurationWidget_->isVisible())
          emit graphWindowActivated(plotMatrix_X_X_ConfigurationWidget_);
    }
    else if (scatterPlotsTabWidget_->currentIndex() == 2) // plot matrix Y-X
    {
      if (plotMatrixConfigurationWidget_)
        if (!plotMatrixConfigurationWidget_->isVisible())
          emit graphWindowActivated(plotMatrixConfigurationWidget_);
    }
    variablesGroupBox_->hide();
  }
  // if no plotWidget is visible
  else
  {
    emit graphWindowDeactivated();
    if (indexTab == 1)
      variablesGroupBox_->show();
    else
      variablesGroupBox_->hide();
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
