// SimulationAnalysisResult.hxx

#ifndef SIMULATIONANALYSISRESULT_H
#define SIMULATIONANALYSISRESULT_H

#include "ModelEvaluationResult.hxx"

namespace OTGUI {
typedef OT::Collection<OT::NumericalSample> NumericalSampleCollection;
class SimulationAnalysisResult : public ModelEvaluationResult
{
  CLASSNAME;

public:
  SimulationAnalysisResult();
  SimulationAnalysisResult(OT::NumericalSample inputSample, OT::NumericalSample outputSample);

  virtual SimulationAnalysisResult * clone() const;

  NumericalSampleCollection getListXMin() const;
  NumericalSampleCollection getListXMax() const;

protected:
  void searchMinMax();

private:
  NumericalSampleCollection listXMin_;
  NumericalSampleCollection listXMax_;
};
}
#endif