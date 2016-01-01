// MonteCarloResultWindow.cxx

#include "otgui/MonteCarloResultWindow.hxx"
#include "otgui/OTguiTableView.hxx"
#include "otgui/MonteCarloAnalysis.hxx"

#include <QVBoxLayout>
#include <QStackedLayout>

#include <limits>

using namespace OT;

namespace OTGUI {

MonteCarloResultWindow::MonteCarloResultWindow(CentralTendencyItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<MonteCarloAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
  , physicalModel_(item->getAnalysis().getPhysicalModel())
  , isConfidenceIntervalRequired_(dynamic_cast<MonteCarloAnalysis*>(&*item->getAnalysis().getImplementation())->isConfidenceIntervalRequired())
  , levelConfidenceInterval_(dynamic_cast<MonteCarloAnalysis*>(&*item->getAnalysis().getImplementation())->getLevelConfidenceInterval())
{
  buildInterface();
  connect(this, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)), this, SLOT(showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates)));
}


void MonteCarloResultWindow::buildInterface()
{
  int nbInputs = result_.getInputSample().getDimension();
  int nbOutputs = result_.getResultSample().getDimension();

  tabWidget_ = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  outputsComboBoxFirstTab_ = new QComboBox;
  QStringList outputNames;
  for (int i=0; i<nbOutputs; ++i)
    outputNames << result_.getOutputNames()[i].c_str();
  outputsComboBoxFirstTab_->addItems(outputNames);
  connect(outputsComboBoxFirstTab_, SIGNAL(currentIndexChanged(int)), this, SLOT(outputFirstTabChanged(int)));
  headLayout->addWidget(outputsComboBoxFirstTab_);

  headLayout->addStretch();

  QLabel * nbSimuLabel = new QLabel(tr("Number of simulations"));
  nbSimuLabel->setStyleSheet("font: bold 14px;");
  headLayout->addWidget(nbSimuLabel);
  nbSimuLabel = new QLabel(QString::number(result_.getResultSample().getSize()));
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
  QHBoxLayout * quantLayout = new QHBoxLayout;
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
  graphsLayout->addWidget(pdfPlot_);

  cdfPlot_ = new PlotWidget;
  cdfPlot_->plotHistogram(result_.getResultSample().getMarginal(0), true);
  cdfPlot_->plotCDFCurve(result_.getFittedDistribution()[0]);
  graphsLayout->addWidget(cdfPlot_);

  hbox->addLayout(graphsLayout);

  tabWidget_->addTab(tab, tr("Result"));

  // second tab --------------------------------
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);
  QHBoxLayout * hLayout = new QHBoxLayout;
  outputName = new QLabel(tr("Output"));
  hLayout->addWidget(outputName);
  outputsComboBoxSecondTab_ = new QComboBox;
  outputsComboBoxSecondTab_->addItems(outputNames);
  connect(outputsComboBoxSecondTab_, SIGNAL(currentIndexChanged(int)), this, SLOT(outputBoxPlotChanged(int)));
  hLayout->addWidget(outputsComboBoxSecondTab_);
  hLayout->addStretch();
  tabLayout->addLayout(hLayout);

  boxPlot_ = new PlotWidget;

  tabLayout->addWidget(boxPlot_);
  tabLayout->addStretch();

  outputBoxPlotChanged(0);

  tabWidget_->addTab(tab, tr("Box plots"));

  // third tab --------------------------------
  tab = new QWidget;
  QStackedLayout * plotLayout = new QStackedLayout(tab);

  QVector<PlotWidget*> listPlotWidgets;
  InputCollection inputs = physicalModel_.getInputs();
  OutputCollection outputs = physicalModel_.getOutputs();
  QStringList inputNames;
  for (int i=0; i<nbInputs; ++i)
    inputNames << physicalModel_.getInputNames()[i].c_str();

  for (int i=0; i<nbOutputs; ++i)
  {
    for (int j=0; j<nbInputs; ++j)
    {
      PlotWidget * plot = new PlotWidget;
      plot->plotScatter(result_.getInputSample().getMarginal(j), result_.getResultSample().getMarginal(i));
      plot->setAxisTitle(QwtPlot::xBottom, inputs[j].getDescription().c_str());
      plot->setAxisTitle(QwtPlot::yLeft, outputs[i].getDescription().c_str());
      plotLayout->addWidget(plot);
      listPlotWidgets.append(plot);
    }
  }

  graphConfigurationWidget_ = new GraphConfigurationWidget(listPlotWidgets, inputNames, outputNames);
  connect(graphConfigurationWidget_, SIGNAL(currentPlotChanged(int)), plotLayout, SLOT(setCurrentIndex(int)));

  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));

  tabWidget_->addTab(tab, tr("Scatter plots"));

  // fourth tab --------------------------------
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);
  NumericalSample sample = result_.getInputSample();
  sample.stack(result_.getResultSample());
  OTguiTableView * tabResultView = new OTguiTableView(sample);
  tabLayout->addWidget(tabResultView);

  tabWidget_->addTab(tab, tr("Result table"));

  //
  setWidget(tabWidget_);
}


void MonteCarloResultWindow::updateLabelsText(int indexOutput)
{
  // mean
  QString meanText = QString::number(result_.getMean()[indexOutput]);
  if (isConfidenceIntervalRequired_)
  {
    double meanCILowerBound = result_.getMeanConfidenceInterval(levelConfidenceInterval_).getLowerBound()[indexOutput];
    double meanCIUpperBound = result_.getMeanConfidenceInterval(levelConfidenceInterval_).getUpperBound()[indexOutput];
    meanText += "\n CI = [" + QString::number(meanCILowerBound) + ", ";
    meanText += QString::number(meanCIUpperBound) + "] at ";
    meanText += QString::number(levelConfidenceInterval_) + "%";
  }
  meanLabel_->setText(meanText);
  // standard deviation
  QString stdText = QString::number(result_.getStandardDeviation()[indexOutput]);
  if (isConfidenceIntervalRequired_)
  {
    double stdCILowerBound = result_.getStdConfidenceInterval(levelConfidenceInterval_).getLowerBound()[indexOutput];
    double stdCIUpperBound = result_.getStdConfidenceInterval(levelConfidenceInterval_).getUpperBound()[indexOutput];
    stdText += "\n CI = [" + QString::number(stdCILowerBound) + ", ";
    stdText += QString::number(stdCIUpperBound) + "] at ";
    stdText += QString::number(levelConfidenceInterval_) + "%";
  }
  stdLabel_->setText(stdText);
  // skewness
  skewnessLabel_->setText(QString::number(result_.getSkewness()[indexOutput]));
  // kurtosis
  kurtosisLabel_->setText(QString::number(result_.getKurtosis()[indexOutput]));
  // first quartile
  firstQuartileLabel_->setText(QString::number(result_.getFirstQuartile()[indexOutput]));
  // third quartile
  thirdQuartileLabel_->setText(QString::number(result_.getThirdQuartile()[indexOutput]));
  // min
  OSS oss3;
  oss3 << result_.getListMin()[indexOutput];
  for (int j=0; j<result_.getListXMin()[indexOutput].getSize();++j)
  {
    NumericalPoint point(result_.getListXMin()[indexOutput][j]);
    oss3 << "\n  X=" << point.__str__();
  }
  minLabel_->setText(QString::fromStdString(oss3.str()));
  // max
  OSS oss4;
  oss4 << result_.getListMax()[indexOutput];
  for (int j=0; j<result_.getListXMax()[indexOutput].getSize();++j)
  {
    NumericalPoint point(result_.getListXMax()[indexOutput][j]);
    oss4 << "\n  X=" << point.__str__();
  }
  maxLabel_->setText(QString::fromStdString(oss4.str()));
}


void MonteCarloResultWindow::probaValueChanged(double proba)
{
  quantileSpinBox_->blockSignals(true);
  quantileSpinBox_->setValue(result_.getResultSample().getMarginal(outputsComboBoxFirstTab_->currentIndex()).computeQuantile(proba)[0]);
  quantileSpinBox_->blockSignals(false);
}


void MonteCarloResultWindow::quantileValueChanged(double quantile)
{
  probaSpinBox_->blockSignals(true);
  double cdf = 0.0;
  double p(1.0 / double(result_.getResultSample().getSize()));

  for (int j = 0; j < result_.getResultSample().getSize(); ++j)
    if (result_.getResultSample()[j][outputsComboBoxFirstTab_->currentIndex()] < quantile)
      cdf += p;

  probaSpinBox_->setValue(cdf);
  probaSpinBox_->blockSignals(false);
}


void MonteCarloResultWindow::outputFirstTabChanged(int indexOutput)
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


void MonteCarloResultWindow::outputBoxPlotChanged(int indexOutput)
{
  boxPlot_->clear();

  double median = result_.getMedian()[indexOutput];
  double Q1 = result_.getFirstQuartile()[indexOutput];
  double Q3 = result_.getThirdQuartile()[indexOutput];

  boxPlot_->plotBoxPlot(median, Q1, Q3, Q1 - 1.5*(Q3-Q1), Q3 + 1.5*(Q3-Q1), result_.getOutliers()[indexOutput]);
}


void MonteCarloResultWindow::showHideGraphConfigurationWidget(int indexTab)
{
  // if plotWidget is visible
  if (indexTab == 2)
    emit graphWindowActivated(graphConfigurationWidget_);
  else
    emit graphWindowDeactivated(graphConfigurationWidget_);
}


void MonteCarloResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (newState == 4 || newState == 8 || newState == 10)
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == 0 || newState == 1 || newState == 2 || newState == 9)
    emit graphWindowDeactivated(graphConfigurationWidget_);
}
}