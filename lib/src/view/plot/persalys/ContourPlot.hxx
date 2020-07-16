//                                               -*- C++ -*-
/**
 *  @brief QwtPlot for contour
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
#ifndef PERSALYS_CONTOURPLOT_HXX
#define PERSALYS_CONTOURPLOT_HXX

#include "persalys/PlotWidget.hxx"

namespace PERSALYS
{

class PERSALYS_PLOT_API ContourPlot : public PlotWidget
{
  Q_OBJECT

public:
  ContourPlot(const OT::Distribution &distribution, const bool isPDF = true, QWidget *parent = 0);
  ContourPlot(const OT::Collection<OT::Drawable> &drawables, QWidget *parent = 0);

public:
  void updateContour(const OT::Distribution &distribution, const bool isPDF);

private:
  void plotContour(const OT::Collection<OT::Drawable>& drawables,
                   const OT::UnsignedInteger drawableIndex,
                   const bool displayGradient,
                   const bool isPDF);
};
}
#endif
