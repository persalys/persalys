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
                     const OT::Indices & nbValues);
  DesignOfExperiment(const std::string & name, const PhysicalModel & physicalModel,
                     const std::string & fileName, OT::Indices columns=OT::Indices());
  DesignOfExperiment(const std::string & name, const PhysicalModel & physicalModel,
                     const OT::Experiment & experiment);
  DesignOfExperiment(const DesignOfExperimentImplementation & implementation);
  DesignOfExperiment(const Implementation & p_implementation);
  DesignOfExperiment(DesignOfExperimentImplementation * p_implementation);
  DesignOfExperiment(const DesignOfExperiment & other);

  virtual ~DesignOfExperiment();

  void addObserver(Observer * observer);

  std::string getName() const;
  void setName(const std::string & name);

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  DesignOfExperimentImplementation::Type getTypeDesignOfExperiment() const;
  int getNumberOfExperiments() const;

  OT::NumericalPoint getLowerBounds() const;
  void setLowerBounds(const OT::NumericalPoint & lowerBounds);
  OT::NumericalPoint getUpperBounds() const;
  void setUpperBounds(const OT::NumericalPoint & upperBounds);
  OT::Indices getLevels() const;
  void setLevels(const OT::Indices & nbValues);
  OT::NumericalPoint getDeltas() const;
  void setDeltas(const OT::NumericalPoint & deltas);

  std::string getFileName() const;
  void setFileName(const std::string & fileName, OT::Indices columns=OT::Indices());
  OT::Indices getColumns() const;
  void setColumns(OT::Indices columns);

  OT::Experiment getExperiment() const;
  void setExperiment(const OT::Experiment & experiment);

  OT::NumericalSample getInputSample();
  void setInputSample(const OT::NumericalSample & sample);
  OT::NumericalSample getOutputSample();
  void setOutputSample(const OT::NumericalSample & sample);
  ParametricAnalysisResult getResult() const;

  void updateParameters();
  void eval();
  std::string dump() const;
};
}
#endif