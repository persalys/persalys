//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of SobolAnalysis
 *
 *  Copyright 2015-2016 EDF
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
#include "otgui/SobolResultWindow.hxx"

#include "otgui/SobolAnalysis.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QScrollArea>
#include <QSplitter>

using namespace OT;

namespace OTGUI {
  
SobolResultWindow::SobolResultWindow(AnalysisItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<SobolAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
{
  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
  {
    std::map<double, int> firstOrder_i;
    std::map<double, int> totalOrder_i;
    for (UnsignedInteger j=0; j<result_.getFirstOrderIndices().getDimension(); ++j)
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
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QFrame * frame = new QFrame;
  frameLayout_ = new QStackedLayout(frame);

  Description inputNames = result_.getInputNames();
  QStringList outputNames;
  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
    outputNames << result_.getOutputNames()[i].c_str();

  NumericalPoint interactionsValues(result_.getOutputNames().getSize());

  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
  {
    QSplitter * verticalSplitter = new QSplitter(Qt::Vertical);

    // plot
    PlotWidget * plot = new PlotWidget(true);
    NumericalPoint firstOrderIndices_i = result_.getFirstOrderIndices()[i];
    SymmetricMatrix secondOrderIndices_i = result_.getSecondOrderIndices()[i];
    NumericalPoint totalOrderIndices_i = result_.getTotalOrderIndices()[i];
    plot->plotSensitivityIndices(firstOrderIndices_i, totalOrderIndices_i, inputNames);
    plot->setAxisTitle(QwtPlot::xBottom, tr("Inputs"));

    verticalSplitter->addWidget(plot);
    verticalSplitter->setStretchFactor(0, 3);
    listPlotWidgets_.append(plot);

    // table of first order en total order indices + interactions
    QWidget * widget = new QWidget;
    QVBoxLayout * vbox = new QVBoxLayout(widget);
    QString warningMessage;

    // table of indices
    QTableWidget * table = new QTableWidget(inputNames.getSize(), 3, this);
    table->setHorizontalHeaderLabels(QStringList() << tr("Input") << tr("First order index") << tr("Total order index"));
    table->verticalHeader()->hide();
    table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    // fill table
    for (UnsignedInteger j=0; j<inputNames.getSize(); ++j)
    {
      QTableWidgetItem * item = new QTableWidgetItem(inputNames[j].c_str());
      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
      table->setItem(j, 0, item);

      item = new QTableWidgetItem(QString::number(firstOrderIndices_i[j], 'g', 4));
      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
      table->setItem(j, 1, item);

      item = new QTableWidgetItem(QString::number(totalOrderIndices_i[j], 'g', 4));
      if (totalOrderIndices_i[j] < firstOrderIndices_i[j])
      {
        item->setToolTip(tr("Warning: The total order index is inferior to the first order index."));
        item->setData(Qt::DecorationRole, QIcon(":/images/task-attention.png"));
      }
      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
      table->setItem(j, 2, item);

      // compute interactions for the ith output
      for (UnsignedInteger k=0; k<inputNames.getSize(); ++k)
        if (j != k) 
          interactionsValues[i] += secondOrderIndices_i(j, k);
    }
    table->setSortingEnabled(true);
    connect(table->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(updateIndicesPlot(int, Qt::SortOrder)));

    vbox->addWidget(table);

    // Interactions
    QHBoxLayout * hbox = new QHBoxLayout;
    QLabel * interactionsLabel = new QLabel(tr("Interactions"));
    interactionsLabel->setStyleSheet("font: bold;");
    hbox->addWidget(interactionsLabel);
    interactionsLabel = new QLabel(QString::number(interactionsValues[i], 'g', 4));
    hbox->addWidget(interactionsLabel);
    if (interactionsValues[i] < 0 || interactionsValues[i] > 1)
    {
      interactionsLabel = new QLabel;
      interactionsLabel->setPixmap(QPixmap(":/images/task-attention.png"));
      interactionsLabel->setToolTip(tr("Warning: The sum of the second order indices is not in the range [0, 1]."));
      hbox->addWidget(interactionsLabel);
    }
    hbox->addStretch();
    vbox->addLayout(hbox);

    verticalSplitter->addWidget(widget);
    verticalSplitter->setStretchFactor(1, 1);
    frameLayout_->addWidget(verticalSplitter);
  }

  plotsConfigurationWidget_ = new GraphConfigurationWidget(listPlotWidgets_, QStringList(), outputNames, GraphConfigurationWidget::SensitivityIndices);
  connect(plotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), frameLayout_, SLOT(setCurrentIndex(int)));

  scrollArea->setWidget(frame);
  tabWidget->addTab(scrollArea, tr("Result"));
  //
  setWidget(tabWidget);
}


void SobolResultWindow::updateIndicesPlot(int section, Qt::SortOrder order)
{
  int indexOutput = frameLayout_->currentIndex();
  NumericalPoint currentFirstOrderIndices(result_.getInputNames().getSize());
  NumericalPoint currentTotalOrderIndices(result_.getInputNames().getSize());
  Description sortedInputNames(result_.getInputNames().getSize());

  switch (section)
  {
    case 1:
    {
      int index = 0;
      if (order == Qt::DescendingOrder)
      {
        for (std::map<double,int>::reverse_iterator it=totalOrderIndices_[indexOutput].rbegin(); it!=totalOrderIndices_[indexOutput].rend(); ++it, index++)
        {
          currentFirstOrderIndices[index] = result_.getFirstOrderIndices()[indexOutput][it->second];
          currentTotalOrderIndices[index] = it->first;
          sortedInputNames[index] = result_.getInputNames()[it->second];
        }
      }
      else
      {
        for (std::map<double,int>::iterator it=totalOrderIndices_[indexOutput].begin(); it!=totalOrderIndices_[indexOutput].end(); ++it, index++)
        {
          currentFirstOrderIndices[index] = result_.getFirstOrderIndices()[indexOutput][it->second];
          currentTotalOrderIndices[index] = it->first;
          sortedInputNames[index] = result_.getInputNames()[it->second];
        }
      }
      break;
    }
    case 2:
    {
      int index = 0;
      if (order == Qt::DescendingOrder)
      {
        for (std::map<double,int>::reverse_iterator it=firstOrderIndices_[indexOutput].rbegin(); it!=firstOrderIndices_[indexOutput].rend(); ++it, index++)
        {
          currentFirstOrderIndices[index] = it->first;
          currentTotalOrderIndices[index] = result_.getTotalOrderIndices()[indexOutput][it->second];
          sortedInputNames[index] = result_.getInputNames()[it->second];
        }
      }
      else
      {
        for (std::map<double,int>::iterator it=firstOrderIndices_[indexOutput].begin(); it!=firstOrderIndices_[indexOutput].end(); ++it, index++)
        {
          currentFirstOrderIndices[index] = it->first;
          currentTotalOrderIndices[index] = result_.getTotalOrderIndices()[indexOutput][it->second];
          sortedInputNames[index] = result_.getInputNames()[it->second];
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