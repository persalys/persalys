//                                               -*- C++ -*-
/**
 *  @brief QWidget for meta model result
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
#ifndef OTGUI_METAMODELVALIDATIONWIDGET_HXX
#define OTGUI_METAMODELVALIDATIONWIDGET_HXX

#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/ResizableTableViewWithoutScrollBar.hxx"
#include "otgui/CustomStandardItemModel.hxx"

#include <openturns/NumericalSample.hxx>

#include <QHeaderView>
#include <QVBoxLayout>

namespace OTGUI {

class OTGUI_API MetaModelValidationWidget : public QWidget
{
  Q_OBJECT

public:
  MetaModelValidationWidget(const OT::NumericalSample& metaModelSample,
                            const OT::NumericalSample& outputSample,
                            const double error,
                            const double value,
                            const bool isR2,
                            const int indexWidget
                           )
    : QWidget()
    , graphConfigWidget_(0)
    , indexWidget_(indexWidget)
  {
    QVBoxLayout * widgetLayout = new QVBoxLayout(this);

    // validation table view
    ResizableTableViewWithoutScrollBar * validationTableView = new ResizableTableViewWithoutScrollBar;
    validationTableView->horizontalHeader()->hide();
    validationTableView->verticalHeader()->hide();
    CustomStandardItemModel * validationTable = new CustomStandardItemModel(2, 2, this);
    validationTableView->setModel(validationTable);
    // - vertical header
    validationTable->setNotEditableHeaderItem(0, 0, tr("Residual"));
    validationTable->setNotEditableHeaderItem(1, 0, isR2? tr("R2") : tr("Q2"));

    // - residual/q2 values
    validationTable->setNotEditableItem(0, 1, error);
    validationTable->setNotEditableItem(1, 1, value);

    validationTableView->resizeToContents();
    widgetLayout->addWidget(validationTableView);

    // plot widget
    PlotWidget * plot = new PlotWidget;
    plot->plotScatter(outputSample, metaModelSample);
    OT::NumericalSample lineSample(outputSample);
    lineSample.stack(lineSample);
    plot->plotCurve(lineSample, QPen(Qt::black, 1));
    plot->setTitle(tr("Metamodel:") + " " + QString::fromUtf8(outputSample.getDescription()[0].c_str()));
    plot->setAxisTitle(QwtPlot::xBottom, tr("Physical model"));
    plot->setAxisTitle(QwtPlot::yLeft, tr("Metamodel"));
    widgetLayout->addWidget(plot);

    // GraphConfigurationWidget
    QVector<PlotWidget*> listPlot;
    listPlot.append(plot);
    graphConfigWidget_ = new GraphConfigurationWidget(listPlot, QStringList(), QStringList(), GraphConfigurationWidget::NoType);
  }

  virtual ~MetaModelValidationWidget()
  {
    delete graphConfigWidget_;
    graphConfigWidget_ = 0;
  }

public slots:
  void showHideGraphConfigurationWidget(int index)
  {
    if (index == indexWidget_ && isVisible())
      emit graphWindowActivated(graphConfigWidget_);
  }

signals:
  void graphWindowActivated(QWidget*);

private:
  GraphConfigurationWidget * graphConfigWidget_;
  int indexWidget_;
};
}
#endif