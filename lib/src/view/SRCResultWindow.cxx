// SRCResultWindow.cxx

#include "otgui/SRCResultWindow.hxx"

#include "otgui/SRCAnalysis.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>

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
  table_ = new QTableWidget(0, 2, this);
  table_->setHorizontalHeaderLabels(QStringList() << tr("Input") << tr("Indice"));
  table_->verticalHeader()->hide();
  table_->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  connect(table_->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(updateIndicesPlot(int, Qt::SortOrder)));
  updateTable(0);
  table_->setSortingEnabled(true);
  tabLayout->addWidget(table_);

  tabWidget->addTab(tab, "Result");
  //
  setWidget(tabWidget);
}


void SRCResultWindow::outputChanged(int index)
{
  updateTable(index);
  updateIndicesPlot(-1, Qt::AscendingOrder);
}


void SRCResultWindow::updateIndicesPlot(int section, Qt::SortOrder order)
{
  Q_ASSERT(indicesPlot_);

  int indexOutput = outputsComboBox_->currentIndex();
  NumericalPoint currentIndices(result_.getInputNames().getSize());
  Description sortedInputNames(result_.getInputNames().getSize());

  switch (section)
  {
    case -1:
    {
      currentIndices = result_.getIndices()[indexOutput];
      sortedInputNames = result_.getInputNames();
      break;
    }
    case 1:
    {
      int indice = 0;
      if (order == Qt::DescendingOrder)
      {
        for (std::map<double,int>::reverse_iterator it=indices_[indexOutput].rbegin(); it!=indices_[indexOutput].rend(); ++it, indice++)
        {
          currentIndices[indice] = result_.getIndices()[indexOutput][it->second];
          sortedInputNames[indice] = result_.getInputNames()[it->second];
        }
      }
      else
      {
        for (std::map<double,int>::iterator it=indices_[indexOutput].begin(); it!=indices_[indexOutput].end(); ++it, indice++)
        {
          currentIndices[indice] = result_.getIndices()[indexOutput][it->second];
          sortedInputNames[indice] = result_.getInputNames()[it->second];
        }
      }
      break;
    }
    default:
      return;
  }

  indicesPlot_->clear();
  indicesPlot_->plotSensitivityIndices(currentIndices, NumericalPoint(), sortedInputNames);
}


void SRCResultWindow::updateTable(int index)
{
  int indexOutput = outputsComboBox_->currentIndex();
  for ( int i = 0; i < result_.getInputNames().getSize(); ++i)
  {
    table_->setRowCount(i + 1);
    QTableWidgetItem * item = new QTableWidgetItem(result_.getInputNames()[i].c_str());
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    table_->setItem(i, 0, item);

    item = new QTableWidgetItem(QString::number(result_.getIndices()[indexOutput][i], 'g', 4));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    table_->setItem(i, 1, item);
  }
}
}