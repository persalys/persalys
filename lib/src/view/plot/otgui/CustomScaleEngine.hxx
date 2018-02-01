//                                               -*- C++ -*-
/**
 *  @brief QwtLinearScaleEngine for distributions plots
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_DISTRIBUTIONSCALEENGINE_HXX
#define OTGUI_DISTRIBUTIONSCALEENGINE_HXX

#include "qwt_scale_engine.h"
#include "otgui/OTGuiprivate.hxx"

namespace OTGUI
{

class OTGUI_API CustomScaleEngine : public QwtLinearScaleEngine
{
public:
  CustomScaleEngine();

  virtual void autoScale(int maxNumSteps, double & x1, double & x2, double & stepSize) const;

protected:
  QwtInterval aligne(const QwtInterval & interval, const double & stepSize, const int & maxNumSteps) const;
};
}
#endif
