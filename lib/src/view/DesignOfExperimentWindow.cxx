// DesignOfExperimentWindow.cxx

#include "otgui/DesignOfExperimentWindow.hxx"
#include "otgui/GraphConfigurationWidget.hxx"

#include <QVBoxLayout>
#include <QStackedLayout>

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

  tabWidget_->addTab(tab, tr("Table"));

  updateWindowForOutputs();

  setWidget(tabWidget_);
}


void DesignOfExperimentWindow::evaluateOutputs()
{
  try
  {
    designOfExperiment_.eval();
  }
  catch (Exception & ex)
  {
    setErrorMessage(ex.what());
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
  int nbInputs = designOfExperiment_.getVariableInputsNames().getSize();
  Indices ind(nbInputs);
  for (int i=0; i<nbInputs; ++i)
    for (UnsignedInteger j=0; j<designOfExperiment_.getInputSample().getDimension(); ++j)
      if (designOfExperiment_.getVariableInputsNames()[i] == designOfExperiment_.getInputSample().getDescription()[j])
      {
        ind[i] = j;
        break;
      }
  NumericalSample variableInputsSample = designOfExperiment_.getInputSample().getMarginal(ind);
  QStringList inputNames;
  for (int i=0; i<nbInputs; ++i)
    inputNames << variableInputsSample.getDescription()[i].c_str();

  int nbOutputs = designOfExperiment_.getResult().getOutputSample().getDimension();
  QStringList outputNames;
  for (int i=0; i<nbOutputs; ++i)
    outputNames << designOfExperiment_.getResult().getOutputSample().getDescription()[i].c_str();

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  QComboBox * outputsComboBoxFirstTab = new QComboBox;
  outputsComboBoxFirstTab->addItems(outputNames);
  connect(outputsComboBoxFirstTab, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLabelsText(int)));
  headLayout->addWidget(outputsComboBoxFirstTab);
  headLayout->addStretch();
  tabLayout->addLayout(headLayout);

  QGridLayout * grid = new QGridLayout;
  int gridRow = -1;

  QLabel * nbSimuLabel = new QLabel(tr("Size of the design of experiment : ") + QString::number(variableInputsSample.getSize()) + "\n");
  grid->addWidget(nbSimuLabel, ++gridRow, 0, 1, 2, Qt::AlignTop);

  QLabel * label = new QLabel(tr("Min"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  minLabel_ = new QLabel;
  grid->addWidget(minLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("Max"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  maxLabel_ = new QLabel;
  grid->addWidget(maxLabel_, gridRow, 1, Qt::AlignTop);

  grid->setColumnStretch(1, 1);
  tabLayout->addLayout(grid);
  tabLayout->addStretch();
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
      plot->setTitle(tr("Scatter plot: ") + outputs[i].getName().c_str() + tr(" vs ") + inputNames[j]);
      String inputDescription = designOfExperiment_.getPhysicalModel().getInputByName(inputNames[j].toStdString()).getDescription();
      if (!inputDescription.empty())
        plot->setAxisTitle(QwtPlot::xBottom, inputDescription.c_str());
      else
        plot->setAxisTitle(QwtPlot::xBottom, inputNames[j]);
      if (outputs[i].getDescription().size())
        plot->setAxisTitle(QwtPlot::yLeft, outputs[i].getDescription().c_str());
      else
        plot->setAxisTitle(QwtPlot::yLeft, outputs[i].getName().c_str());
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
        plot->setAxisTitle(QwtPlot::xBottom, inputDescription.c_str());
      else
        plot->setAxisTitle(QwtPlot::xBottom, inputNames[j]);
      inputDescription = designOfExperiment_.getPhysicalModel().getInputByName(inputNames[i].toStdString()).getDescription();
      if (!inputDescription.empty())
        plot->setAxisTitle(QwtPlot::yLeft, inputDescription.c_str());
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
  // min
  OSS oss1;
  oss1 << designOfExperiment_.getResult().getOutputSample().getMin()[indexOutput];
  for (UnsignedInteger j=0; j<designOfExperiment_.getResult().getListXMin()[indexOutput].getSize(); ++j)
  {
    NumericalPoint point(designOfExperiment_.getResult().getListXMin()[indexOutput][j]);
    oss1 << "\n  X=" << point.__str__();
  }
  minLabel_->setText(QString::fromStdString(oss1.str()));
  // max
  OSS oss2;
  oss2 << designOfExperiment_.getResult().getOutputSample().getMax()[indexOutput];
  for (UnsignedInteger j=0; j<designOfExperiment_.getResult().getListXMax()[indexOutput].getSize(); ++j)
  {
    NumericalPoint point(designOfExperiment_.getResult().getListXMax()[indexOutput][j]);
    oss2 << "\n  X=" << point.__str__();
  }
  maxLabel_->setText(QString::fromStdString(oss2.str()));
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