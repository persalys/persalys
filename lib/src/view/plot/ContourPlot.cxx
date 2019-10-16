//                                               -*- C++ -*-
/**
 *  @brief QwtPlot for contour
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/ContourPlot.hxx"

#include "persalys/CustomScaleDraw.hxx"

#include <openturns/Contour.hxx>

#include <qwt_scale_widget.h>
#include <qwt_color_map.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_layout.h>

using namespace OT;

namespace PERSALYS
{

// Custom QwtMatrixRasterData
class RasterData : public QwtMatrixRasterData
{
public:
  RasterData(Contour *contour)
  : QwtMatrixRasterData()
  , dx_(contour->getX()(1, 0) - contour->getX()(0, 0))
  , dy_(contour->getY()(1, 0) - contour->getY()(0, 0))
  {
    setResampleMode(QwtMatrixRasterData::BilinearInterpolation);

    QVector<double> zValues(contour->getData().getSize());
    Sample sample(contour->getData());
    std::copy(&sample(0, 0), &sample(contour->getData().getSize()-1, 0)+1, zValues.begin());
    setValueMatrix(zValues, contour->getX().getSize());
  };


  QRectF pixelHint(const QRectF & /*area*/) const
  {
    const QwtInterval intervalX(interval(Qt::XAxis));
    const QwtInterval intervalY(interval(Qt::YAxis));
    if (intervalX.isValid() && intervalY.isValid())
      return QRectF(intervalX.minValue(), intervalY.minValue(), dx_, dy_);
    return QRectF();
  }

private:
  double dx_;
  double dy_;
};


// Custom QwtLinearColorMap
class ColorMap : public QwtLinearColorMap
{
public:
  ColorMap(const QList<double> &levels)
  : QwtLinearColorMap(Qt::darkBlue, Qt::darkRed)
  {
    const double maxLevel = levels[levels.size() - 1];
    const double minLevel = levels[0];
    const double intervalWidth = maxLevel - minLevel;

    if (levels.size() > 2) addColorStop((levels[1] - minLevel) / intervalWidth, Qt::blue);
    if (levels.size() > 3) addColorStop((levels[2] - minLevel) / intervalWidth, Qt::cyan);
    if (levels.size() > 4) addColorStop((levels[3] - minLevel) / intervalWidth, Qt::green);
    if (levels.size() > 9) addColorStop((levels[levels.size() - 5] - minLevel) / intervalWidth, Qt::yellow);
    if (levels.size() > 7) addColorStop((levels[levels.size() - 3] - minLevel) / intervalWidth, "#ffb000"); // orange
    if (levels.size() > 6) addColorStop((levels[levels.size() - 2] - minLevel) / intervalWidth, Qt::red);
  }
};


// ContourPlot

ContourPlot::ContourPlot(const Distribution &distribution, const bool isPDF, QWidget *parent)
  : PlotWidget(isPDF ? tr("copulaPDF") : tr("copulaCDF"), true, parent)
{
  updateContour(distribution, isPDF);
}


ContourPlot::ContourPlot(const Collection<Drawable> &drawables, QWidget *parent)
  : PlotWidget(tr("correlation"), true, parent)
{
  plotContour(drawables, 0, true, false);
}


void ContourPlot::updateContour(const Distribution &distribution, const bool isPDF)
{
  if (distribution.getDimension() != 2)
  {
    qDebug() << "In plotContour: distribution dimension must be 2";
    return;
  }

  // plot
  if (isPDF)
    plotContour(distribution.drawPDF().getDrawables(), 1, false, true);
  else
    plotContour(distribution.drawCDF().getDrawables(), 0, false, false);
}


void ContourPlot::plotContour(const Collection<Drawable>& drawables,
                              const UnsignedInteger drawableIndex,
                              const bool displayGradient,
                              const bool isPDF)
{
  // contour
  Contour * contour = dynamic_cast<Contour*>(drawables[drawableIndex].getImplementation().get());
  if (!contour)
  {
    qDebug() << "In plotContour: the drawable is not a Contour";
    return;
  }

  // spectrogram
  QwtPlotSpectrogram * spectrogram = new QwtPlotSpectrogram;

  spectrogram->setRenderThreadCount(0); // use system specific thread count
  spectrogram->setCachePolicy(QwtPlotRasterItem::PaintCache);
  spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, true);
  spectrogram->setRenderHint(QwtPlotItem::RenderAntialiased, true);
  spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode, displayGradient); // color gradient
  if (!displayGradient)
    spectrogram->setDefaultContourPen(QPen(Qt::NoPen));

  // intervals
  const QwtInterval xInterval(contour->getX().getMin()[0], contour->getX().getMax()[0]);
  const QwtInterval yInterval(contour->getY().getMin()[0], contour->getY().getMax()[0]);
  const double maxZ = isPDF ? contour->getData().computeQuantile(0.97)[0] : contour->getData().getMax()[0];
  const QwtInterval zInterval(contour->getData().getMin()[0], maxZ);
  // data
  RasterData * rasterData = new RasterData(contour);
  rasterData->setInterval(Qt::XAxis, xInterval);
  rasterData->setInterval(Qt::YAxis, yInterval);
  rasterData->setInterval(Qt::ZAxis, zInterval);
  spectrogram->setData(rasterData);

  // levels
  QList<double> levels;
  for (UnsignedInteger i = 0; i < drawables.getSize(); ++i)
  {
    Contour * contour = dynamic_cast<Contour*>(drawables[i].getImplementation().get());
    if (contour)
      levels.append(drawables[i].getLevels()[0]);
  }
  std::sort(levels.begin(), levels.end());
  spectrogram->setContourLevels(levels);

  // color map
  spectrogram->setColorMap(new ColorMap(levels));

  spectrogram->attach(this);

  // A color bar on the right axis
  QwtScaleWidget * rightAxis = axisWidget(QwtPlot::yRight);
  rightAxis->setTitle(tr("Density"));
  rightAxis->setColorBarEnabled(true);
  rightAxis->setScaleDraw(new CustomScaleDraw);
  // - color map
  rightAxis->setColorMap(zInterval, new ColorMap(levels));

  setAxisScale(QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue());
  enableAxis(QwtPlot::yRight);

  setAxisScale(QwtPlot::xBottom, xInterval.minValue(), xInterval.maxValue());
  setAxisScale(QwtPlot::yLeft, yInterval.minValue(), yInterval.maxValue());

  plotLayout()->setAlignCanvasToScales(true);
}
}
