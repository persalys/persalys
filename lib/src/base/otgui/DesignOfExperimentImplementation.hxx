// DesignOfExperimentImplementation.hxx

#ifndef DESIGNOFEXPERIMENTIMPLEMENTATION_H
#define DESIGNOFEXPERIMENTIMPLEMENTATION_H

#include "PhysicalModel.hxx"
#include "SimulationAnalysisResult.hxx"

#include "Experiment.hxx"

namespace OTGUI {
class DesignOfExperimentImplementation : public OT::PersistentObject, public Observable
{
public:
  enum Type {FromBoundsAndLevels, FromBoundsAndDeltas, FromFile, FromExperiment};

  DesignOfExperimentImplementation(const OT::String & name, const PhysicalModel & physicalModel);
  DesignOfExperimentImplementation(const OT::String & name, const PhysicalModel & physicalModel,
                     const OT::NumericalPoint & lowerBounds, const OT::NumericalPoint & upperBounds,
                     const OT::Indices & nbValues, const OT::NumericalPoint & values=OT::NumericalPoint(0));
  DesignOfExperimentImplementation(const OT::String & name, const PhysicalModel & physicalModel,
                                   const OT::String & fileName, const OT::Indices & columns);
//   TODO
//   DesignOfExperimentImplementation(const OT::String & name, const PhysicalModel & physicalModel,
//                                    const OT::Experiment & experiment);
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
  virtual OT::String dump() const;

protected:
  void initializeParameters(const InputCollection & inputs);

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