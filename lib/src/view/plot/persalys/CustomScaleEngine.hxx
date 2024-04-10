//                                               -*- C++ -*-
/**
 *  @brief QwtLinearScaleEngine for distributions plots
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#ifndef PERSALYS_DISTRIBUTIONSCALEENGINE_HXX
#define PERSALYS_DISTRIBUTIONSCALEENGINE_HXX

#include <qwt_scale_engine.h>
#include "persalys/PersalysPrivate.hxx"

namespace PERSALYS
{

class PERSALYS_PLOT_API CustomScaleEngine : public QwtLinearScaleEngine
{
public:
  CustomScaleEngine();

  virtual void autoScale(int maxNumSteps, double & x1, double & x2, double & stepSize) const;

protected:
  QwtInterval aligne(const QwtInterval & interval, const double & stepSize, const int & maxNumSteps) const;
};
}
#endif
