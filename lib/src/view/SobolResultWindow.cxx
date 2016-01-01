// SobolResultWindow.cxx

#include "otgui/SobolResultWindow.hxx"

#include "otgui/SobolAnalysis.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>

using namespace OT;

namespace OTGUI {
  
SobolResultWindow::SobolResultWindow(SensitivityAnalysisItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<SobolAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
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


void SobolResultWindow::buildInterface()
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
    items << result_.getOutputNames()[i].c_str();
  outputsComboBox_->addItems(items);
  connect(outputsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(outputChanged(int)));
  headLayout->addWidget(outputsComboBox_);
  headLayout->addStretch();
  tabLayout->addLayout(headLayout);

  // graph
  indicesPlot_ = new PlotWidget(true);
  tabLayout->addWidget(indicesPlot_);

  updateIndicesPlot(-1, Qt::AscendingOrder);

  // table of indices
  table_ = new QTableWidget(0, 3, this);
  table_->setHorizontalHeaderLabels(QStringList() << tr("Input") << tr("First order indice") << tr("Total indice"));
  table_->verticalHeader()->hide();
  table_->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  connect(table_->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(updateIndicesPlot(int, Qt::SortOrder)));
  // fill table
  updateTable(0);
  table_->setSortingEnabled(true);

  tabLayout->addWidget(table_);

  tabWidget->addTab(tab, "Result");
  //
  setWidget(tabWidget);
}


void SobolResultWindow::outputChanged(int index)
{
  updateTable(index);
  updateIndicesPlot(-1, Qt::AscendingOrder);
}


void SobolResultWindow::updateIndicesPlot(int section, Qt::SortOrder order)
{
  Q_ASSERT(indicesPlot_);

  int indexOutput = outputsComboBox_->currentIndex();
  NumericalPoint currentFirstOrderIndices(result_.getInputNames().getSize());
  NumericalPoint currentTotalOrderIndices(result_.getInputNames().getSize());
  Description sortedInputNames(result_.getInputNames().getSize());

  switch (section)
  {
    case -1: // initial state
    {
      currentFirstOrderIndices = result_.getFirstOrderIndices()[indexOutput];
      currentTotalOrderIndices = result_.getTotalOrderIndices()[indexOutput];
      sortedInputNames = result_.getInputNames();
      break;
    }
    case 1:
    {
      int indice = 0;
      if (order == Qt::DescendingOrder)
      {
        for (std::map<double,int>::reverse_iterator it=totalOrderIndices_[indexOutput].rbegin(); it!=totalOrderIndices_[indexOutput].rend(); ++it, indice++)
        {
          currentFirstOrderIndices[indice] = result_.getFirstOrderIndices()[indexOutput][it->second];
          currentTotalOrderIndices[indice] = it->first;
          sortedInputNames[indice] = result_.getInputNames()[it->second];
        }
        break;
      }
      else
      {
        for (std::map<double,int>::iterator it=totalOrderIndices_[indexOutput].begin(); it!=totalOrderIndices_[indexOutput].end(); ++it, indice++)
        {
          currentFirstOrderIndices[indice] = result_.getFirstOrderIndices()[indexOutput][it->second];
          currentTotalOrderIndices[indice] = it->first;
          sortedInputNames[indice] = result_.getInputNames()[it->second];
        }
      }
    }
    case 2:
    {
      int indice = 0;
      if (order == Qt::DescendingOrder)
      {
        for (std::map<double,int>::reverse_iterator it=firstOrderIndices_[indexOutput].rbegin(); it!=firstOrderIndices_[indexOutput].rend(); ++it, indice++)
        {
          currentFirstOrderIndices[indice] = it->first;
          currentTotalOrderIndices[indice] = result_.getTotalOrderIndices()[indexOutput][it->second];
          sortedInputNames[indice] = result_.getInputNames()[it->second];
        }
      }
      else
      {
        for (std::map<double,int>::iterator it=firstOrderIndices_[indexOutput].begin(); it!=firstOrderIndices_[indexOutput].end(); ++it, indice++)
        {
          currentFirstOrderIndices[indice] = it->first;
          currentTotalOrderIndices[indice] = result_.getTotalOrderIndices()[indexOutput][it->second];
          sortedInputNames[indice] = result_.getInputNames()[it->second];
        }
      }
      break;
    }
    default:
      return;
  }

  indicesPlot_->clear();
  indicesPlot_->plotSensitivityIndices(currentFirstOrderIndices, currentTotalOrderIndices, sortedInputNames);
}


void SobolResultWindow::updateTable(int index)
{
  int indexOutput = outputsComboBox_->currentIndex();
  for (int i = 0; i < result_.getInputNames().getSize(); ++i)
  {
    table_->setRowCount(i + 1);
    QTableWidgetItem * item = new QTableWidgetItem(result_.getInputNames()[i].c_str());
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    table_->setItem(i, 0, item);

    item = new QTableWidgetItem(QString::number(result_.getFirstOrderIndices()[indexOutput][i], 'g', 4));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    table_->setItem(i, 1, item);

    item = new QTableWidgetItem(QString::number(result_.getTotalOrderIndices()[indexOutput][i], 'g', 4));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    table_->setItem(i, 2, item);
  }
}
}