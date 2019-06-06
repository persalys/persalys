//                                               -*- C++ -*-
/**
 *  @brief Class for analysis with stop criteria
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
#ifndef PERSALYS_WITHSTOPCRITERIAANALYSIS_HXX
#define PERSALYS_WITHSTOPCRITERIAANALYSIS_HXX

#include "SimulationAnalysis.hxx"

namespace PERSALYS
{
class PERSALYS_API WithStopCriteriaAnalysis
{
public:
  /** Default constructor */
  WithStopCriteriaAnalysis();

  virtual ~WithStopCriteriaAnalysis();

  OT::UnsignedInteger getMaximumCalls() const;
  virtual void setMaximumCalls(const OT::UnsignedInteger maxi);

  double getMaximumCoefficientOfVariation() const;
  void setMaximumCoefficientOfVariation(const double coef);

  OT::UnsignedInteger getMaximumElapsedTime() const;
  void setMaximumElapsedTime(const OT::UnsignedInteger seconds);

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  /** Stop callback */
  static OT::Bool Stop(void * p);

private:
  OT::UnsignedInteger maximumCalls_;
  double maximumCoefficientOfVariation_;
  OT::UnsignedInteger maximumElapsedTime_;
};
}
#endif