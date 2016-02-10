//                                               -*- C++ -*-
/**
 *  @brief TaylorExpansionsMomentsAnalysis computes the moments with the Taylor Expansion method
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
#ifndef OTGUI_TAYLOREXPANSIONSMOMENTSANALYSIS_HXX
#define OTGUI_TAYLOREXPANSIONSMOMENTSANALYSIS_HXX

#include "Analysis.hxx"
#include "TaylorExpansionsMomentsResult.hxx"

namespace OTGUI {
class TaylorExpansionsMomentsAnalysis : public AnalysisImplementation
{
  CLASSNAME;

public:
  TaylorExpansionsMomentsAnalysis(const OT::String & name, const PhysicalModel & physicalModel);

  virtual TaylorExpansionsMomentsAnalysis * clone() const;

  OutputCollection getOutputs() const;
  void setOutputs(const OutputCollection & outputs);

  TaylorExpansionsMomentsResult getResult() const;

  virtual void run();
  virtual OT::String dump() const;
  virtual bool analysisLaunched() const;

private:
  OutputCollection outputs_;
  TaylorExpansionsMomentsResult result_;
};
}
#endif