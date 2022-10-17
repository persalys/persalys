//                                               -*- C++ -*-
/**
 *  @brief QWidget for sensitivity analysis result
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "persalys/SensitivityResultWidget.hxx"

#include "persalys/SensitivityIndicesPlot.hxx"
#include "persalys/GraphConfigurationWidget.hxx"
#include "persalys/CopyableTableView.hxx"
#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/WidgetBoundToDockWidget.hxx"

#include <openturns/Exception.hxx>

#include <QHeaderView>
#include <QVBoxLayout>
#include <QSplitter>
#include <QScrollArea>

using namespace OT;

namespace PERSALYS
{

SensitivityResultWidget::SensitivityResultWidget(const Point& firstIndices,
    const Interval& firstIndicesIntervals,
    const Point& totalIndices,
    const Interval& totalIndicesIntervals,
    const Description& inputNames,
    const String& outputName,
    const Type type,
    QWidget * parent)
  : QWidget(parent)
  , plot_(0)
  , proxyModel_(0)
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  QSplitter * mainSplitter = new QSplitter(Qt::Vertical);

  // set info
  QString graphTitle;
  QString defaultFileName;
  QStringList tableTitles(tr("Input"));
  QStringList legendNames;
  if (type == SensitivityResultWidget::Sobol)
  {
    graphTitle = tr("Sobol sensitivity indices:");
    defaultFileName = tr("sensitivitySobol");
    legendNames << tr("First order index");
    tableTitles << tr("First order\nindex");
    if (firstIndicesIntervals.getDimension() == firstIndices.getSize())
    {
      tableTitles << tr("First order index\nconfidence interval");
    }
    tableTitles << tr("Total index");
    legendNames << tr("Total index");
    if (totalIndicesIntervals.getDimension() == totalIndices.getSize())
    {
      tableTitles << tr("Total index\nconfidence interval");
    }
  }
  else if (type == SensitivityResultWidget::SRC)
  {
    graphTitle = tr("SRC sensitivity indices:");
    defaultFileName = tr("sensitivitySRC");
    legendNames << tr("Squared SRC") << tr("SRC");
    tableTitles << tr("Squared SRC");
    if (firstIndicesIntervals.getDimension() == firstIndices.getSize())
      tableTitles << tr("Squared SRC\nconfidence interval");
    if (totalIndices.getSize())
    {
      tableTitles << tr("SRC");
      if (totalIndicesIntervals.getDimension() == totalIndices.getSize())
        tableTitles << tr("SRC\nconfidence interval");
    }
  }
  // plot
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  plot_ = new SensitivityIndicesPlot(defaultFileName, firstIndices, totalIndices, firstIndicesIntervals, totalIndicesIntervals, inputNames, legendNames);
  plot_->setTitle(graphTitle + " " + QString::fromUtf8(outputName.c_str()));

  SensitivityIndicesGraphSetting * graphSetting = new SensitivityIndicesGraphSetting(plot_, this);
  scrollArea->setWidget(new WidgetBoundToDockWidget(plot_, graphSetting, this));

  mainSplitter->addWidget(scrollArea);
  mainSplitter->setStretchFactor(0, 2);

  QWidget * subWidget = new QWidget;
  QVBoxLayout * subWidgetLayout = new QVBoxLayout(subWidget);

  // table of first order (+ total order indices + interactions)
  CopyableTableView * tableView = new CopyableTableView;
  tableView->verticalHeader()->hide();
  tableView->setSortingEnabled(true);

  CustomStandardItemModel * indicesTableModel = new CustomStandardItemModel(inputNames.getSize(), 0, tableView);
  indicesTableModel->setHorizontalHeaderLabels(tableTitles);

  // fill table
  Scalar sumFirstIndices = 0.;
  for (UnsignedInteger j = 0; j < inputNames.getSize(); ++j)
  {
    int col = 0;
    indicesTableModel->setNotEditableItem(j, col, QString::fromUtf8(inputNames[j].c_str()));
    indicesTableModel->setData(indicesTableModel->index(j, col), true, Qt::UserRole);
    indicesTableModel->setData(indicesTableModel->index(j, col), QString::fromUtf8(inputNames[j].c_str()), Qt::UserRole + 1);

    // first order index
    indicesTableModel->setNotEditableItem(j, ++col, firstIndices[j]);
    indicesTableModel->setData(indicesTableModel->index(j, col), true, Qt::UserRole);
    indicesTableModel->setData(indicesTableModel->index(j, col), firstIndices[j], Qt::UserRole + 1);

    // confidence interval
    if (firstIndicesIntervals.getDimension() == inputNames.getSize())
    {
      const Interval foInterval(firstIndicesIntervals.getLowerBound()[j], firstIndicesIntervals.getUpperBound()[j]);
      indicesTableModel->setNotEditableItem(j, ++col, foInterval.__str__().c_str());
      indicesTableModel->setData(indicesTableModel->index(j, col), false, Qt::UserRole);
      indicesTableModel->setData(indicesTableModel->index(j, col), foInterval.getLowerBound()[0], Qt::UserRole + 2);
      indicesTableModel->setData(indicesTableModel->index(j, col), foInterval.getUpperBound()[0], Qt::UserRole + 3);
    }

    if (totalIndices.getSize())
    {
      // total index
      indicesTableModel->setNotEditableItem(j, ++col, totalIndices[j]);
      indicesTableModel->setData(indicesTableModel->index(j, col), true, Qt::UserRole);
      indicesTableModel->setData(indicesTableModel->index(j, col), totalIndices[j], Qt::UserRole + 1);

      if (type != SensitivityResultWidget::SRC && totalIndices[j] < firstIndices[j])
      {
        indicesTableModel->setData(indicesTableModel->index(j, col), tr("Warning: The total index is less than the first order index."), Qt::ToolTipRole);
        indicesTableModel->setData(indicesTableModel->index(j, col), QIcon(":/images/task-attention.png"), Qt::DecorationRole);
      }

      // confidence interval
      if (totalIndicesIntervals.getDimension() == inputNames.getSize())
      {
        const Interval toInterval(totalIndicesIntervals.getLowerBound()[j], totalIndicesIntervals.getUpperBound()[j]);
        indicesTableModel->setNotEditableItem(j, ++col, toInterval.__str__().c_str());
        indicesTableModel->setData(indicesTableModel->index(j, col), false, Qt::UserRole);
        indicesTableModel->setData(indicesTableModel->index(j, col), toInterval.getLowerBound()[0], Qt::UserRole + 2);
        indicesTableModel->setData(indicesTableModel->index(j, col), toInterval.getUpperBound()[0], Qt::UserRole + 3);
      }

      // compute sum of first indices to be able to compute interaction
      sumFirstIndices += firstIndices[j];
    }
  }
  proxyModel_ = new IndicesProxyModel(tableView);
  proxyModel_->setSourceModel(indicesTableModel);
  proxyModel_->setSortRole(Qt::UserRole + 1);
  tableView->setModel(proxyModel_);
  subWidgetLayout->addWidget(tableView);

  // resize table
  tableView->resizeColumnsToContents();
  tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
  tableView->horizontalHeader()->setSectionResizeMode(proxyModel_->columnCount() - 1, QHeaderView::Stretch);

  // if the table is sorted : we sort the points in the graph too
  connect(tableView->horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(updateIndicesPlot(int, Qt::SortOrder)));

  // Interactions widgets
  if (totalIndices.getSize() && type != SensitivityResultWidget::SRC)
  {
    QWidget * interactionWidget = new QWidget;
    QHBoxLayout * hbox = new QHBoxLayout(interactionWidget);
    QLabel * interactionsLabel = new QLabel(tr("Interactions"));
    interactionsLabel->setStyleSheet("QLabel {font: bold;}");
    hbox->addWidget(interactionsLabel);
    interactionsLabel = new QLabel(QString::number(1. - sumFirstIndices, 'g', 4));
    hbox->addWidget(interactionsLabel);
    hbox->addStretch();
    hbox->setContentsMargins(0, 0, 0, 0);
    subWidgetLayout->addWidget(interactionWidget);
  }
  subWidgetLayout->setContentsMargins(0, 0, 0, 0);

  mainSplitter->addWidget(subWidget);
  mainSplitter->setStretchFactor(1, 1);
  mainLayout->addWidget(mainSplitter);
  mainLayout->setContentsMargins(0, 0, 0, 0);
}


void SensitivityResultWidget::updateIndicesPlot(int, Qt::SortOrder)
{
  if (!proxyModel_)
    throw InternalException(HERE) << "SensitivityResultWidget::updateIndicesPlot: not defined table\n";

  const UnsignedInteger nbInputs = proxyModel_->rowCount();
  Point sortedFirstOrderIndices(nbInputs);
  Point sortedTotalIndices;
  Description sortedInputNames(nbInputs);
  Point sortedFOIntervalLowerBounds;
  Point sortedFOIntervalUpperBounds;
  Point sortedTOIntervalLowerBounds;
  Point sortedTOIntervalUpperBounds;

  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    sortedInputNames[i] = proxyModel_->data(proxyModel_->index(i, 0)).toString().toStdString();
    sortedFirstOrderIndices[i] = proxyModel_->data(proxyModel_->index(i, 1), Qt::UserRole + 1).toDouble();
    // Sobol result
    if (proxyModel_->columnCount() == 5)
    {
      sortedFOIntervalLowerBounds.add(proxyModel_->data(proxyModel_->index(i, 2), Qt::UserRole + 2).toDouble());
      sortedFOIntervalUpperBounds.add(proxyModel_->data(proxyModel_->index(i, 2), Qt::UserRole + 3).toDouble());
      sortedTotalIndices.add(proxyModel_->data(proxyModel_->index(i, 3), Qt::UserRole + 1).toDouble());
      sortedTOIntervalLowerBounds.add(proxyModel_->data(proxyModel_->index(i, 4), Qt::UserRole + 2).toDouble());
      sortedTOIntervalUpperBounds.add(proxyModel_->data(proxyModel_->index(i, 4), Qt::UserRole + 3).toDouble());
    }
    // Functional chaos result
    else if (proxyModel_->columnCount() == 3)
    {
      sortedTotalIndices.add(proxyModel_->data(proxyModel_->index(i, 2), Qt::UserRole + 1).toDouble());
    }
  }

  const Interval foInterval(sortedFOIntervalLowerBounds, sortedFOIntervalUpperBounds);
  const Interval toInterval(sortedTOIntervalLowerBounds, sortedTOIntervalUpperBounds);
  plot_->updatePlot(sortedFirstOrderIndices, sortedTotalIndices, foInterval, toInterval, sortedInputNames);
}
}
