//                                               -*- C++ -*-
/**
 *  @brief Class for approximation reliability analysis
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_APPROXIMATIONANALYSIS_HXX
#define PERSALYS_APPROXIMATIONANALYSIS_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/OptimizationAlgorithm.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API ApproximationAnalysis
{
public:
  /** Default constructor */
  ApproximationAnalysis();

  virtual ~ApproximationAnalysis();

  OT::OptimizationAlgorithm getOptimizationAlgorithm() const;
  void setOptimizationAlgorithm(const OT::OptimizationAlgorithm& solver);

  OT::Point getPhysicalStartingPoint() const;
  void setPhysicalStartingPoint(const OT::Point& point);

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  OT::OptimizationAlgorithm optimizationAlgorithm_;
private:
  OT::Point physicalStartingPoint_;
};
}
#endif
