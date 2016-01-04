// SRCResultWindow.cxx

#include "otgui/SRCResultWindow.hxx"

#include "otgui/SRCAnalysis.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>

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
      indices_i[result_.getIndices()[i][j]] = j;
    indices_.push_back(indices_i);
  }
  buildInterface();
  connect(this, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)), this, SLOT(showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates)));
}


void SRCResultWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  plotLayout_ = new QStackedLayout(tab);

  NumericalPoint currentIndices(result_.getInputNames().getSize());
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
    currentIndices = result_.getIndices()[i];
    plot->plotSensitivityIndices(currentIndices, NumericalPoint(), sortedInputNames);
    plot->setAxisTitle(QwtPlot::xBottom, "Inputs");

    plot->setAxisTitle(QwtPlot::yLeft, result_.getOutputNames()[i].c_str());
    vbox->addWidget(plot);
    listPlotWidgets_.append(plot);

    // table of indices
    QTableWidget * table = new QTableWidget(0, 2, this);
    table->setHorizontalHeaderLabels(QStringList() << tr("Input") << tr("Indice"));
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

      item = new QTableWidgetItem(QString::number(result_.getIndices()[i][j], 'g', 4));
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      table->setItem(j, 1, item);
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


void SRCResultWindow::updateIndicesPlot(int section, Qt::SortOrder order)
{
  int indexOutput = plotLayout_->currentIndex();
  NumericalPoint currentIndices(result_.getInputNames().getSize());
  Description sortedInputNames(result_.getInputNames().getSize());

  switch (section)
  {
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

  listPlotWidgets_[indexOutput]->clear();
  listPlotWidgets_[indexOutput]->plotSensitivityIndices(currentIndices, NumericalPoint(), sortedInputNames);
}


void SRCResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (newState == 4 || newState == 8 || newState == 10)
    emit graphWindowActivated(plotsConfigurationWidget_);
  else if (newState == 0 || newState == 1 || newState == 2 || newState == 9)
    emit graphWindowDeactivated(plotsConfigurationWidget_);
}
}