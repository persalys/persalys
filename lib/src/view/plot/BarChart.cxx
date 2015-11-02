#include "BarChart.hxx"

#include <qwt_plot_multi_barchart.h>
#include <qwt_column_symbol.h>
#include <qwt_plot_layout.h>

using namespace OT;

namespace OTGUI {

BarChart::BarChart(QWidget* parent)
: OtguiPlotWidget(parent)
{
}


void BarChart::plotSensitivityIndices(const NumericalPoint firstOrder, const NumericalPoint totalOrder, const Description inputNames)
{
  setTitle("Sensitivity indices");

  setAxisTitle(QwtPlot::yLeft, "Sensitivity indice");
  setAxisTitle(QwtPlot::xBottom, "Inputs");

  QwtPlotMultiBarChart * barChartItem = new QwtPlotMultiBarChart;
  barChartItem->setLayoutPolicy(QwtPlotMultiBarChart::AutoAdjustSamples);
  barChartItem->setSpacing(20);
  barChartItem->setMargin(3);

  barChartItem->attach(this);

//   insertLegend(new QwtLegend());
//   setAutoReplot(true);

  // populate bar chart
  static const char *colors[] = {"DarkOrchid", "SteelBlue"};

  const int numSamples = inputNames.getSize();
  const int numBars = sizeof(colors) / sizeof(colors[0]);

  QList<QwtText> titles;
  titles += QwtText("First order indice");
  titles += QwtText("Total order indice");

  barChartItem->setBarTitles(titles);
  barChartItem->setLegendIconSize(QSize(10, 14));

  for (int i=0; i<numBars; i++)
  {
    QwtColumnSymbol *symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
    symbol->setLineWidth(2);
    symbol->setFrameStyle(QwtColumnSymbol::Raised);
    symbol->setPalette(QPalette(colors[i]));

    barChartItem->setSymbol(i, symbol);
  }
  
  QVector< QVector<double> > series;
  for (int i=0; i<numSamples; i++)
  {
    QVector<double> values;
    values += firstOrder[i];
    values += totalOrder[i];

    series += values;
  }

  barChartItem->setSamples(series);

  // scales

  setAxisScaleDraw(QwtPlot::xBottom, new BarChartHorizontalScaleDraw(inputNames));
  setAxisScale(QwtPlot::xBottom, 0, barChartItem->dataSize()-0.5, 1.0);
  setAxisAutoScale(QwtPlot::yLeft);

  QwtScaleDraw *scaleDraw2 = axisScaleDraw(QwtPlot::yLeft);
  scaleDraw2->enableComponent(QwtScaleDraw::Backbone, true);
  scaleDraw2->enableComponent(QwtScaleDraw::Ticks, true);

  plotLayout()->setAlignCanvasToScale(QwtPlot::xBottom, true);
  plotLayout()->setAlignCanvasToScale(QwtPlot::yLeft, false);

  plotLayout()->setCanvasMargin(0);
  updateCanvasMargins();

  replot();
}


}