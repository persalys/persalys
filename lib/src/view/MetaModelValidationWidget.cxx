//                                               -*- C++ -*-
/**
 *  @brief QWidget for meta model result
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#include "persalys/MetaModelValidationWidget.hxx"

#include "persalys/GraphConfigurationWidget.hxx"
#include "persalys/ParametersTableView.hxx"
#include "persalys/WidgetBoundToDockWidget.hxx"
#include "persalys/TranslationManager.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace PERSALYS
{

MetaModelValidationWidget::MetaModelValidationWidget(const MetaModelValidationResult& result,
    const Sample& outSample,
    const UnsignedInteger indexOutput,
    const QString measure,
    QWidget * parent
                                                    )
  : QScrollArea(parent)
{
  setWidgetResizable(true);

  WidgetBoundToDockWidget * mainWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * mainWidgetLayout = new QVBoxLayout(mainWidget);

  if (!measure.isEmpty())
  {
    // validation table view
    QStringList namesList;
    QStringList valuesList;

    namesList << tr("Number of points");
    valuesList << QString::number(outSample.getSize());

    for (UnsignedInteger i = 0; i < result.getParameters().getSize(); ++i)
    {
      namesList << TranslationManager::GetTranslatedParameterName(result.getParameters().getDescription()[i]);
      valuesList << QString::number(result.getParameters()[i]);
    }
    if (result.getResiduals().getSize() == outSample.getDimension())
    {
      namesList << tr("Residual");
      valuesList << QString::number(result.getResiduals()[indexOutput]);
    }
    Q_ASSERT(indexOutput < result.getQ2().getSize());
    namesList << measure;
    valuesList << QString::number(result.getQ2()[indexOutput]);

    ParametersTableView * table = new ParametersTableView(namesList, valuesList, true, true);

    mainWidgetLayout->addWidget(table, 0, Qt::AlignTop);
  }

  // plot widget
  if (result.getMetaModelOutputSample().getSize())
  {
    Q_ASSERT(indexOutput < result.getMetaModelOutputSample().getDimension());
    Q_ASSERT(indexOutput < outSample.getDimension());

    const Sample outputSample(outSample.getMarginal(indexOutput));

    PlotWidget * plot = new PlotWidget(tr("metaModel"));
    plot->plotScatter(outputSample, result.getMetaModelOutputSample().getMarginal(indexOutput));
    Sample lineSample(outputSample);
    lineSample.stack(lineSample);
    plot->plotCurve(lineSample, QPen(Qt::black, 1));

    if (result.getName() == "K-Fold")
      plot->setTitle(tr("Curve fitting of K-Fold cross-validation"));
    else if (result.getName() == "Test sample")
      plot->setTitle(tr("Curve fitting of the test sample"));
    else if (result.getName() == "Analytical")
      plot->setTitle(tr("Curve fitting of the analytical values"));
    else
      plot->setTitle(tr("Curve fitting of %1").arg(QString::fromUtf8(outputSample.getDescription()[0].c_str())));

    plot->setAxisTitle(QwtPlot::xBottom, tr("Real output value"));
    plot->setAxisTitle(QwtPlot::yLeft, tr("Prediction"));
    mainWidgetLayout->addWidget(plot);

    // SimpleGraphSetting
    SimpleGraphSetting * graphSettingWidget = new SimpleGraphSetting(plot, this);
    mainWidget->setDockWidget(graphSettingWidget);
  }
  setWidget(mainWidget);
}
}
