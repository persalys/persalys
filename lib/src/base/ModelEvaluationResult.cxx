// ModelEvaluationResult.cxx

#include "otgui/ModelEvaluationResult.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(ModelEvaluationResult);

ModelEvaluationResult::ModelEvaluationResult()
{

}


ModelEvaluationResult::ModelEvaluationResult(NumericalSample inputSample, NumericalSample outputSample)
  : inputSample_(inputSample)
  , outputSample_(outputSample)
{
}


ModelEvaluationResult* ModelEvaluationResult::clone() const
{
  return new ModelEvaluationResult(*this);
}


NumericalSample ModelEvaluationResult::getInputSample() const
{
  return inputSample_;
}


NumericalSample ModelEvaluationResult::getOutputSample() const
{
  return outputSample_;
}
}