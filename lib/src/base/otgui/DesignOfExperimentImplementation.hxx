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
#ifndef OTGUI_DESIGNOFEXPERIMENTIMPLEMENTATION_HXX
#define OTGUI_DESIGNOFEXPERIMENTIMPLEMENTATION_HXX

#include "PhysicalModel.hxx"
#include "SimulationAnalysisResult.hxx"

#include "Experiment.hxx"

namespace OTGUI {
class OTGUI_API DesignOfExperimentImplementation : public OT::PersistentObject, public Observable
{
  CLASSNAME;

public:
  enum Type {FromBoundsAndLevels, FromBoundsAndDeltas, FromFile, FromExperiment};

  /** Default constructor */
  DesignOfExperimentImplementation();
  /** Constructor with parameters */
  DesignOfExperimentImplementation(const OT::String & name, const PhysicalModel & physicalModel);
  /** Constructor with parameters */
  DesignOfExperimentImplementation(const OT::String & name, const PhysicalModel & physicalModel,
                     const OT::NumericalPoint & lowerBounds, const OT::NumericalPoint & upperBounds,
                     const OT::Indices & nbValues, const OT::NumericalPoint & values=OT::NumericalPoint(0));
  /** Constructor with parameters */
  DesignOfExperimentImplementation(const OT::String & name, const PhysicalModel & physicalModel,
                                   const OT::String & fileName, const OT::Indices & columns);
//   TODO
  /** Constructor with parameters */
//   DesignOfExperimentImplementation(const OT::String & name, const PhysicalModel & physicalModel,
//                                    const OT::Experiment & experiment);
  /** Virtual constructor */
  virtual DesignOfExperimentImplementation * clone() const;

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  Type getTypeDesignOfExperiment() const;
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

  void setFileName(const OT::String & fileName);
  OT::String getFileName() const;
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
  virtual OT::String getPythonScript() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  void initializeParameters();

private:
  PhysicalModel physicalModel_;
  Type type_;
  OT::NumericalSample inputSample_;
  OT::Description inputNames_;
  OT::NumericalPoint values_;
  OT::NumericalPoint lowerBounds_;
  OT::NumericalPoint upperBounds_;
  OT::Indices levels_;
  OT::NumericalPoint deltas_;
  OT::String fileName_;
  OT::Indices columns_;
//   TODO
//   OT::Experiment experiment_;
  SimulationAnalysisResult result_;
};
}
#endif