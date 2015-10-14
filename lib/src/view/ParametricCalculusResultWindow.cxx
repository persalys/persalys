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
  , result_(dynamic_cast<ParametricCalculus*>(&*item->getCalculus().getImplementation())->getResult())
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

  for (int i=0; i<result_.getResultSample().getDimension(); ++i)
  {
    oss <<"Output : " << result_.getOutputNames()[i] << "\n";
    oss << "- Min = " << result_.getListMin()[i]<< "\n";
    for (int j=0; j<result_.getListXMin()[i].getSize();++j)
    {
      OT::NumericalPoint point(result_.getListXMin()[i][j]);
      oss << "  X=" <<point.__str__() << "\n";
    }
    oss << "- Max = " << result_.getListMax()[i] << "\n";
    for (int j=0; j<result_.getListXMax()[i].getSize();++j)
    {
      OT::NumericalPoint point(result_.getListXMax()[i][j]);
      oss << "  X=" <<point.__str__() << "\n";
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
  OT::NumericalSample sample = result_.getInputSample();
  sample.stack(result_.getResultSample());
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
  for (int i=0; i<result_.getResultSample().getDimension(); ++i)
    items<<QString::fromStdString(result_.getOutputNames()[i]);
  outputsComboBox_->addItems(items);
  connect(outputsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(outputChanged(int)));
  hLayout->addWidget(outputsComboBox_);
  hLayout->addStretch();
  tabLayout->addLayout(hLayout);

  scatterPlot_ = new PlotWidget();
  tabLayout->addWidget(scatterPlot_->getPlotLabel());

  hLayout = new QHBoxLayout;
  inputsComboBox_ = new QComboBox;
  items = QStringList();
  for (int i=0; i<result_.getInputSample().getDimension(); ++i)
    items<<QString::fromStdString(result_.getInputNames()[i]);
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
  scatterPlot_->plotScatter(result_.getInputSample().getMarginal(indexInput), result_.getResultSample().getMarginal(indexOutput));
  scatterPlot_->replot();
}


}