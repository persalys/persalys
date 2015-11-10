// ParametricAnalysis.hxx

#ifndef PARAMETRICANALYSIS_H
#define PARAMETRICANALYSIS_H

#include "SimulationAnalysis.hxx"
#include "ParametricAnalysisResult.hxx"

namespace OTGUI {
class ParametricAnalysis : public SimulationAnalysis
{
  CLASSNAME;

public:
  ParametricAnalysis(const std::string & name, const PhysicalModel & physicalModel);
  ParametricAnalysis(const std::string & name, const PhysicalModel & physicalModel,
                     const OT::NumericalPoint & infBounds, const OT::NumericalPoint & supBounds,
                     const OT::Indices & nbValues);
  ParametricAnalysis(const ParametricAnalysis & other);
  virtual ParametricAnalysis * clone() const;

  void initializeParameters(const InputCollection & inputs);
  void updateParameters();

  OT::NumericalSample getInputSample();
  void setInputSample(const OT::NumericalSample & inputSample);

  OT::NumericalPoint getInfBounds() const;
  void setInfBound(const int & index, const double & infBounds);
  OT::NumericalPoint getSupBounds() const;
  void setSupBound(const int & index, const double & supBounds);
  OT::Indices getNbValues() const;
  void setNbValues(const int & index, const int & nbValues);

  ParametricAnalysisResult getResult() const;

  virtual void run();
  virtual std::string dump() const;
  virtual bool analysisLaunched() const;

private:
  OT::Description inputNames_;
  OT::NumericalSample inputSample_;
  ParametricAnalysisResult result_;
  OT::NumericalPoint infBounds_;
  OT::NumericalPoint supBounds_;
  OT::Indices nbValues_;
};
}
#endif