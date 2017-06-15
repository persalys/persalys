//                                               -*- C++ -*-
/**
 *  @brief Class for approximation reliability analysis
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
#include "otgui/ApproximationAnalysis.hxx"

#include <openturns/Cobyla.hxx>

using namespace OT;

namespace OTGUI {

/* Default constructor */
ApproximationAnalysis::ApproximationAnalysis()
  : optimizationAlgorithm_(Cobyla())
  , physicalStartingPoint_()
{
}


ApproximationAnalysis::~ApproximationAnalysis()
{
}


OptimizationAlgorithm ApproximationAnalysis::getOptimizationAlgorithm() const
{
  return optimizationAlgorithm_;
}


void ApproximationAnalysis::setOptimizationAlgorithm(const OptimizationAlgorithm& solver)
{
  optimizationAlgorithm_ = solver;
  optimizationAlgorithm_.setStartingPoint(physicalStartingPoint_);
}


Point ApproximationAnalysis::getPhysicalStartingPoint() const
{
  return physicalStartingPoint_;
}


void ApproximationAnalysis::setPhysicalStartingPoint(const Point& point)
{
  optimizationAlgorithm_.setStartingPoint(point);
  physicalStartingPoint_ = point;
}


/* String converter */
String ApproximationAnalysis::__repr__() const
{
  OSS oss;
  oss << " optimization algorithm=" << getOptimizationAlgorithm();
  oss << " physicalStartingPoint=" << getPhysicalStartingPoint();

  return oss;
}


/* Method save() stores the object through the StorageManager */
void ApproximationAnalysis::save(Advocate & adv) const
{
  adv.saveAttribute("optimizationAlgorithm_", optimizationAlgorithm_);
  adv.saveAttribute("physicalStartingPoint_", physicalStartingPoint_);
}


/* Method load() reloads the object from the StorageManager */
void ApproximationAnalysis::load(Advocate & adv)
{
  adv.loadAttribute("optimizationAlgorithm_", optimizationAlgorithm_);
  adv.loadAttribute("physicalStartingPoint_", physicalStartingPoint_);
}
}