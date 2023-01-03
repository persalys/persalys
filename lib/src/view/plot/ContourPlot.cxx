//                                               -*- C++ -*-
/**
 *  @brief QwtPlot for contour
 *
 *  Copyright 2015-2023 EDF-Phimeca
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

using namespace OT;

namespace PERSALYS
{


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
    throw InvalidArgumentException(HERE)<< "In plotContour: distribution dimension must be 2";

  // plot
  if (isPDF)
    plotContour(distribution.drawPDF().getDrawables(), 1, false, true);
  else
    plotContour(distribution.drawCDF().getDrawables(), 0, false, false);
}


}
