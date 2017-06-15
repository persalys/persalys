//                                               -*- C++ -*-
/**
 *  @brief QWidget for sensitivity analysis result
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#ifndef OTGUI_SENSITIVITYRESULTWIDGET_HXX
#define OTGUI_SENSITIVITYRESULTWIDGET_HXX

#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/CopyableTableView.hxx"
#include "otgui/CustomStandardItemModel.hxx"

#include <openturns/Point.hxx>
#include <openturns/Exception.hxx>

#include <QHeaderView>
#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>

namespace OTGUI {

class OTGUI_API SensitivityResultWidget : public QSplitter
{
  Q_OBJECT

public:
  enum Type {Sobol, SRC};

  SensitivityResultWidget(const int indexWidget,
                          const OT::Point& firstIndices,
                          const OT::Point& totalIndices,
                          const OT::Description& inputNames,
                          const OT::String& outputName,
                          const Type type
                         )
    : QSplitter(Qt::Vertical)
    , plot_(0)
    , graphConfigWidget_(0)
    , indicesTableModel_(0)
    , indexWidget_(indexWidget)
  {
    // set info
    QString graphTitle;
    QString defaultFileName;
    QStringList tableTitles;
    if (type == SensitivityResultWidget::Sobol)
    {
      graphTitle = tr("Sobol sensitivity indices:");
      defaultFileName = "sensitivitySobol";
      tableTitles << tr("Input") << tr("First order index") << tr("Total index");
    }
    else if (type == SensitivityResultWidget::SRC)
    {
      graphTitle = tr("SRC sensitivity indices:");
      defaultFileName = "sensitivitySRC";
      tableTitles << tr("Input") << tr("Index");
    }
    // plot
    plot_ = new PlotWidget(defaultFileName, true);
    plot_->plotSensitivityIndices(firstIndices, totalIndices, inputNames);
    plot_->setTitle(graphTitle + " " + QString::fromUtf8(outputName.c_str()));

    QVector< PlotWidget* > listPlotWidget;
    listPlotWidget.append(plot_);
    graphConfigWidget_ = new GraphConfigurationWidget(listPlotWidget, QStringList(), QStringList(), GraphConfigurationWidget::SensitivityIndices);

    addWidget(plot_);
    setStretchFactor(0, 2);

    QWidget * subWidget = new QWidget;
    QVBoxLayout * subWidgetLayout = new QVBoxLayout(subWidget);

    // table of first order (+ total order indices + interactions)
    CopyableTableView * tableView = new CopyableTableView;
    tableView->verticalHeader()->hide();
#if QT_VERSION >= 0x050000
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    tableView->setSortingEnabled(true);

    indicesTableModel_ = new CustomStandardItemModel(inputNames.getSize(), 0, tableView);
    indicesTableModel_->setHorizontalHeaderLabels(tableTitles);

    // fill table
    OT::Scalar interactionsValue = 0.;
    for (OT::UnsignedInteger j=0; j<inputNames.getSize(); ++j)
    {
      indicesTableModel_->setNotEditableItem(j, 0, QString::fromUtf8(inputNames[j].c_str()));
      indicesTableModel_->setNotEditableItem(j, 1, firstIndices[j]);

      if (totalIndices.getSize())
      {
        indicesTableModel_->setNotEditableItem(j, 2, totalIndices[j]);

        if (totalIndices[j] < firstIndices[j])
        {
          indicesTableModel_->setData(indicesTableModel_->index(j, 2), tr("Warning: The total index is inferior to the first order index."), Qt::ToolTipRole);
          indicesTableModel_->setData(indicesTableModel_->index(j, 2), QIcon(":/images/task-attention.png"), Qt::DecorationRole);
        }

        // compute interactions for the ith output
        interactionsValue += (totalIndices[j] - firstIndices[j]);
      }
    }
    tableView->setModel(indicesTableModel_);
    subWidgetLayout->addWidget(tableView);
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

    addWidget(subWidget);
    setStretchFactor(1, 1);
  }

  virtual ~SensitivityResultWidget()
  {
    delete graphConfigWidget_;
    graphConfigWidget_ = 0;
  }


  PlotWidget * getPlotWidget()
  {
    return plot_;
  }


public slots:
  void showHideGraphConfigurationWidget(int index)
  {
    if (index == indexWidget_ && isVisible())
      emit graphWindowActivated(graphConfigWidget_);
  }


  void updateIndicesPlot(int, Qt::SortOrder)
  {
    if (!indicesTableModel_)
      throw OT::InternalException(HERE) << "SensitivityResultWidget::updateIndicesPlot: not defined table\n";

    const OT::UnsignedInteger inputDimension = indicesTableModel_->rowCount();
    OT::Point currentFirstOrderIndices(inputDimension);
    OT::Point currentTotalIndices;
    OT::Description sortedInputNames(inputDimension);

    for (OT::UnsignedInteger i=0; i<inputDimension; ++i)
    {
      sortedInputNames[i] = indicesTableModel_->data(indicesTableModel_->index(i, 0)).toString().toStdString();
      currentFirstOrderIndices[i] = indicesTableModel_->data(indicesTableModel_->index(i, 1)).toDouble();
      if (indicesTableModel_->columnCount() == 3)
        currentTotalIndices.add(indicesTableModel_->data(indicesTableModel_->index(i, 2)).toDouble());
    }

    plot_->clear();
    plot_->plotSensitivityIndices(currentFirstOrderIndices, currentTotalIndices, sortedInputNames);
  }

signals:
  void graphWindowActivated(QWidget*);

private:
  PlotWidget * plot_;
  GraphConfigurationWidget * graphConfigWidget_;
  CustomStandardItemModel * indicesTableModel_;
  int indexWidget_;
};
}
#endif