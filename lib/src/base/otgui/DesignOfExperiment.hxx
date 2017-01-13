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
#ifndef OTGUI_DESIGNOFEXPERIMENT_HXX
#define OTGUI_DESIGNOFEXPERIMENT_HXX

#include "DesignOfExperimentImplementation.hxx"

namespace OTGUI {
class OTGUI_API DesignOfExperiment : public OT::TypedInterfaceObject<DesignOfExperimentImplementation>
{
  CLASSNAME;

public:
  typedef OT::Pointer<DesignOfExperimentImplementation>       Implementation;

  /** Default constructor */
  DesignOfExperiment();
  /** Constructor with parameters */
  DesignOfExperiment(const OT::String & name, const PhysicalModel & physicalModel);

  /** Default constructor */
  DesignOfExperiment(const DesignOfExperimentImplementation & implementation);
  /** Constructor from implementation */
  DesignOfExperiment(const Implementation & p_implementation);
  /** Constructor from implementation pointer */
  DesignOfExperiment(DesignOfExperimentImplementation * p_implementation);

  void addObserver(Observer * observer);

  bool hasPhysicalModel() const;

  PhysicalModel getPhysicalModel() const;

  OT::Description getVariableInputNames() const;

  OT::NumericalSample getInputSample() const;
  OT::NumericalSample getFailedInputSample() const;
  OT::NumericalSample getNotEvaluatedInputSample() const;
  OT::NumericalSample getOutputSample() const;
  OT::NumericalSample getSample() const;

  OT::String getErrorMessage() const;
  int getProgressValue() const;

  void run();
  OT::String getPythonScript() const;
  void stop();

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);
};
}
#endif