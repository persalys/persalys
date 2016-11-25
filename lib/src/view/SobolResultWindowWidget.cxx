//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of SobolAnalysis
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
#include "otgui/SobolResultWindowWidget.hxx"

#include "otgui/CopyableTableView.hxx"
#include "otgui/OTguiException.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QSplitter>
#include <QLabel>
#include <QStackedWidget>

using namespace OT;

namespace OTGUI {

SobolResultWindowWidget::SobolResultWindowWidget(const SobolResult& result, const QString& warningMessage)
  : QScrollArea()
  , result_(result)
  , currentIndex_(0)
  , warningMessage_(warningMessage)
{
  setWidgetResizable(true);
  buildInterface();
}


void SobolResultWindowWidget::buildInterface()
{
  Description inputNames = result_.getInputNames();
  QStringList outputNames;
  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
    outputNames << QString::fromUtf8(result_.getOutputNames()[i].c_str());

  QSplitter * verticalSplitter = new QSplitter(Qt::Vertical);

  QStackedWidget * plotStackedWidget = new QStackedWidget;
  QStackedWidget * tableStackedWidget = new QStackedWidget;
  QStackedWidget * interactionStackedWidget = new QStackedWidget;

  QWidget * widget = new QWidget;
  QVBoxLayout * vbox = new QVBoxLayout(widget);

  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
  {
    // plot
    PlotWidget * plot = new PlotWidget("sensitivitySobol", true);
    plot->plotSensitivityIndices(result_.getFirstOrderIndices()[i], result_.getTotalIndices()[i], inputNames);
    plot->setTitle(tr("Sobol sensitivity indices:") + " " + outputNames[i]);
    listPlotWidgets_.append(plot);
    plotStackedWidget->addWidget(plot);

    // table of first order en total order indices + interactions
    CopyableTableView * tableView = new CopyableTableView;
    tableView->verticalHeader()->hide();
#if QT_VERSION >= 0x050000
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    tableView->setSortingEnabled(true);

    CustomStandardItemModel * tableModel = new CustomStandardItemModel(inputNames.getSize(), 3, tableView);
    tableModel->setHorizontalHeaderLabels(QStringList() << tr("Input")
                                                        << tr("First order index")
                                                        << tr("Total index"));

    // fill table
    NumericalScalar interactionsValue(0.);
    for (UnsignedInteger j=0; j<inputNames.getSize(); ++j)
    {
      tableModel->setNotEditableItem(j, 0, QString::fromUtf8(inputNames[j].c_str()));
      tableModel->setNotEditableItem(j, 1, result_.getFirstOrderIndices()[i][j]);
      tableModel->setNotEditableItem(j, 2, result_.getTotalIndices()[i][j]);

      if (result_.getTotalIndices()[i][j] < result_.getFirstOrderIndices()[i][j])
      {
        tableModel->setData(tableModel->index(j, 2), tr("Warning: The total index is inferior to the first order index."), Qt::ToolTipRole);
        tableModel->setData(tableModel->index(j, 2), QIcon(":/images/task-attention.png"), Qt::DecorationRole);
      }

      // compute interactions for the ith output
      interactionsValue += (result_.getTotalIndices()[i][j]-result_.getFirstOrderIndices()[i][j]);
    }
    tableView->setModel(tableModel);
    connect(tableView->horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(updateIndicesPlot(int, Qt::SortOrder)));
    listTableModels_.append(tableModel);
    tableStackedWidget->addWidget(tableView);

    // Interactions widgets
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
    interactionStackedWidget->addWidget(interactionWidget);
  }

  vbox->addWidget(tableStackedWidget);
  vbox->addWidget(interactionStackedWidget);

  // add a warning (if the model has not an independent copula when doing a SensitivityAnalysis)
  if (!warningMessage_.isEmpty())
  {
    QLabel * warningLabel = new QLabel(QString("%1%2%3").arg("<font color=red>").arg(warningMessage_).arg("</font>"));
    vbox->addWidget(warningLabel);
  }

  connect(this, SIGNAL(currentPlotChanged(int)), plotStackedWidget, SLOT(setCurrentIndex(int)));
  connect(this, SIGNAL(currentPlotChanged(int)), tableStackedWidget, SLOT(setCurrentIndex(int)));
  connect(this, SIGNAL(currentPlotChanged(int)), interactionStackedWidget, SLOT(setCurrentIndex(int)));

  verticalSplitter->addWidget(plotStackedWidget);
  verticalSplitter->setStretchFactor(0, 3);

  verticalSplitter->addWidget(widget);
  verticalSplitter->setStretchFactor(1, 1);

  setWidget(verticalSplitter);
}


QVector< PlotWidget* > SobolResultWindowWidget::getPlotWidgets()
{
  return listPlotWidgets_;
}


void SobolResultWindowWidget::updateIndicesPlot(int section, Qt::SortOrder order)
{
  const UnsignedInteger inputDimension = result_.getInputNames().getSize();
  NumericalPoint currentFirstOrderIndices(inputDimension);
  NumericalPoint currentTotalIndices(inputDimension);
  Description sortedInputNames(inputDimension);

  CustomStandardItemModel * model = listTableModels_[currentIndex_];
  if (!model)
    throw InternalException(HERE) << "SobolResultWindowWidget::updateIndicesPlot: can not update indices plot\n";

  for (UnsignedInteger i=0; i<inputDimension; ++i)
  {
    sortedInputNames[i] = model->data(model->index(i, 0)).toString().toStdString();
    currentFirstOrderIndices[i] = model->data(model->index(i, 1)).toDouble();
    currentTotalIndices[i] = model->data(model->index(i, 2)).toDouble();
  }

  listPlotWidgets_[currentIndex_]->clear();
  listPlotWidgets_[currentIndex_]->plotSensitivityIndices(currentFirstOrderIndices, currentTotalIndices, sortedInputNames);
}


void SobolResultWindowWidget::setCurrentIndex(int index)
{
  currentIndex_ = index;
}
}