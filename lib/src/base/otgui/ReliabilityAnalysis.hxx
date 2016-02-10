//                                               -*- C++ -*-
/**
 *  @brief ReliabilityAnalysis is the base class for Reliability Analysis
 *
 *  Copyright 2015-2016 EDF
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

#include "AnalysisImplementation.hxx"
#include "LimitState.hxx"

namespace OTGUI {
class ReliabilityAnalysis : public AnalysisImplementation
{
public:
  ReliabilityAnalysis(const OT::String & name, const LimitState & limitState);

  virtual ReliabilityAnalysis * clone() const;

  LimitState getLimitState() const;

private:
  LimitState limitState_;
};
}
#endif