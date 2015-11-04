// SobolCalculusResultWindow.cxx

#include "otgui/SobolCalculusResultWindow.hxx"

#include "otgui/SobolCalculus.hxx"

#include <QVBoxLayout>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {
  
SobolCalculusResultWindow::SobolCalculusResultWindow(SensitivityAnalysisItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<SobolCalculus*>(&*item->getCalculus().getImplementation())->getResult())
{
  for (int i=0; i<result_.getOutputNames().getSize(); ++i)
  {
    std::map<double, int> firstOrder_i;
    std::map<double, int> totalOrder_i;
    for (int j=0; j<result_.getFirstOrderIndices().getDimension(); ++j)
    {
      firstOrder_i[result_.getFirstOrderIndices()[i][j]] = j;
      totalOrder_i[result_.getTotalOrderIndices()[i][j]] = j;
    }
    firstOrderIndices_.push_back(firstOrder_i);
    totalOrderIndices_.push_back(totalOrder_i);
  }
  buildInterface();
}


void SobolCalculusResultWindow::buildInterface()
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
  items = QStringList()<<tr("Total")<<tr("First")<<tr("None");
  sortComboBox_->addItems(items);
  connect(sortComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBarChart()));
  mainLayout->addWidget(sortComboBox_);

  mainLayout->addStretch();
  tabLayout->addLayout(mainLayout);

  updateBarChart();

  // table of indices
  table_ = new QTableWidget(0, 3, this);
  table_->setHorizontalHeaderLabels(QStringList() << tr("Input") << tr("First order indice") << tr("Total indice"));
  table_->verticalHeader()->hide();
  table_->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  table_->setSortingEnabled(true);

  updateTable(0);
  tabLayout->addWidget(table_);

  tabWidget->addTab(tab, "Result");
  //
  setWidget(tabWidget);
}


void SobolCalculusResultWindow::outputChanged(int index)
{
  updateTable(index);
  updateBarChart();
}


void SobolCalculusResultWindow::updateBarChart()
{
  Q_ASSERT(barChart_);

  int indexOutput = outputsComboBox_->currentIndex();
  NumericalPoint currentFirstOrderIndices(result_.getInputNames().getSize());
  NumericalPoint currentTotalOrderIndices(result_.getInputNames().getSize());
  Description sortedInputNames(result_.getInputNames().getSize());

  switch (sortComboBox_->currentIndex())
  {
    case 0:
    {
      int indice = 0;
      for (std::map<double,int>::reverse_iterator it=totalOrderIndices_[indexOutput].rbegin(); it!=totalOrderIndices_[indexOutput].rend(); ++it)
      {
        currentFirstOrderIndices[indice] = result_.getFirstOrderIndices()[indexOutput][it->second];
        currentTotalOrderIndices[indice] = it->first;
        sortedInputNames[indice] = result_.getInputNames()[it->second];
        ++indice;
      }
      break;
    }
    case 1:
    {
      int indice = 0;
      for (std::map<double,int>::reverse_iterator it=firstOrderIndices_[indexOutput].rbegin(); it!=firstOrderIndices_[indexOutput].rend(); ++it)
      {
        currentFirstOrderIndices[indice] = it->first;
        currentTotalOrderIndices[indice] = result_.getTotalOrderIndices()[indexOutput][it->second];
        sortedInputNames[indice] = result_.getInputNames()[it->second];
        ++indice;
      }
      break;
    }
    case 2:
    {
      currentFirstOrderIndices = result_.getFirstOrderIndices()[indexOutput];
      currentTotalOrderIndices = result_.getTotalOrderIndices()[indexOutput];
      sortedInputNames = result_.getInputNames();
      break;
    }
  }

  barChart_->clear();
  barChart_->plotBarChart(currentFirstOrderIndices, currentTotalOrderIndices, sortedInputNames);
}


void SobolCalculusResultWindow::updateTable(int index)
{
  int indexOutput = outputsComboBox_->currentIndex();
  for ( int i = 0; i < result_.getInputNames().getSize(); ++i)
  {
    table_->setRowCount(i + 1);
    table_->setItem(i, 0, new QTableWidgetItem(result_.getInputNames()[i].c_str()));
    table_->setItem(i, 1, new QTableWidgetItem(QString::number(result_.getFirstOrderIndices()[indexOutput][i], 'g', 4)));
    table_->setItem(i, 2, new QTableWidgetItem(QString::number(result_.getTotalOrderIndices()[indexOutput][i], 'g', 4)));
  }
}

}