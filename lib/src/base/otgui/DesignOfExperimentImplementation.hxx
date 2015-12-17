// DesignOfExperimentImplementation.hxx

#ifndef DESIGNOFEXPERIMENTIMPLEMENTATION_H
#define DESIGNOFEXPERIMENTIMPLEMENTATION_H

#include "PhysicalModel.hxx"
#include "ParametricAnalysisResult.hxx"

#include "Experiment.hxx"

namespace OTGUI {
class DesignOfExperimentImplementation : public OT::PersistentObject, public Observable
{
public:
  enum Type {FromBoundsAndLevels, FromBoundsAndDeltas, FromFile, FromExperiment};

  DesignOfExperimentImplementation(const std::string & name, const PhysicalModel & physicalModel);
  DesignOfExperimentImplementation(const std::string & name, const PhysicalModel & physicalModel,
                     const OT::NumericalPoint & lowerBounds, const OT::NumericalPoint & upperBounds,
                     const OT::Indices & nbValues);
  DesignOfExperimentImplementation(const std::string & name, const PhysicalModel & physicalModel,
                                   const std::string & fileName, OT::Indices columns);
  DesignOfExperimentImplementation(const std::string & name, const PhysicalModel & physicalModel,
                                   const OT::Experiment & experiment);
  DesignOfExperimentImplementation(const DesignOfExperimentImplementation & other);
  virtual DesignOfExperimentImplementation * clone() const;

  std::string getName() const;
  void setName(const std::string & name);

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  Type getTypeDesignOfExperiment() const;
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
  void setFileName(const std::string & fileName, OT::Indices columns);
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
  virtual std::string dump() const;

protected:
  void initializeParameters(const InputCollection & inputs);
  void importSampleFromFile(const std::string & fileName);

private:
  std::string name_;
  PhysicalModel physicalModel_;
  Type type_;
  OT::NumericalSample inputSample_;
  OT::NumericalSample sampleFromFile_;
  OT::Description inputNames_;
  OT::NumericalPoint lowerBounds_;
  OT::NumericalPoint upperBounds_;
  OT::Indices levels_;
  OT::NumericalPoint deltas_;
  std::string fileName_;
  OT::Indices columns_;
  OT::Experiment experiment_;
  OT::NumericalSample outputSample_;
  ParametricAnalysisResult result_;
};
}
#endif