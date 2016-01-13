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
  if (!designOfExperiment_.getOutputSample().getSize())
  {
    OTguiTableModel * model = new OTguiTableModel(designOfExperiment_.getInputSample());
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
  if (designOfExperiment_.getOutputSample().getSize() > 0)
  {
    NumericalSample sample = designOfExperiment_.getInputSample();
    sample.stack(designOfExperiment_.getOutputSample());
    OTguiTableModel * model = new OTguiTableModel(sample);
    tableView_->setModel(model);
    addTabsForOutputs();
    evaluateButton_->setEnabled(false);
  }
}


void DesignOfExperimentWindow::addTabsForOutputs()
{
  int nbInputs = designOfExperiment_.getInputSample().getDimension();
  int nbOutputs = designOfExperiment_.getOutputSample().getDimension();

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  QComboBox * outputsComboBoxFirstTab_ = new QComboBox;
  QStringList outputNames;
  for (int i=0; i<nbOutputs; ++i)
    outputNames << designOfExperiment_.getPhysicalModel().getOutputNames()[i].c_str();
  outputsComboBoxFirstTab_->addItems(outputNames);
  connect(outputsComboBoxFirstTab_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLabelsText(int)));
  headLayout->addWidget(outputsComboBoxFirstTab_);
  headLayout->addStretch();
  tabLayout->addLayout(headLayout);

  QGridLayout * grid = new QGridLayout;
  int gridRow = -1;

  QLabel * nbSimuLabel = new QLabel(tr("Size of the design of experiment : ") + QString::number(designOfExperiment_.getInputSample().getSize()) + "\n");
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

  QVector<PlotWidget*> listPlotWidgets;
  InputCollection inputs = designOfExperiment_.getPhysicalModel().getInputs();
  OutputCollection outputs = designOfExperiment_.getPhysicalModel().getOutputs();
  QStringList inputNames;
  for (int i=0; i<nbInputs; ++i)
    inputNames << designOfExperiment_.getPhysicalModel().getInputNames()[i].c_str();

  for (int i=0; i<nbOutputs; ++i)
  {
    for (int j=0; j<nbInputs; ++j)
    {
      PlotWidget * plot = new PlotWidget;
      plot->plotScatter(designOfExperiment_.getInputSample().getMarginal(j), designOfExperiment_.getOutputSample().getMarginal(i));
      plot->setTitle(tr("Scatter plot: ") + outputs[i].getName().c_str() + tr(" vs ") + inputs[j].getName().c_str());
      if (inputs[j].getDescription().size())
        plot->setAxisTitle(QwtPlot::xBottom, inputs[j].getDescription().c_str());
      else
        plot->setAxisTitle(QwtPlot::xBottom, inputs[j].getName().c_str());
      if (outputs[i].getDescription().size())
        plot->setAxisTitle(QwtPlot::yLeft, outputs[i].getDescription().c_str());
      else
        plot->setAxisTitle(QwtPlot::yLeft, outputs[i].getName().c_str());
      plotLayout->addWidget(plot);
      listPlotWidgets.append(plot);
    }
  }

  graphConfigurationWidget_ = new GraphConfigurationWidget(listPlotWidgets, inputNames, outputNames, GraphConfigurationWidget::Scatter);
  connect(graphConfigurationWidget_, SIGNAL(currentPlotChanged(int)), plotLayout, SLOT(setCurrentIndex(int)));

  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));

  tabWidget_->addTab(tab, tr("Scatter plots"));
}


void DesignOfExperimentWindow::updateLabelsText(int indexOutput)
{
  // min
  OSS oss1;
  oss1 << designOfExperiment_.getResult().getListMin()[indexOutput];
  for (int j=0; j<designOfExperiment_.getResult().getListXMin()[indexOutput].getSize();++j)
  {
    NumericalPoint point(designOfExperiment_.getResult().getListXMin()[indexOutput][j]);
    oss1 << "\n  X=" << point.__str__();
  }
  minLabel_->setText(QString::fromStdString(oss1.str()));
  // max
  OSS oss2;
  oss2 << designOfExperiment_.getResult().getListMax()[indexOutput];
  for (int j=0; j<designOfExperiment_.getResult().getListXMax()[indexOutput].getSize();++j)
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
  else
    emit graphWindowDeactivated(graphConfigurationWidget_);
}


void DesignOfExperimentWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (newState == 4 || newState == 8 || newState == 10)
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == 0 || newState == 1 || newState == 2 || newState == 9)
    emit graphWindowDeactivated(graphConfigurationWidget_);
}
}
