//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#ifndef OTGUI_DESIGNOFEXPERIMENT_HXX
#define OTGUI_DESIGNOFEXPERIMENT_HXX

#include "DesignOfExperimentImplementation.hxx"

namespace OTGUI {
class DesignOfExperiment : public OT::TypedInterfaceObject<DesignOfExperimentImplementation>
{
  CLASSNAME;

public:
  typedef OT::Pointer<DesignOfExperimentImplementation>       Implementation;

  /** Default constructor */
  DesignOfExperiment();
  /** Constructor with parameters */
  DesignOfExperiment(const OT::String & name, const PhysicalModel & physicalModel);
  /** Constructor with parameters */
  DesignOfExperiment(const OT::String & name, const PhysicalModel & physicalModel,
                     const OT::NumericalPoint & lowerBounds, const OT::NumericalPoint & upperBounds,
                     const OT::Indices & nbValues, const OT::NumericalPoint & values=OT::NumericalPoint(0));
  /** Constructor with parameters */
  DesignOfExperiment(const OT::String & name, const PhysicalModel & physicalModel,
                     const OT::String & fileName, OT::Indices columns=OT::Indices());
//   TODO
  /** Constructor with parameters */
//   DesignOfExperiment(const OT::String & name, const PhysicalModel & physicalModel,
//                      const OT::Experiment & experiment);
  /** Default constructor */
  DesignOfExperiment(const DesignOfExperimentImplementation & implementation);
  /** Constructor from implementation */
  DesignOfExperiment(const Implementation & p_implementation);
  /** Constructor from implementation pointer */
  DesignOfExperiment(DesignOfExperimentImplementation * p_implementation);

  void addObserver(Observer * observer);

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  DesignOfExperimentImplementation::Type getTypeDesignOfExperiment() const;
  int getNumberOfExperiments() const;

  OT::NumericalPoint getValues() const;
  void setValues(const OT::NumericalPoint & values);
  OT::NumericalPoint getLowerBounds() const;
  void setLowerBounds(const OT::NumericalPoint & lowerBounds);
  OT::NumericalPoint getUpperBounds() const;
  void setUpperBounds(const OT::NumericalPoint & upperBounds);
  OT::Indices getLevels() const;
  void setLevels(const OT::Indices & nbValues);
  OT::NumericalPoint getDeltas() const;
  void setDeltas(const OT::NumericalPoint & deltas);

  OT::String getFileName() const;
  void setFileName(const OT::String & fileName);
  OT::Indices getColumns() const;
  void setColumns(OT::Indices columns);

//   TODO
//   OT::Experiment getExperiment() const;
//   void setExperiment(const OT::Experiment & experiment);

  OT::Description getVariableInputsNames() const;

  OT::NumericalSample getInputSample();
  void setInputSample(const OT::NumericalSample & sample);
  SimulationAnalysisResult getResult() const;
  void clearResult();

  void updateParameters();
  void eval();
  OT::String getPythonScript() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);
};
}
#endif