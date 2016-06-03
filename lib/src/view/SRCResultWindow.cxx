//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of SRCAnalysis
 *
 *  Copyright 2015-2016 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/SRCResultWindow.hxx"

#include "otgui/SRCAnalysis.hxx"
#include "otgui/CopyableTableView.hxx"
#include "otgui/CustomStandardItemModel.hxx"


#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QScrollArea>
#include <QSplitter>

using namespace OT;

namespace OTGUI {
  
SRCResultWindow::SRCResultWindow(AnalysisItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<SRCAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
{
  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
  {
    std::map<double, int> indices_i;
    for (UnsignedInteger j=0; j<result_.getIndices().getDimension(); ++j)
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
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  scrollAreaWidget_ = new QStackedWidget;

  Description inputNames = result_.getInputNames();
  QStringList outputNames;
  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
    outputNames << result_.getOutputNames()[i].c_str();

  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
  {
    QSplitter * verticalSplitter = new QSplitter(Qt::Vertical);

    // plot
    PlotWidget * plot = new PlotWidget(true);
    plot->plotSensitivityIndices(result_.getIndices()[i], NumericalPoint(), inputNames);
    plot->setAxisTitle(QwtPlot::xBottom, tr("Inputs"));

    verticalSplitter->addWidget(plot);
    verticalSplitter->setStretchFactor(0, 3);
    listPlotWidgets_.append(plot);

    // table of indices
    CopyableTableView * table = new CopyableTableView;
    table->verticalHeader()->hide();
#if QT_VERSION >= 0x050000
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    CustomStandardItemModel * model = new CustomStandardItemModel(inputNames.getSize(), 2);
    model->setHorizontalHeaderLabels(QStringList() << tr("Input") << tr("Index"));
    table->setModel(model);

    // fill table
    for (UnsignedInteger j=0; j<inputNames.getSize(); ++j)
    {
      model->setNotEditableItem(j, 0, inputNames[j].c_str());
      model->setNotEditableItem(j, 1, result_.getIndices()[i][j]);
    }
    table->setSortingEnabled(true);
    connect(table->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(updateIndicesPlot(int, Qt::SortOrder)));

    verticalSplitter->addWidget(table);
    verticalSplitter->setStretchFactor(1, 1);
    scrollAreaWidget_->addWidget(verticalSplitter);
  }

  plotsConfigurationWidget_ = new GraphConfigurationWidget(listPlotWidgets_, QStringList(), outputNames, GraphConfigurationWidget::SensitivityIndices);
  connect(plotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), scrollAreaWidget_, SLOT(setCurrentIndex(int)));

  scrollArea->setWidget(scrollAreaWidget_);
  tabWidget->addTab(scrollArea, tr("Result"));
  //
  setWidget(tabWidget);
}


void SRCResultWindow::updateIndicesPlot(int section, Qt::SortOrder order)
{
  int indexOutput = scrollAreaWidget_->currentIndex();
  NumericalPoint currentIndices(result_.getInputNames().getSize());
  Description sortedInputNames(result_.getInputNames().getSize());

  switch (section)
  {
    case 1:
    {
      int index = 0;
      if (order == Qt::DescendingOrder)
      {
        for (std::map<double,int>::reverse_iterator it=indices_[indexOutput].rbegin(); it!=indices_[indexOutput].rend(); ++it, index++)
        {
          currentIndices[index] = result_.getIndices()[indexOutput][it->second];
          sortedInputNames[index] = result_.getInputNames()[it->second];
        }
      }
      else
      {
        for (std::map<double,int>::iterator it=indices_[indexOutput].begin(); it!=indices_[indexOutput].end(); ++it, index++)
        {
          currentIndices[index] = result_.getIndices()[indexOutput][it->second];
          sortedInputNames[index] = result_.getInputNames()[it->second];
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
  if (oldState == 2)
    return;

  if (newState == 4 || newState == 10)
    if (!plotsConfigurationWidget_->isVisible())
      emit graphWindowActivated(plotsConfigurationWidget_);
  else if (newState == 0 || newState == 1 || newState == 9)
    emit graphWindowDeactivated();
}
}
