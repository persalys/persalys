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
  // data
  int nbInputs = result_.getInputSample().getDimension();
  QStringList inputNames;
  for (int i=0; i<nbInputs; ++i)
    inputNames << physicalModel_.getStochasticInputNames()[i].c_str();

  int nbOutputs = result_.getOutputSample().getDimension();
  OutputCollection outputs = physicalModel_.getOutputs();
  QStringList outputNames;
  for (int i=0; i<nbOutputs; ++i)
    outputNames << result_.getOutputSample().getDescription()[i].c_str();

  // tabWidget
  tabWidget_ = new QTabWidget;

  // first tab: Table --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  NumericalSample sample = result_.getInputSample();
  sample.stack(result_.getOutputSample());
  OTguiTableView * tabResultView = new OTguiTableView(sample);
  tabLayout->addWidget(tabResultView);

  tabWidget_->addTab(tab, tr("Result table"));

  // if the sample contains nan:
  if (!dynamic_cast<OTguiTableModel*>(tabResultView->model())->sampleIsValid())
  {
    setWidget(tabWidget_);
    return;
  }

  // second tab: Summary -----------------------------
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);

  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  outputsComboBoxFirstTab_ = new QComboBox;
  outputsComboBoxFirstTab_->addItems(outputNames);
  connect(outputsComboBoxFirstTab_, SIGNAL(currentIndexChanged(int)), this, SLOT(outputFirstTabChanged(int)));
  headLayout->addWidget(outputsComboBoxFirstTab_);
  headLayout->addStretch();
  tabLayout->addLayout(headLayout);

  QVBoxLayout * vbox = new QVBoxLayout;
  QGridLayout * grid = new QGridLayout;
  int gridRow = -1;

  QLabel * nbSimuLabel = new QLabel(tr("Number of simulations : ") + QString::number(result_.getInputSample().getSize()) + "\n");
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

  label = new QLabel(tr("Mean"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  meanLabel_ = new QLabel;
  grid->addWidget(meanLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("Standard deviation"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  stdLabel_ = new QLabel;
  grid->addWidget(stdLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("Skewness"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  skewnessLabel_ = new QLabel;
  grid->addWidget(skewnessLabel_, gridRow, 1);

  label = new QLabel(tr("Kurtosis"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  kurtosisLabel_ = new QLabel;
  grid->addWidget(kurtosisLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("First quartile"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  firstQuartileLabel_ = new QLabel;
  grid->addWidget(firstQuartileLabel_, gridRow, 1, Qt::AlignTop);

  label = new QLabel(tr("Third quartile"));
  label->setStyleSheet("font: bold;");
  grid->addWidget(label, ++gridRow, 0, Qt::AlignTop);
  thirdQuartileLabel_ = new QLabel;
  grid->addWidget(thirdQuartileLabel_, gridRow, 1, Qt::AlignTop);

  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);
  updateLabelsText();

  // quantiles
  QHBoxLayout * quantLayout = new QHBoxLayout;
  label = new QLabel(tr("Probability"));
  label->setStyleSheet("font: bold;");
  quantLayout->addWidget(label);
  probaSpinBox_ = new QDoubleSpinBox;
  label->setBuddy(probaSpinBox_);
  probaSpinBox_->setMinimum(0.0);
  probaSpinBox_->setMaximum(1.0);
  probaSpinBox_->setSingleStep(0.01);
  probaSpinBox_->setValue(0.5);
  quantLayout->addWidget(probaSpinBox_);
  label = new QLabel(tr("Quantile"));
  label->setStyleSheet("font: bold;");
  quantLayout->addWidget(label);
  quantileSpinBox_ = new QDoubleSpinBox;
  label->setBuddy(quantileSpinBox_);
  quantileSpinBox_->setMinimum(std::numeric_limits<double>::min());
  quantileSpinBox_->setMaximum(std::numeric_limits<double>::max());
  quantLayout->addWidget(quantileSpinBox_);

  connect(probaSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(probaValueChanged(double)));
  connect(quantileSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(quantileValueChanged(double)));
  probaValueChanged(0.5);

  quantLayout->addStretch();
  vbox->addLayout(quantLayout);
  vbox->addStretch();
  tabLayout->addLayout(vbox);

  tabWidget_->addTab(tab, tr("Summary"));

  // third tab: PDF/CDF ------------------------------
  tab = new QWidget;
  QStackedLayout * plotLayout = new QStackedLayout(tab);

  QVector<PlotWidget*> listPlotWidgets;

  for (int i=0; i<nbOutputs; ++i)
  {
    PlotWidget * plot = new PlotWidget;
    plot->plotHistogram(result_.getOutputSample().getMarginal(i));
    plot->plotPDFCurve(result_.getFittedDistribution()[i]);
    plot->setTitle(tr("PDF ") + outputs[i].getName().c_str());
    plotLayout->addWidget(plot);
    listPlotWidgets.append(plot);

    plot = new PlotWidget;
    plot->plotHistogram(result_.getOutputSample().getMarginal(i), true);
    plot->plotCDFCurve(result_.getFittedDistribution()[i]);
    plot->setTitle(tr("CDF ") + outputs[i].getName().c_str());
    plotLayout->addWidget(plot);
    listPlotWidgets.append(plot);
  }

  pdf_cdfPlotsConfigurationWidget_ = new GraphConfigurationWidget(listPlotWidgets, QStringList(), outputNames, GraphConfigurationWidget::PDFResult);
  connect(pdf_cdfPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), plotLayout, SLOT(setCurrentIndex(int)));

  tabWidget_->addTab(tab, tr("PDF/CDF"));

  // fourth tab: box plots ---------------------------
  tab = new QWidget;
  plotLayout = new QStackedLayout(tab);

  QVector<PlotWidget*> listBoxPlotWidgets;

  for (int i=0; i<nbOutputs; ++i)
  {
    PlotWidget * plot = new PlotWidget;
    double median = result_.getMedian()[i];
    double Q1 = result_.getFirstQuartile()[i];
    double Q3 = result_.getThirdQuartile()[i];

    plot->plotBoxPlot(median, Q1, Q3, Q1 - 1.5*(Q3-Q1), Q3 + 1.5*(Q3-Q1), result_.getOutliers()[i]);
    plot->setTitle(tr("Box plot"));
    if (outputs[i].getDescription().size())
      plot->setAxisTitle(QwtPlot::yLeft, outputs[i].getDescription().c_str());
    else
      plot->setAxisTitle(QwtPlot::yLeft, outputs[i].getName().c_str());
    plotLayout->addWidget(plot);
    listBoxPlotWidgets.append(plot);
  }

  boxPlotsConfigurationWidget_ = new GraphConfigurationWidget(listBoxPlotWidgets, QStringList(), outputNames, GraphConfigurationWidget::BoxPlot);
  connect(boxPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), plotLayout, SLOT(setCurrentIndex(int)));

  tabWidget_->addTab(tab, tr("Box plots"));

  // fifth tab: scatter plots ------------------------
  tab = new QWidget;
  plotLayout = new QStackedLayout(tab);

  QVector<PlotWidget*> listScatterPlotWidgets;

  for (int j=0; j<nbInputs; ++j)
  {
    for (int i=0; i<nbOutputs; ++i)
    {
      PlotWidget * plot = new PlotWidget;
      plot->plotScatter(result_.getInputSample().getMarginal(j), result_.getOutputSample().getMarginal(i));
      plot->setTitle(tr("Scatter plot: ") + outputs[i].getName().c_str() + tr(" vs ") + inputNames[j]);
      String inputDescription = physicalModel_.getInputByName(inputNames[j].toStdString()).getDescription();
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
      plot->plotScatter(result_.getInputSample().getMarginal(j), result_.getInputSample().getMarginal(i));
      plot->setTitle(tr("Scatter plot: ") + inputNames[i] + tr(" vs ") + inputNames[j]);
      String inputDescription = physicalModel_.getInputByName(inputNames[j].toStdString()).getDescription();
      if (!inputDescription.empty())
        plot->setAxisTitle(QwtPlot::xBottom, inputDescription.c_str());
      else
        plot->setAxisTitle(QwtPlot::xBottom, inputNames[j]);
      inputDescription = physicalModel_.getInputByName(inputNames[i].toStdString()).getDescription();
      if (!inputDescription.empty())
        plot->setAxisTitle(QwtPlot::yLeft, inputDescription.c_str());
      else
        plot->setAxisTitle(QwtPlot::yLeft, inputNames[i]);
      plotLayout->addWidget(plot);
      listScatterPlotWidgets.append(plot);
    }
  }

  scatterPlotsConfigurationWidget_ = new GraphConfigurationWidget(listScatterPlotWidgets, inputNames, outputNames, GraphConfigurationWidget::Scatter);
  connect(scatterPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), plotLayout, SLOT(setCurrentIndex(int)));

  tabWidget_->addTab(tab, tr("Scatter plots"));

  // sixth tab: plot matrix Y-X ----------------------
  tab = new PlotMatrixWidget(result_.getInputSample(), result_.getOutputSample());
  plotMatrixConfigurationWidget_ = new PlotMatrixConfigurationWidget(dynamic_cast<PlotMatrixWidget*>(tab));

  tabWidget_->addTab(tab, tr("Plot matrix Y-X"));

  // seventh tab: plot matrix X-X ----------------------
  tab = new PlotMatrixWidget(result_.getInputSample(), result_.getInputSample());
  plotMatrix_X_X_ConfigurationWidget_ = new PlotMatrixConfigurationWidget(dynamic_cast<PlotMatrixWidget*>(tab));

  tabWidget_->addTab(tab, tr("Plot matrix X-X"));

  //
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));
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
  oss3 << result_.getOutputSample().getMin()[indexOutput];
  for (UnsignedInteger j=0; j<result_.getListXMin()[indexOutput].getSize();++j)
  {
    NumericalPoint point(result_.getListXMin()[indexOutput][j]);
    oss3 << "\n  X=" << point.__str__();
  }
  minLabel_->setText(QString::fromStdString(oss3.str()));
  // max
  OSS oss4;
  oss4 << result_.getOutputSample().getMax()[indexOutput];
  for (UnsignedInteger j=0; j<result_.getListXMax()[indexOutput].getSize();++j)
  {
    NumericalPoint point(result_.getListXMax()[indexOutput][j]);
    oss4 << "\n  X=" << point.__str__();
  }
  maxLabel_->setText(QString::fromStdString(oss4.str()));
}


void MonteCarloResultWindow::probaValueChanged(double proba)
{
  quantileSpinBox_->blockSignals(true);
  quantileSpinBox_->setValue(result_.getOutputSample().getMarginal(outputsComboBoxFirstTab_->currentIndex()).computeQuantile(proba)[0]);
  quantileSpinBox_->blockSignals(false);
}


void MonteCarloResultWindow::quantileValueChanged(double quantile)
{
  probaSpinBox_->blockSignals(true);
  double cdf = 0.0;
  double p = 1.0 / double(result_.getOutputSample().getSize());

  for (UnsignedInteger j=0; j<result_.getOutputSample().getSize(); ++j)
    if (result_.getOutputSample()[j][outputsComboBoxFirstTab_->currentIndex()] < quantile)
      cdf += p;

  probaSpinBox_->setValue(cdf);
  probaSpinBox_->blockSignals(false);
}


void MonteCarloResultWindow::outputFirstTabChanged(int indexOutput)
{
  updateLabelsText(indexOutput);
  probaValueChanged(0.5);
}


void MonteCarloResultWindow::showHideGraphConfigurationWidget(int indexTab)
{
  switch (indexTab)
  {
    // if a plotWidget is visible
    case 2:
      emit graphWindowActivated(pdf_cdfPlotsConfigurationWidget_);
      break;
    case 3:
      emit graphWindowActivated(boxPlotsConfigurationWidget_);
      break;
    case 4:
      emit graphWindowActivated(scatterPlotsConfigurationWidget_);
      break;
    case 5:
      emit graphWindowActivated(plotMatrixConfigurationWidget_);
      break;
    case 6:
      emit graphWindowActivated(plotMatrix_X_X_ConfigurationWidget_);
      break;
    // if no plotWidget is visible
    default:
    {
      emit graphWindowDeactivated(pdf_cdfPlotsConfigurationWidget_);
      emit graphWindowDeactivated(boxPlotsConfigurationWidget_);
      emit graphWindowDeactivated(scatterPlotsConfigurationWidget_);
      emit graphWindowDeactivated(plotMatrixConfigurationWidget_);
      emit graphWindowDeactivated(plotMatrix_X_X_ConfigurationWidget_);
      break;
    }
  }
}


void MonteCarloResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (newState == 4 || newState == 8 || newState == 10)
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == 0 || newState == 1 || newState == 2 || newState == 9)
    showHideGraphConfigurationWidget(-1);
}
}