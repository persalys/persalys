// DesignOfExperimentWindow.cxx

#include "otgui/DesignOfExperimentWindow.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI {

DesignOfExperimentWindow::DesignOfExperimentWindow(DesignOfExperimentItem * item)
  : OTguiSubWindow(item)
  , designOfExperiment_(item->getDesignOfExperiment())
{
  buildInterface();
}


void DesignOfExperimentWindow::buildInterface()
{
  tabWidget_ = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  NumericalSample sample = designOfExperiment_.getInputSample();
  if (designOfExperiment_.getOutputSample().getSize())
    sample.stack(designOfExperiment_.getOutputSample());
  tableView_ = new OTguiTableView;
  OTguiTableModel * model = new OTguiTableModel(sample);
  tableView_->setModel(model);

  tabLayout->addWidget(tableView_);

  QHBoxLayout * layout = new QHBoxLayout;
  evaluateButton_ = new QPushButton(tr("Evaluate outputs"));
  if (designOfExperiment_.getOutputSample().getSize())
    evaluateButton_->setEnabled(false);
  layout->addStretch();
  layout->addWidget(evaluateButton_);
  tabLayout->addLayout(layout);
  connect(evaluateButton_, SIGNAL(clicked(bool)), this, SLOT(evaluateOutputs()));

  tabWidget_->addTab(tab, tr("Table"));

  setWidget(tabWidget_);
}


void DesignOfExperimentWindow::evaluateOutputs()
{
  designOfExperiment_.eval();

  if (!designOfExperiment_.getOutputSample().getSize())
    throw;

  NumericalSample sample = designOfExperiment_.getInputSample();
  sample.stack(designOfExperiment_.getOutputSample());
  OTguiTableModel * model = new OTguiTableModel(sample);
  tableView_->setModel(model);
  addTabsForOutputs();
  evaluateButton_->setEnabled(false);
}


void DesignOfExperimentWindow::addTabsForOutputs()
{
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  QComboBox * outputsComboBoxFirstTab_ = new QComboBox;
  QStringList items = QStringList();
  for (int i=0; i<designOfExperiment_.getOutputSample().getDimension(); ++i)
    items << designOfExperiment_.getPhysicalModel().getOutputNames()[i].c_str();
  outputsComboBoxFirstTab_->addItems(items);
  connect(outputsComboBoxFirstTab_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLabelsText(int)));
  headLayout->addWidget(outputsComboBoxFirstTab_);
  headLayout->addStretch();

  QLabel * nbSimuLabel = new QLabel(tr("Size of the design of experiment"));
  nbSimuLabel->setStyleSheet("font: bold 14px;");
  headLayout->addWidget(nbSimuLabel);
  nbSimuLabel = new QLabel(QString::number(designOfExperiment_.getInputSample().getSize()));
  headLayout->addWidget(nbSimuLabel);
  tabLayout->addLayout(headLayout);

  QGridLayout * grid = new QGridLayout;
  int gridRow = -1;

  QLabel * label = new QLabel(tr("Min"));
  label->setStyleSheet("font: bold 14px;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  minLabel_ = new QLabel;
  grid->addWidget(minLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("Max"));
  label->setStyleSheet("font: bold 14px;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  maxLabel_ = new QLabel;
  grid->addWidget(maxLabel_, gridRow, 1, Qt::AlignTop);

  tabLayout->addLayout(grid);
  tabLayout->addStretch();
  updateLabelsText();

  tabWidget_->addTab(tab, tr("Min/Max"));

  // second tab --------------------------------
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);

  QHBoxLayout * hLayout = new QHBoxLayout;
  outputName = new QLabel(tr("Output"));
  hLayout->addWidget(outputName);
  outputsComboBoxSecondTab_ = new QComboBox;
  outputsComboBoxSecondTab_->addItems(items);
  connect(outputsComboBoxSecondTab_, SIGNAL(currentIndexChanged(int)), this, SLOT(outputScatterPlotChanged(int)));
  hLayout->addWidget(outputsComboBoxSecondTab_);
  hLayout->addStretch();
  tabLayout->addLayout(hLayout);

  hLayout = new QHBoxLayout;
  scatterPlot_ = new PlotWidget;
  hLayout->addWidget(scatterPlot_->getPlotLabel());
  QLabel * inputName = new QLabel(tr("Input"));
  hLayout->addWidget(inputName, 0, Qt::AlignBottom);
  inputsComboBox_ = new QComboBox;
  items = QStringList();
  for (int i=0; i<designOfExperiment_.getInputSample().getDimension(); ++i)
    items << designOfExperiment_.getResult().getInputNames()[i].c_str();
  inputsComboBox_->addItems(items);
  connect(inputsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(inputScatterPlotChanged(int)));
  hLayout->addWidget(inputsComboBox_, 0, Qt::AlignBottom);
  hLayout->addStretch();
  tabLayout->addLayout(hLayout);
  tabLayout->addStretch();

  updateScatterPlot();

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


void DesignOfExperimentWindow::inputScatterPlotChanged(int index)
{
  updateScatterPlot(index, outputsComboBoxSecondTab_->currentIndex());
}


void DesignOfExperimentWindow::outputScatterPlotChanged(int index)
{
  updateScatterPlot(inputsComboBox_->currentIndex(), index);
}


void DesignOfExperimentWindow::updateScatterPlot(int indexInput, int indexOutput)
{
  Q_ASSERT(scatterPlot_);
  scatterPlot_->clear();
  scatterPlot_->plotScatter(designOfExperiment_.getInputSample().getMarginal(indexInput), designOfExperiment_.getOutputSample().getMarginal(indexOutput));
}
}