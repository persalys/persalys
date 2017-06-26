//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all analysis with design of experiment
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
#ifndef OTGUI_DESIGNOFEXPERIMENTANALYSIS_HXX
#define OTGUI_DESIGNOFEXPERIMENTANALYSIS_HXX

#include "AnalysisImplementation.hxx"
#include "DesignOfExperiment.hxx"

namespace OTGUI {
class OTGUI_API DesignOfExperimentAnalysis : public AnalysisImplementation
{
public:
  /** Default constructor */
  DesignOfExperimentAnalysis();

  /** Constructor with parameters */
  DesignOfExperimentAnalysis(const OT::String & name, const DesignOfExperiment & designOfExperiment);

  /** Virtual constructor */
  virtual DesignOfExperimentAnalysis * clone() const;

  DesignOfExperiment getDesignOfExperiment() const;
  virtual void setDesignOfExperiment(const DesignOfExperiment & designOfExperiment);

  virtual void setInterestVariables(const OT::Description& variablesNames);

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  DesignOfExperiment designOfExperiment_;
};
}
#endif