// ModelEvaluationResult.hxx

#ifndef MODELEVALUATIONRESULT_H
#define MODELEVALUATIONRESULT_H

#include "PhysicalModel.hxx"

namespace OTGUI {
class ModelEvaluationResult
{
  CLASSNAME;

public:
  ModelEvaluationResult();
  ModelEvaluationResult(OT::NumericalSample inputSample, OT::NumericalSample outputSample);

  OT::NumericalSample getInputSample() const;
  OT::NumericalSample getOutputSample() const;

private:
  OT::NumericalSample outputSample_;
  OT::NumericalSample inputSample_;
};
}
#endif