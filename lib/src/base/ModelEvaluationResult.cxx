// ModelEvaluationResult.cxx

#include "otgui/ModelEvaluationResult.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(ModelEvaluationResult);

ModelEvaluationResult::ModelEvaluationResult()
{

}


ModelEvaluationResult::ModelEvaluationResult(NumericalSample inputSample, NumericalSample outputSample)
  : outputSample_(outputSample)
  , inputSample_(inputSample)
{
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