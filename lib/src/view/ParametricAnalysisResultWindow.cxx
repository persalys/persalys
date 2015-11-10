// ParametricAnalysisResultWindow.cxx

#include "otgui/ParametricAnalysisResultWindow.hxx"
#include "otgui/OTguiTableView.hxx"
#include "otgui/ParametricAnalysis.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI {

ParametricAnalysisResultWindow::ParametricAnalysisResultWindow(ParametricAnalysisItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<ParametricAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
{
  buildInterface();
}


void ParametricAnalysisResultWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  outputsComboBoxFirstTab_ = new QComboBox;
  QStringList items = QStringList();
  for (int i=0; i<result_.getResultSample().getDimension(); ++i)
    items<<QString::fromStdString(result_.getOutputNames()[i]);
  outputsComboBoxFirstTab_->addItems(items);
  connect(outputsComboBoxFirstTab_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLabelsText(int)));
  headLayout->addWidget(outputsComboBoxFirstTab_);
  headLayout->addStretch();

  QLabel * nbSimuLabel = new QLabel(tr("Number of simulations"));
  nbSimuLabel->setStyleSheet("font: bold 14px;");
  headLayout->addWidget(nbSimuLabel);
  nbSimuLabel = new QLabel(QString::fromStdString((OSS()<<result_.getResultSample().getSize()).str()));
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

  tabWidget->addTab(tab, tr("Result"));

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
  for (int i=0; i<result_.getInputSample().getDimension(); ++i)
    items<<QString::fromStdString(result_.getInputNames()[i]);
  inputsComboBox_->addItems(items);
  connect(inputsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(inputScatterPlotChanged(int)));
  hLayout->addWidget(inputsComboBox_, 0, Qt::AlignBottom);
  hLayout->addStretch();
  tabLayout->addLayout(hLayout);
  tabLayout->addStretch();

  updateScatterPlot();

  tabWidget->addTab(tab, tr("Scatter plots"));

  // third tab --------------------------------
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);
  NumericalSample sample = result_.getInputSample();
  sample.stack(result_.getResultSample());
  OTguiTableView * tabResultView = new OTguiTableView(sample);
  tabLayout->addWidget(tabResultView);

  tabWidget->addTab(tab, tr("Result table"));

  ////////////////
  setWidget(tabWidget);
}


void ParametricAnalysisResultWindow::updateLabelsText(int indexOutput)
{
  // min
  OSS oss1;
  oss1 << result_.getListMin()[indexOutput];
  for (int j=0; j<result_.getListXMin()[indexOutput].getSize();++j)
  {
    NumericalPoint point(result_.getListXMin()[indexOutput][j]);
    oss1 << "\n  X=" <<point.__str__();
  }
  minLabel_->setText(QString::fromStdString(oss1.str()));
  // max
  OSS oss2;
  oss2 << result_.getListMax()[indexOutput];
  for (int j=0; j<result_.getListXMax()[indexOutput].getSize();++j)
  {
    NumericalPoint point(result_.getListXMax()[indexOutput][j]);
    oss2 << "\n  X=" <<point.__str__();
  }
  maxLabel_->setText(QString::fromStdString(oss2.str()));
}


void ParametricAnalysisResultWindow::inputScatterPlotChanged(int index)
{
  updateScatterPlot(index, outputsComboBoxSecondTab_->currentIndex());
}


void ParametricAnalysisResultWindow::outputScatterPlotChanged(int index)
{
  updateScatterPlot(inputsComboBox_->currentIndex(), index);
}


void ParametricAnalysisResultWindow::updateScatterPlot(int indexInput, int indexOutput)
{
  Q_ASSERT(scatterPlot_);
  scatterPlot_->clear();
  scatterPlot_->plotScatter(result_.getInputSample().getMarginal(indexInput), result_.getResultSample().getMarginal(indexOutput));
}


}