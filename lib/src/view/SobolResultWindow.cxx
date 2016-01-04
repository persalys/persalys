// SobolResultWindow.cxx

#include "otgui/SobolResultWindow.hxx"

#include "otgui/SobolAnalysis.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QStackedLayout>
#include <QTableWidget>

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
  connect(this, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)), this, SLOT(showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates)));
}


void SobolResultWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  plotLayout_ = new QStackedLayout(tab);

  NumericalPoint currentFirstOrderIndices(result_.getInputNames().getSize());
  NumericalPoint currentTotalOrderIndices(result_.getInputNames().getSize());
  Description sortedInputNames = result_.getInputNames();
  QStringList outputNames;
  for (int i=0; i<result_.getOutputNames().getSize(); ++i)
    outputNames << result_.getOutputNames()[i].c_str();

  for (int i=0; i<result_.getOutputNames().getSize(); ++i)
  {
    QWidget * widget = new QWidget;
    QVBoxLayout * vbox = new QVBoxLayout(widget);

    // plot
    PlotWidget * plot = new PlotWidget;
    currentFirstOrderIndices = result_.getFirstOrderIndices()[i];
    currentTotalOrderIndices = result_.getTotalOrderIndices()[i];
    plot->plotSensitivityIndices(currentFirstOrderIndices, currentTotalOrderIndices, sortedInputNames);
    plot->setAxisTitle(QwtPlot::xBottom, "Inputs");

    plot->setAxisTitle(QwtPlot::yLeft, result_.getOutputNames()[i].c_str());
    vbox->addWidget(plot);
    listPlotWidgets_.append(plot);

    // table of indices
    QTableWidget * table = new QTableWidget(0, 3, this);
    table->setHorizontalHeaderLabels(QStringList() << tr("Input") << tr("First order indice") << tr("Total indice"));
    table->verticalHeader()->hide();
    table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    connect(table->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(updateIndicesPlot(int, Qt::SortOrder)));
    table->setSortingEnabled(true);
    // fill table
    for (int j=0; j<result_.getInputNames().getSize(); ++j)
    {
      table->setRowCount(j + 1);
      QTableWidgetItem * item = new QTableWidgetItem(result_.getInputNames()[j].c_str());
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      table->setItem(j, 0, item);

      item = new QTableWidgetItem(QString::number(result_.getFirstOrderIndices()[i][j], 'g', 4));
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      table->setItem(j, 1, item);

      item = new QTableWidgetItem(QString::number(result_.getTotalOrderIndices()[i][j], 'g', 4));
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      table->setItem(j, 2, item);
    }
    vbox->addWidget(table);
    plotLayout_->addWidget(widget);
  }

  plotsConfigurationWidget_ = new GraphConfigurationWidget(listPlotWidgets_, QStringList(), outputNames, GraphConfigurationWidget::SensitivityIndices);
  connect(plotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), plotLayout_, SLOT(setCurrentIndex(int)));

  tabWidget->addTab(tab, "Result");
  //
  setWidget(tabWidget);
}


void SobolResultWindow::updateIndicesPlot(int section, Qt::SortOrder order)
{
  int indexOutput = plotLayout_->currentIndex();
  NumericalPoint currentFirstOrderIndices(result_.getInputNames().getSize());
  NumericalPoint currentTotalOrderIndices(result_.getInputNames().getSize());
  Description sortedInputNames(result_.getInputNames().getSize());

  switch (section)
  {
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
      break;
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

  listPlotWidgets_[indexOutput]->clear();
  listPlotWidgets_[indexOutput]->plotSensitivityIndices(currentFirstOrderIndices, currentTotalOrderIndices, sortedInputNames);
}


void SobolResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (newState == 4 || newState == 8 || newState == 10)
    emit graphWindowActivated(plotsConfigurationWidget_);
  else if (newState == 0 || newState == 1 || newState == 2 || newState == 9)
    emit graphWindowDeactivated(plotsConfigurationWidget_);
}
}