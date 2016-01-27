// DesignOfExperiment.hxx

#ifndef DESIGNOFEXPERIMENT_H
#define DESIGNOFEXPERIMENT_H

#include "DesignOfExperimentImplementation.hxx"

namespace OTGUI {
class DesignOfExperiment : public OT::TypedInterfaceObject<DesignOfExperimentImplementation>
{
  CLASSNAME;

public:
  typedef OT::Pointer<DesignOfExperimentImplementation>       Implementation;

  DesignOfExperiment(const std::string & name, const PhysicalModel & physicalModel);
  DesignOfExperiment(const std::string & name, const PhysicalModel & physicalModel,
                     const OT::NumericalPoint & lowerBounds, const OT::NumericalPoint & upperBounds,
                     const OT::Indices & nbValues, const OT::NumericalPoint & values=OT::NumericalPoint(0));
  DesignOfExperiment(const std::string & name, const PhysicalModel & physicalModel,
                     const std::string & fileName, OT::Indices columns=OT::Indices());
//   TODO
//   DesignOfExperiment(const std::string & name, const PhysicalModel & physicalModel,
//                      const OT::Experiment & experiment);
  DesignOfExperiment(const DesignOfExperimentImplementation & implementation);
  DesignOfExperiment(const Implementation & p_implementation);
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

  std::string getFileName() const;
  void setFileName(const std::string & fileName);
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
  std::string dump() const;
};
}
#endif