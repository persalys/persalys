// SRCResultWindow.cxx

#include "otgui/SRCResultWindow.hxx"

#include "otgui/SRCAnalysis.hxx"

#include <QVBoxLayout>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {
  
SRCResultWindow::SRCResultWindow(SensitivityAnalysisItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<SRCAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
{
  for (int i=0; i<result_.getOutputNames().getSize(); ++i)
  {
    std::map<double, int> indices_i;
    for (int j=0; j<result_.getIndices().getDimension(); ++j)
    {
      indices_i[result_.getIndices()[i][j]] = j;
    }
    indices_.push_back(indices_i);
  }
  buildInterface();
}


void SRCResultWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  outputsComboBox_ = new QComboBox;
  QStringList items = QStringList();
  for (int i=0; i<result_.getOutputNames().getSize(); ++i)
    items<<QString::fromStdString(result_.getOutputNames()[i]);
  outputsComboBox_->addItems(items);
  connect(outputsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(outputChanged(int)));
  headLayout->addWidget(outputsComboBox_);
  headLayout->addStretch();
  tabLayout->addLayout(headLayout);

  // graph
  QHBoxLayout * mainLayout = new QHBoxLayout;
  barChart_ = new PlotWidget(true);
  mainLayout->addWidget(barChart_->getPlotLabel());

  sortComboBox_ = new QComboBox;
  items = QStringList()<<tr("True")<<tr("False");
  sortComboBox_->addItems(items);
  connect(sortComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBarChart()));
  mainLayout->addWidget(sortComboBox_);

  mainLayout->addStretch();
  tabLayout->addLayout(mainLayout);

  updateBarChart();

  // table of indices
  table_ = new QTableWidget(0, 2, this);
  table_->setHorizontalHeaderLabels(QStringList() << tr("Input") << tr("Indice"));
  table_->verticalHeader()->hide();
  table_->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  table_->setSortingEnabled(true);

  updateTable(0);
  tabLayout->addWidget(table_);

  tabWidget->addTab(tab, "Result");
  //
  setWidget(tabWidget);
}


void SRCResultWindow::outputChanged(int index)
{
  updateTable(index);
  updateBarChart();
}


void SRCResultWindow::updateBarChart()
{
  Q_ASSERT(barChart_);

  int indexOutput = outputsComboBox_->currentIndex();
  NumericalPoint currentIndices(result_.getInputNames().getSize());
  Description sortedInputNames(result_.getInputNames().getSize());

  switch (sortComboBox_->currentIndex())
  {
    case 0:
    {
      int indice = 0;
      for (std::map<double,int>::reverse_iterator it=indices_[indexOutput].rbegin(); it!=indices_[indexOutput].rend(); ++it)
      {
        currentIndices[indice] = result_.getIndices()[indexOutput][it->second];
        sortedInputNames[indice] = result_.getInputNames()[it->second];
        ++indice;
      }
      break;
    }
    case 1:
    {
      currentIndices = result_.getIndices()[indexOutput];
      sortedInputNames = result_.getInputNames();
      break;
    }
  }

  barChart_->clear();
  barChart_->plotBarChart(currentIndices, NumericalPoint(), sortedInputNames);
}


void SRCResultWindow::updateTable(int index)
{
  int indexOutput = outputsComboBox_->currentIndex();
  for ( int i = 0; i < result_.getInputNames().getSize(); ++i)
  {
    table_->setRowCount(i + 1);
    table_->setItem(i, 0, new QTableWidgetItem(result_.getInputNames()[i].c_str()));
    table_->setItem(i, 1, new QTableWidgetItem(QString::number(result_.getIndices()[indexOutput][i], 'g', 4)));
  }
}

}