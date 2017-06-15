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
#ifndef OTGUI_METAMODELVALIDATIONWIDGET_HXX
#define OTGUI_METAMODELVALIDATIONWIDGET_HXX

#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/ParametersTableView.hxx"

#include <openturns/Sample.hxx>

#include <QVBoxLayout>

namespace OTGUI {

class OTGUI_API MetaModelValidationWidget : public QWidget
{
  Q_OBJECT

public:
  MetaModelValidationWidget(const int indexWidget,
                            const OT::Sample& metaModelSample,
                            const OT::Sample& outputSample,
                            const double error=-1.0,
                            const double value=-1.0,
                            const QString measure=""
                           )
    : QWidget()
    , graphConfigWidget_(0)
    , indexWidget_(indexWidget)
  {
    QVBoxLayout * widgetLayout = new QVBoxLayout(this);

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

      widgetLayout->addWidget(table);
    }

    // plot widget
    PlotWidget * plot = new PlotWidget;
    plot->plotScatter(outputSample, metaModelSample);
    OT::Sample lineSample(outputSample);
    lineSample.stack(lineSample);
    plot->plotCurve(lineSample, QPen(Qt::black, 1));
    plot->setTitle(tr("Metamodel:") + " " + QString::fromUtf8(outputSample.getDescription()[0].c_str()));
    plot->setAxisTitle(QwtPlot::xBottom, tr("Physical model"));
    plot->setAxisTitle(QwtPlot::yLeft, tr("Metamodel"));
    widgetLayout->addWidget(plot);

    // GraphConfigurationWidget
    QVector<PlotWidget*> listPlot;
    listPlot.append(plot);
    graphConfigWidget_ = new GraphConfigurationWidget(listPlot);
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