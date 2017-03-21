//                                               -*- C++ -*-
/**
 *  @brief ReliabilityAnalysis is the base class for Reliability Analysis
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
#ifndef OTGUI_RELIABILITYANALYSIS_HXX
#define OTGUI_RELIABILITYANALYSIS_HXX

#include "PhysicalModelAnalysis.hxx"
#include "LimitState.hxx"

namespace OTGUI {
class OTGUI_API ReliabilityAnalysis : public PhysicalModelAnalysis
{
public:
  /** Default constructor */
  ReliabilityAnalysis();
  /** Constructor with parameters */
  ReliabilityAnalysis(const OT::String & name, const LimitState & limitState);

  /** Virtual constructor */
  virtual ReliabilityAnalysis * clone() const = 0;

  LimitState getLimitState() const;

  virtual void setInterestVariables(const OT::Description& outputsNames);

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  LimitState limitState_;
};
}
#endif