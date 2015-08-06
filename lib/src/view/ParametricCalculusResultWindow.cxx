// ParametricCalculusResultWindow.cxx

#include "ParametricCalculusResultWindow.hxx"
#include "OTguiTableView.hxx"

#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>

#include <iostream>
#include <string>

namespace OTGUI {

ParametricCalculusResultWindow::ParametricCalculusResultWindow(ParametricCalculusItem * item)
  : OTguiSubWindow(item)
  , inputSample_(item->getInputSample())
  , outputSample_(item->getResultSample())
{
  buildInterface();
}


ParametricCalculusResultWindow::~ParametricCalculusResultWindow()
{
}


void ParametricCalculusResultWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  // first tab
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout;
  QLabel * label = new QLabel;
  OT::OSS oss;
  int size = inputSample_.getSize();
  int numberInputs = inputSample_.getDimension();
  OT::NumericalSample sample = inputSample_;
  sample.stack(outputSample_);

  OT::Indices indicesInputs(numberInputs);
  indicesInputs.fill();

  for (int i=numberInputs; i<sample.getDimension(); ++i)
  {
    oss <<"output : " << sample.getDescription()[i] << "\n";
    OT::NumericalSample orderedSample = sample.sortAccordingToAComponent(i);

    double minValue = orderedSample[0][i];
    oss << "- min = " << minValue << "\n";
    int it=0;

    double value = orderedSample[it][i];
    while (value == minValue)
    {
      OT::NumericalPoint point = orderedSample.getMarginal(indicesInputs)[it];
      oss << "  X=" <<point.__str__() << "\n";
      ++it;
      if (it < size)
        value = orderedSample[it][i];
      else
        break;
    }
    oss << "\n";

    double maxValue = orderedSample[size-1][i];
    oss << "- max = " << maxValue << "\n";
    it=0;

    value = orderedSample[size-1-it][i];
    while (value == maxValue)
    {
      OT::NumericalPoint point = orderedSample.getMarginal(indicesInputs)[size-1-it];
      oss << "  X=" <<point.__str__() << "\n";
      ++it;
      if (it < size)
        value = orderedSample[size-1-it][i];
      else
        break;
    }
    oss << "\n";

  }

  label->setText(QString::fromStdString(oss.str()));
  tabLayout->addWidget(label);

  tabLayout->addStretch();
  tab->setLayout(tabLayout);
  tabWidget->addTab(tab, tr("Result"));

  // second tab
  tabLayout = new QVBoxLayout;
  OTguiTableView * tabResultView = new OTguiTableView(sample);
  tabLayout->addWidget(tabResultView);

  tab = new QWidget;
  tab->setLayout(tabLayout);
  tabWidget->addTab(tab, tr("Result table"));

  // third tab
  tabLayout = new QVBoxLayout;

  QHBoxLayout * hLayout = new QHBoxLayout;
  outputsComboBox_ = new QComboBox;
  QStringList items = QStringList();
  for (int i=0; i<outputSample_.getDimension(); ++i)
    items<<QString::fromStdString(outputSample_.getDescription()[i]);
  outputsComboBox_->addItems(items);
  connect(outputsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(outputChanged(int)));
  hLayout->addWidget(outputsComboBox_);
  hLayout->addStretch();
  tabLayout->addLayout(hLayout);

  scatterPlot_ = new PlotWidget();
  tabLayout->addWidget(scatterPlot_);

  hLayout = new QHBoxLayout;
  inputsComboBox_ = new QComboBox;
  items = QStringList();
  for (int i=0; i<numberInputs; ++i)
    items<<QString::fromStdString(sample.getDescription()[i]);
  inputsComboBox_->addItems(items);
  connect(inputsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(inputChanged(int)));
  hLayout->addStretch();
  hLayout->addWidget(inputsComboBox_);
  tabLayout->addLayout(hLayout);

  updatePlot();

  tab = new QWidget;
  tab->setLayout(tabLayout);
  tabWidget->addTab(tab, tr("Scatter Plots"));
  

  ////////////////
  setWidget(tabWidget);
}


void ParametricCalculusResultWindow::inputChanged(int index)
{
  updatePlot(index, outputsComboBox_->currentIndex());
}


void ParametricCalculusResultWindow::outputChanged(int index)
{
  updatePlot(inputsComboBox_->currentIndex(), index);
}


void ParametricCalculusResultWindow::updatePlot(int indexInput, int indexOutput)
{
  Q_ASSERT(scatterPlot_);
  scatterPlot_->clear();
  scatterPlot_->plotScatter(inputSample_.getMarginal(indexInput), outputSample_.getMarginal(indexOutput));
  scatterPlot_->replot();
}


}