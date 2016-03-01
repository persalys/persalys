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
class OTGUI_API TaylorExpansionsMomentsAnalysis : public AnalysisImplementation
{
  CLASSNAME;

public:
  /** Default constructor */
  TaylorExpansionsMomentsAnalysis();
  /** Constructor with parameters */
  TaylorExpansionsMomentsAnalysis(const OT::String & name, const PhysicalModel & physicalModel);

  /** Virtual constructor */
  virtual TaylorExpansionsMomentsAnalysis * clone() const;

  OT::Description getOutputNames() const;
  void setOutputNames(const OT::Description & outputNames);

  TaylorExpansionsMomentsResult getResult() const;

  virtual void run();
  virtual OT::String getPythonScript() const;
  virtual bool analysisLaunched() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::Description outputNames_;
  TaylorExpansionsMomentsResult result_;
};
}
#endif