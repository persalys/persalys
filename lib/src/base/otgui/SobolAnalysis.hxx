//                                               -*- C++ -*-
/**
 *  @brief SobolAnalysis computes the Sobol indices
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
#ifndef OTGUI_SOBOLANALYSIS_HXX
#define OTGUI_SOBOLANALYSIS_HXX

#include "SimulationAnalysis.hxx"
#include "SobolResult.hxx"

namespace OTGUI {
class SobolAnalysis : public SimulationAnalysis
{
  CLASSNAME;

public:
  SobolAnalysis(const OT::String & name, const PhysicalModel & physicalModel, const OT::UnsignedInteger nbSimu=10000);

  virtual SobolAnalysis * clone() const;

  SobolResult getResult() const;

  virtual void run();
  virtual OT::String dump() const;
  virtual bool analysisLaunched() const;

private:
  SobolResult result_;
};
}
#endif