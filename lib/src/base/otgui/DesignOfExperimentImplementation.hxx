//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#ifndef OTGUI_DESIGNOFEXPERIMENTIMPLEMENTATION_HXX
#define OTGUI_DESIGNOFEXPERIMENTIMPLEMENTATION_HXX

#include "PhysicalModel.hxx"
#include "SimulationAnalysisResult.hxx"

namespace OTGUI {
class OTGUI_API DesignOfExperimentImplementation : public OT::PersistentObject, public Observable
{
public:
  /** Default constructor */
  DesignOfExperimentImplementation();
  /** Constructor with parameters */
  DesignOfExperimentImplementation(const OT::String & name, const PhysicalModel & physicalModel);

  /** Virtual constructor */
  virtual DesignOfExperimentImplementation * clone() const;

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  virtual OT::Description getVariableInputNames() const;

  virtual OT::NumericalSample getInputSample();
  void setInputSample(const OT::NumericalSample & sample);
  SimulationAnalysisResult getResult() const;
  void clearResult();

  void evaluate();
  virtual OT::String getPythonScript() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  PhysicalModel physicalModel_;
  OT::NumericalSample inputSample_;
  SimulationAnalysisResult result_;
};
}
#endif