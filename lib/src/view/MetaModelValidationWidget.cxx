//                                               -*- C++ -*-
/**
 *  @brief QWidget for meta model result
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
#include "otgui/MetaModelValidationWidget.hxx"

#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/ParametersTableView.hxx"
#include "otgui/WidgetBoundToDockWidget.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI {

MetaModelValidationWidget::MetaModelValidationWidget(const Sample& metaModelSample,
                                                     const Sample& outputSample,
                                                     const double error,
                                                     const double value,
                                                     const QString measure,
                                                     QWidget * parent
                                                    )
  : QWidget(parent)
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  WidgetBoundToDockWidget * plotWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * plotWidgetLayout = new QVBoxLayout(plotWidget);

  if (!measure.isEmpty())
  {
    // validation table view
    QStringList namesList;
    namesList << tr("Residual");
    namesList << measure;

    QStringList valuesList;
    valuesList << QString::number(error);
    valuesList << QString::number(value);

    ParametersTableView * table = new ParametersTableView(namesList, valuesList, true, true);

    plotWidgetLayout->addWidget(table);
  }

  // plot widget
  PlotWidget * plot = new PlotWidget(tr("metaModel"));
  plot->plotScatter(outputSample, metaModelSample);
  Sample lineSample(outputSample);
  lineSample.stack(lineSample);
  plot->plotCurve(lineSample, QPen(Qt::black, 1));
  plot->setTitle(tr("Metamodel:") + " " + QString::fromUtf8(outputSample.getDescription()[0].c_str()));
  plot->setAxisTitle(QwtPlot::xBottom, tr("Physical model"));
  plot->setAxisTitle(QwtPlot::yLeft, tr("Metamodel"));
  plotWidgetLayout->addWidget(plot);

  // GraphConfigurationWidget
  QVector<PlotWidget*> listPlot;
  listPlot.append(plot);
  GraphConfigurationWidget * graphSettingWidget = new GraphConfigurationWidget(listPlot,
                                                                               QStringList(),
                                                                               QStringList(),
                                                                               GraphConfigurationWidget::NoType,
                                                                               this);
  plotWidget->setDockWidget(graphSettingWidget);
  widgetLayout->addWidget(plotWidget);
}
}
