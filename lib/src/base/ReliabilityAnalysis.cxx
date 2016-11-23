//                                               -*- C++ -*-
/**
 *  @brief ReliabilityAnalysis is the base class for Reliability Analysis
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/ReliabilityAnalysis.hxx"

using namespace OT;

namespace OTGUI {

/* Default constructor */
ReliabilityAnalysis::ReliabilityAnalysis()
  : PhysicalModelAnalysis()
{
  setIsReliabilityAnalysis(true);
}


/* Constructor with parameters */
ReliabilityAnalysis::ReliabilityAnalysis(const String & name, const LimitState & limitState)
  : PhysicalModelAnalysis(name, limitState.getPhysicalModel())
  , limitState_(limitState)
{
  setIsReliabilityAnalysis(true);
}


LimitState ReliabilityAnalysis::getLimitState() const
{
  return limitState_;
}


void ReliabilityAnalysis::setInterestVariables(const Description& outputsNames)
{
  if (outputsNames.getSize() == 1)
    if (outputsNames[0] == limitState_.getOutputName())
      return;
  throw InvalidArgumentException(HERE) << "The output to be analysed " << limitState_.getOutputName() << "is already defined in the limit state";
}


/* Method save() stores the object through the StorageManager */
void ReliabilityAnalysis::save(Advocate & adv) const
{
  PhysicalModelAnalysis::save(adv);
  adv.saveAttribute("limitState_", limitState_);
}


/* Method load() reloads the object from the StorageManager */
void ReliabilityAnalysis::load(Advocate & adv)
{
  PhysicalModelAnalysis::load(adv);
  adv.loadAttribute("limitState_", limitState_);
}
}