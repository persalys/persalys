//                                               -*- C++ -*-
/**
 *  @brief SimulationAnalysis evaluates the models
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
#ifndef OTGUI_SIMULATIONANALYSIS_HXX
#define OTGUI_SIMULATIONANALYSIS_HXX

#include "AnalysisImplementation.hxx"

namespace OTGUI {
class OTGUI_API SimulationAnalysis : public AnalysisImplementation
{
  CLASSNAME;

public:
  /** Default constructor */
  SimulationAnalysis();
  /** Constructor with parameters */
  SimulationAnalysis(const OT::String & name, const PhysicalModel & physicalModel, const OT::UnsignedInteger nbSimu);

  /** Virtual constructor */
  virtual SimulationAnalysis * clone() const;

  // TODO choose output(s) to evaluate. But maybe use outputNames instead of OutputCollection
//   OutputCollection getOutputs() const;
//   void setOutputs(const OutputCollection & outputs);

  OT::UnsignedInteger getNbSimulations() const;
  void setNbSimulations(const OT::UnsignedInteger nbSimu);

  OT::NumericalSample getInputSample();
  OT::NumericalSample getInputSample(const OT::UnsignedInteger nbSimu);
  OT::Description getOutputNames() const;

  OT::UnsignedInteger getSeed() const;
  void setSeed(const OT::UnsignedInteger seed);

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  OT::NumericalSample getOutputSample(OT::NumericalSample inputSample) const;
  OT::NumericalSample getOutputSample(OT::NumericalSample inputSample, const OT::Description & outputNames) const;

private:
  // TODO choose output(s) to evaluate
//   OT::PersistentCollection<Output> outputs_;
  OT::UnsignedInteger nbSimulations_;
  OT::UnsignedInteger seed_;
};
}
#endif