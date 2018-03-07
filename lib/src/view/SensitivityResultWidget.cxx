//                                               -*- C++ -*-
/**
 *  @brief QWidget for sensitivity analysis result
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/SensitivityResultWidget.hxx"

#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/CopyableTableView.hxx"
#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/WidgetBoundToDockWidget.hxx"

#include <openturns/Exception.hxx>

#include <QHeaderView>
#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>

using namespace OT;

namespace OTGUI
{

SensitivityResultWidget::SensitivityResultWidget(const Point& firstIndices,
    const Interval& firstIndicesIntervals,
    const Point& totalIndices,
    const Interval& totalIndicesIntervals,
    const Description& inputNames,
    const String& outputName,
    const Type type,
    QWidget * parent
                                                )
  : QWidget(parent)
  , plot_(0)
  , proxyModel_(0)
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  QSplitter * mainSplitter = new QSplitter(Qt::Vertical);

  // set info
  QString graphTitle;
  QString defaultFileName;
  QStringList tableTitles;
  if (type == SensitivityResultWidget::Sobol)
  {
    graphTitle = tr("Sobol sensitivity indices:");
    defaultFileName = tr("sensitivitySobol");
    tableTitles << tr("Input") << tr("First order index");
    if (firstIndicesIntervals.getDimension() == firstIndices.getSize())
    {
      tableTitles << tr("First order index\nconfidence interval");
    }
    tableTitles << tr("Total index");
    if (firstIndicesIntervals.getDimension() == firstIndices.getSize())
    {
      tableTitles << tr("Total index\nconfidence interval");
    }
  }
  else if (type == SensitivityResultWidget::SRC)
  {
    graphTitle = tr("SRC sensitivity indices:");
    defaultFileName = tr("sensitivitySRC");
    tableTitles << tr("Input") << tr("Index");
  }
  // plot
  WidgetBoundToDockWidget * plotWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * plotWidgetLayout = new QVBoxLayout(plotWidget);

  plot_ = new PlotWidget(defaultFileName, true);
  plot_->plotSensitivityIndices(firstIndices, totalIndices, inputNames, firstIndicesIntervals, totalIndicesIntervals);
  plot_->setTitle(graphTitle + " " + QString::fromUtf8(outputName.c_str()));
  plotWidgetLayout->addWidget(plot_);

  GraphConfigurationWidget * graphSetting = new GraphConfigurationWidget(plot_,
      QStringList(),
      QStringList(),
      GraphConfigurationWidget::SensitivityIndices,
      this);
  plotWidget->setDockWidget(graphSetting);

  mainSplitter->addWidget(plotWidget);
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
  Scalar interactionsValue = 0.;
  for (UnsignedInteger j = 0; j < inputNames.getSize(); ++j)
  {
    int col = 0;
    indicesTableModel->setNotEditableItem(j, col, QString::fromUtf8(inputNames[j].c_str()));
    indicesTableModel->setData(indicesTableModel->index(j, col), true, Qt::UserRole);

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
      if (totalIndices[j] < firstIndices[j])
      {
        indicesTableModel->setData(indicesTableModel->index(j, col), tr("Warning: The total index is lesser than the first order index."), Qt::ToolTipRole);
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

      // compute interactions for the ith output
      interactionsValue += (totalIndices[j] - firstIndices[j]);
    }
  }
  proxyModel_ = new IndicesProxyModel(tableView);
  proxyModel_->setSourceModel(indicesTableModel);
  proxyModel_->setSortRole(Qt::UserRole + 1);
  tableView->setModel(proxyModel_);
  subWidgetLayout->addWidget(tableView);

  // resize table
  tableView->resizeColumnsToContents();
#if QT_VERSION >= 0x050000
  tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
  tableView->horizontalHeader()->setSectionResizeMode(proxyModel_->columnCount() - 1, QHeaderView::Stretch);
#else
  tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  tableView->horizontalHeader()->setResizeMode(proxyModel_->columnCount() - 1, QHeaderView::Stretch);
#endif

  // if the table is sorted : we sort the points in the graph too
  connect(tableView->horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(updateIndicesPlot(int, Qt::SortOrder)));

  // Interactions widgets
  if (totalIndices.getSize())
  {
    QWidget * interactionWidget = new QWidget;
    QHBoxLayout * hbox = new QHBoxLayout(interactionWidget);
    QLabel * interactionsLabel = new QLabel(tr("Interactions"));
    interactionsLabel->setStyleSheet("font: bold;");
    hbox->addWidget(interactionsLabel);
    interactionsLabel = new QLabel(QString::number(interactionsValue, 'g', 4));
    hbox->addWidget(interactionsLabel);
    if (interactionsValue < 0 || interactionsValue > 1)
    {
      interactionsLabel = new QLabel;
      interactionsLabel->setPixmap(QPixmap(":/images/task-attention.png"));
      interactionsLabel->setToolTip(tr("Warning: The sum of the second order indices is not in the range [0, 1]."));
      hbox->addWidget(interactionsLabel);
    }
    hbox->addStretch();
    subWidgetLayout->addWidget(interactionWidget);
  }

  mainSplitter->addWidget(subWidget);
  mainSplitter->setStretchFactor(1, 1);
  mainLayout->addWidget(mainSplitter);
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

  plot_->clear();
  const Interval foInterval(sortedFOIntervalLowerBounds, sortedFOIntervalUpperBounds);
  const Interval toInterval(sortedTOIntervalLowerBounds, sortedTOIntervalUpperBounds);
  plot_->plotSensitivityIndices(sortedFirstOrderIndices, sortedTotalIndices, sortedInputNames, foInterval, toInterval);
}
}
