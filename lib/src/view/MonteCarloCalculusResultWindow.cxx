// MonteCarloCalculusResultWindow.cxx

#include "MonteCarloCalculusResultWindow.hxx"
#include "OTguiTableView.hxx"
#include "MonteCarloCalculus.hxx"

#include <QVBoxLayout>

#include <limits>

using namespace OT;

namespace OTGUI {

MonteCarloCalculusResultWindow::MonteCarloCalculusResultWindow(DistributionAnalysisItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<MonteCarloCalculus*>(&*item->getCalculus().getImplementation())->getResult())
  , isConfidenceIntervalRequired_(dynamic_cast<MonteCarloCalculus*>(&*item->getCalculus().getImplementation())->isConfidenceIntervalRequired())
  , levelConfidenceInterval_(dynamic_cast<MonteCarloCalculus*>(&*item->getCalculus().getImplementation())->getLevelConfidenceInterval())
{
  buildInterface();
}


void MonteCarloCalculusResultWindow::buildInterface()
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
  connect(outputsComboBoxFirstTab_, SIGNAL(currentIndexChanged(int)), this, SLOT(outputFirstTabChanged(int)));
  headLayout->addWidget(outputsComboBoxFirstTab_);

  headLayout->addStretch();

  QLabel * nbSimuLabel = new QLabel(tr("Number of simulations"));
  nbSimuLabel->setStyleSheet("font: bold 14px;");
  headLayout->addWidget(nbSimuLabel);
  nbSimuLabel = new QLabel(QString::fromStdString((OSS()<<result_.getResultSample().getSize()).str()));
  headLayout->addWidget(nbSimuLabel);
  tabLayout->addLayout(headLayout);

  QHBoxLayout * hbox = new QHBoxLayout;

  QVBoxLayout * vbox = new QVBoxLayout;
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

  label = new QLabel(tr("Mean"));
  label->setStyleSheet("font: bold 14px;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  meanLabel_ = new QLabel;
  grid->addWidget(meanLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("Standard deviation"));
  label->setStyleSheet("font: bold 14px;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  stdLabel_ = new QLabel;
  grid->addWidget(stdLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("Skewness"));
  label->setStyleSheet("font: bold 14px;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  skewnessLabel_ = new QLabel;
  grid->addWidget(skewnessLabel_, gridRow, 1);

  label = new QLabel(tr("Kurtosis"));
  label->setStyleSheet("font: bold 14px;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  kurtosisLabel_ = new QLabel;
  grid->addWidget(kurtosisLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("First quartile"));
  label->setStyleSheet("font: bold 14px;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  firstQuartileLabel_ = new QLabel;
  grid->addWidget(firstQuartileLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("Third quartile"));
  label->setStyleSheet("font: bold 14px;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  thirdQuartileLabel_ = new QLabel;
  grid->addWidget(thirdQuartileLabel_, gridRow, 1, Qt::AlignTop);

  vbox->addLayout(grid);
  updateLabelsText();

  // quantiles
  QHBoxLayout * quantLayout = new QHBoxLayout();
  label = new QLabel(tr("Probability"));
  label->setStyleSheet("font: bold 14px;");
  quantLayout->addWidget(label);
  probaSpinBox_ = new QDoubleSpinBox;
  label->setBuddy(probaSpinBox_);
  probaSpinBox_->setMinimum(0.0);
  probaSpinBox_->setMaximum(1.0);
  probaSpinBox_->setSingleStep(0.01);
  probaSpinBox_->setValue(0.5);
  quantLayout->addWidget(probaSpinBox_);
  label = new QLabel(tr("Quantile"));
  label->setStyleSheet("font: bold 14px;");
  quantLayout->addWidget(label);
  quantileSpinBox_ = new QDoubleSpinBox;
  label->setBuddy(quantileSpinBox_);
  quantileSpinBox_->setMinimum(std::numeric_limits<int>::min());
  quantileSpinBox_->setMaximum(std::numeric_limits<int>::max());
  quantLayout->addWidget(quantileSpinBox_);

  connect(probaSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(probaValueChanged(double)));
  connect(quantileSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(quantileValueChanged(double)));
  probaValueChanged(0.5);

  vbox->addLayout(quantLayout);

  vbox->addStretch();
  hbox->addLayout(vbox);
  tabLayout->addLayout(hbox);

  // PDF
  QVBoxLayout * graphsLayout = new QVBoxLayout;

  pdfPlot_ = new PlotWidget;
  pdfPlot_->plotHistogram(result_.getResultSample().getMarginal(0));
  pdfPlot_->plotPDFCurve(result_.getFittedDistribution()[0]);
  graphsLayout->addWidget(pdfPlot_->getPlotLabel());

  cdfPlot_ = new PlotWidget;
  cdfPlot_->plotHistogram(result_.getResultSample().getMarginal(0), true);
  cdfPlot_->plotCDFCurve(result_.getFittedDistribution()[0]);
  graphsLayout->addWidget(cdfPlot_->getPlotLabel());

  hbox->addLayout(graphsLayout);

  tabWidget->addTab(tab, tr("Result"));

  // second tab --------------------------------
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);
  QHBoxLayout * hLayout = new QHBoxLayout;
  outputName = new QLabel(tr("Output"));
  hLayout->addWidget(outputName);
  outputsComboBoxSecondTab_ = new QComboBox;
  items = QStringList();
  for (int i=0; i<result_.getResultSample().getDimension(); ++i)
    items<<QString::fromStdString(result_.getOutputNames()[i]);
  outputsComboBoxSecondTab_->addItems(items);
  connect(outputsComboBoxSecondTab_, SIGNAL(currentIndexChanged(int)), this, SLOT(outputBoxPlotChanged(int)));
  hLayout->addWidget(outputsComboBoxSecondTab_);
  hLayout->addStretch();
  tabLayout->addLayout(hLayout);

  boxPlot_ = new PlotWidget;

  tabLayout->addWidget(boxPlot_->getPlotLabel());
  tabLayout->addStretch();

  outputBoxPlotChanged(0);

  tabWidget->addTab(tab, tr("Box plots"));

  // third tab --------------------------------
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);

  hLayout = new QHBoxLayout;
  outputName = new QLabel(tr("Output"));
  hLayout->addWidget(outputName);
  outputsComboBoxThirdTab_ = new QComboBox;
  outputsComboBoxThirdTab_->addItems(items);
  connect(outputsComboBoxThirdTab_, SIGNAL(currentIndexChanged(int)), this, SLOT(outputScatterPlotChanged(int)));
  hLayout->addWidget(outputsComboBoxThirdTab_);
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

  // fourth tab --------------------------------
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);
  NumericalSample sample = result_.getInputSample();
  sample.stack(result_.getResultSample());
  OTguiTableView * tabResultView = new OTguiTableView(sample);
  tabLayout->addWidget(tabResultView);

  tabWidget->addTab(tab, tr("Result table"));

  //
  setWidget(tabWidget);
}


void MonteCarloCalculusResultWindow::updateLabelsText(int indexOutput)
{
  // mean
  OSS oss1;
  oss1 << result_.getMean()[indexOutput];
  if (isConfidenceIntervalRequired_)
  {
    double meanCILowerBound = result_.getMeanConfidenceInterval(levelConfidenceInterval_).getLowerBound()[indexOutput];
    double meanCIUpperBound = result_.getMeanConfidenceInterval(levelConfidenceInterval_).getUpperBound()[indexOutput];
    oss1 << "\n" <<" CI = [" << meanCILowerBound <<", "<<meanCIUpperBound<<"] at ";
    oss1 << levelConfidenceInterval_ << "%";
  }
  meanLabel_->setText(QString::fromStdString(oss1.str()));
  // standard deviation
  OSS oss2;
  oss2 << result_.getStandardDeviation()[indexOutput];
  if (isConfidenceIntervalRequired_)
  {
    double stdCILowerBound = result_.getStdConfidenceInterval(levelConfidenceInterval_).getLowerBound()[indexOutput];
    double stdCIUpperBound = result_.getStdConfidenceInterval(levelConfidenceInterval_).getUpperBound()[indexOutput];
    oss2 << "\n" <<" CI = [" << stdCILowerBound <<", "<<stdCIUpperBound<<"] at ";
    oss2 << levelConfidenceInterval_ << "%";
  }
  stdLabel_->setText(QString::fromStdString(oss2.str()));
  // skewness
  skewnessLabel_->setText(QString::fromStdString((OSS()<<result_.getSkewness()[indexOutput]).str()));
  // kurtosis
  kurtosisLabel_->setText(QString::fromStdString((OSS()<<result_.getKurtosis()[indexOutput]).str()));
  // first quartile
  firstQuartileLabel_->setText(QString::fromStdString((OSS()<<result_.getFirstQuartile()[indexOutput]).str()));
  // third quartile
  thirdQuartileLabel_->setText(QString::fromStdString((OSS()<<result_.getThirdQuartile()[indexOutput]).str()));
  // min
  OSS oss3;
  oss3 << result_.getListMin()[indexOutput];
  for (int j=0; j<result_.getListXMin()[indexOutput].getSize();++j)
  {
    NumericalPoint point(result_.getListXMin()[indexOutput][j]);
    oss3 << "\n  X=" <<point.__str__();
  }
  minLabel_->setText(QString::fromStdString(oss3.str()));
  // max
  OSS oss4;
  oss4 << result_.getListMax()[indexOutput];
  for (int j=0; j<result_.getListXMax()[indexOutput].getSize();++j)
  {
    NumericalPoint point(result_.getListXMax()[indexOutput][j]);
    oss4 << "\n  X=" <<point.__str__();
  }
  maxLabel_->setText(QString::fromStdString(oss4.str()));
}


void MonteCarloCalculusResultWindow::probaValueChanged(double proba)
{
  quantileSpinBox_->blockSignals(true);
  quantileSpinBox_->setValue(result_.getResultSample().getMarginal(outputsComboBoxFirstTab_->currentIndex()).computeQuantile(proba)[0]);
  quantileSpinBox_->blockSignals(false);
}


void MonteCarloCalculusResultWindow::quantileValueChanged(double quantile)
{
  probaSpinBox_->blockSignals(true);
  double cdf = 0.0;
  double p(1.0 / double(result_.getResultSample().getSize()));
  for (int j = 0; j < result_.getResultSample().getSize(); ++j)
  {
    if (result_.getResultSample()[j][outputsComboBoxFirstTab_->currentIndex()] < quantile)
      cdf += p;
  }
  probaSpinBox_->setValue(cdf);
  probaSpinBox_->blockSignals(false);
}


void MonteCarloCalculusResultWindow::outputFirstTabChanged(int indexOutput)
{
  updateLabelsText(indexOutput);
  probaValueChanged(0.5);
  pdfPlot_->clear();
  pdfPlot_->plotHistogram(result_.getResultSample().getMarginal(indexOutput));
  pdfPlot_->plotPDFCurve(result_.getFittedDistribution()[indexOutput]);
  cdfPlot_->clear();
  cdfPlot_->plotHistogram(result_.getResultSample().getMarginal(indexOutput), true);
  cdfPlot_->plotCDFCurve(result_.getFittedDistribution()[indexOutput]);
}


void MonteCarloCalculusResultWindow::outputBoxPlotChanged(int indexOutput)
{
  boxPlot_->clear();

  double median = result_.getMedian()[indexOutput];
  double Q1 = result_.getFirstQuartile()[indexOutput];
  double Q3 = result_.getThirdQuartile()[indexOutput];

  boxPlot_->plotBoxPlot(median, Q1, Q3, Q1 - 1.5*(Q3-Q1), Q3 + 1.5*(Q3-Q1), result_.getOutliers()[indexOutput]);
}


void MonteCarloCalculusResultWindow::inputScatterPlotChanged(int index)
{
  updateScatterPlot(index, outputsComboBoxThirdTab_->currentIndex());
}


void MonteCarloCalculusResultWindow::outputScatterPlotChanged(int index)
{
  updateScatterPlot(inputsComboBox_->currentIndex(), index);
}


void MonteCarloCalculusResultWindow::updateScatterPlot(int indexInput, int indexOutput)
{
  Q_ASSERT(scatterPlot_);
  scatterPlot_->clear();
  scatterPlot_->plotScatter(result_.getInputSample().getMarginal(indexInput), result_.getResultSample().getMarginal(indexOutput));
}


}