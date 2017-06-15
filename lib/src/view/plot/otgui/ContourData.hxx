//                                               -*- C++ -*-
/**
 *  @brief QwtRasterData to draw OpenTURNS Contour
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
#ifndef OTGUI_CONTOURDATA_HXX
#define OTGUI_CONTOURDATA_HXX

#include <openturns/Contour.hxx>

#include <qwt_raster_data.h>

namespace OTGUI {

class OTGUI_API ContourData: public QwtRasterData
{
public:
  ContourData()
    : QwtRasterData()
    , Zvalues_()
    , sizeX_(0)
    , minX_(0.)
    , minY_(0.)
    , deltaX_(0.)
    , deltaY_(0.)
  {

  }

  void setData(const OT::Contour& contour)
  {
    Zvalues_ = contour.getData();

    sizeX_ = contour.getX().getSize();

    minX_ = contour.getX().getMin()[0];
    minY_ = contour.getY().getMin()[0];

    deltaX_ = contour.getX()[1][0] - contour.getX()[0][0];
    deltaY_ = contour.getY()[1][0] - contour.getY()[0][0];

    setInterval(Qt::XAxis, QwtInterval(minX_, contour.getX().getMax()[0]));
    setInterval(Qt::YAxis, QwtInterval(minY_, contour.getY().getMax()[0]));
    setInterval(Qt::ZAxis, QwtInterval(Zvalues_.getMin()[0], Zvalues_.getMax()[0]));
  }

  virtual double value(double x, double y) const
  {
    const int xpos_inf = (int)(floor((x - minX_) / deltaX_));
    const int xpos_sup = (int)(ceil((x - minX_) / deltaX_));

    const int ypos_inf = (int)(floor((y - minY_) / deltaY_));
    const int ypos_sup = (int)(ceil((y - minY_) / deltaY_));

    const int pos1 = getPositionZvalue(xpos_inf, ypos_inf);
    const int pos2 = getPositionZvalue(xpos_inf, ypos_sup);
    const int pos3 = getPositionZvalue(xpos_sup, ypos_inf);
    const int pos4 = getPositionZvalue(xpos_sup, ypos_sup);

    double delta_f_x = 0.;
    double dx_delta_x = 0.;
    if (xpos_inf != xpos_sup)
    {
      dx_delta_x = (x - (xpos_inf * deltaX_)) / deltaX_;
      delta_f_x = Zvalues_[pos3][0] - Zvalues_[pos1][0];
    }

    double delta_f_y = 0.;
    double dy_delta_y = 0.;
    if (ypos_inf != ypos_sup)
    {
      dy_delta_y = (y - (ypos_inf * deltaY_)) / deltaY_;
      delta_f_y = Zvalues_[pos2][0] - Zvalues_[pos1][0];
    }

    const double delta_f_xy = Zvalues_[pos1][0] + Zvalues_[pos4][0] - Zvalues_[pos3][0] - Zvalues_[pos2][0];

    return delta_f_x * dx_delta_x + delta_f_y * dy_delta_y + delta_f_xy * dx_delta_x * dy_delta_y + Zvalues_[pos1][0];
  }

protected:
  int getPositionZvalue(const int x, const int y) const
  {
    return x + sizeX_ * y;
  }

private:
  OT::Sample Zvalues_;
  int sizeX_;
  double minX_;
  double minY_;
  double deltaX_;
  double deltaY_;
};
}
#endif