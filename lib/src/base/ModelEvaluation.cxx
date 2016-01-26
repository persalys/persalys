// ModelEvaluation.cxx

#include "otgui/ModelEvaluation.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(ModelEvaluation);

ModelEvaluation::ModelEvaluation(const std::string & name, const PhysicalModel & physicalModel)
  : AnalysisImplementation(name, physicalModel)
{
  initializeParameters(physicalModel.getInputs());
}


ModelEvaluation::ModelEvaluation(const std::string & name, const PhysicalModel & physicalModel,
                                 const NumericalPoint & inputsValues)
  : AnalysisImplementation(name, physicalModel)
  , inputNames_(getPhysicalModel().getInputNames())
  , inputsValues_(inputsValues)
{
}


ModelEvaluation::ModelEvaluation(const ModelEvaluation & other)
  : AnalysisImplementation(other)
  , inputNames_(other.inputNames_)
  , inputsValues_(other.inputsValues_)
{
}


ModelEvaluation* ModelEvaluation::clone() const
{
  return new ModelEvaluation(*this);
}


void ModelEvaluation::initializeParameters(const InputCollection & inputs)
{
  inputsValues_.clear();
  inputNames_ = getPhysicalModel().getInputNames();

  int inputSize = inputs.getSize();
  inputsValues_ = NumericalPoint(inputSize);

  for (int i=0; i<inputSize; ++i)
    inputsValues_[i] = inputs[i].getValue();
}


void ModelEvaluation::updateParameters()
{
  Description inputNames(inputNames_);
  NumericalPoint values(inputsValues_);

  initializeParameters(getPhysicalModel().getInputs());

  for (int i=0; i<inputNames.getSize(); ++i)
  {
    const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
    if (it != inputNames.end())
      inputsValues_[i] = values[it - inputNames.begin()];
  }
}


void ModelEvaluation::run()
{
  // output = f(input)
  NumericalSample inputSample(1, getInputsValues());
  inputSample.setDescription(inputNames_);
  NumericalSample outputSample(getPhysicalModel().getFunction()(inputSample));
  outputSample.setDescription(getPhysicalModel().getOutputNames());
  result_ = ModelEvaluationResult(inputSample, outputSample);
  notify("analysisFinished");
}


NumericalPoint ModelEvaluation::getInputsValues() const
{
  return inputsValues_;
}


void ModelEvaluation::setInputValue(const int & index, const double & value)
{
  // TODO error if index > nbInputs
  inputsValues_[index] = value;
}


ModelEvaluationResult ModelEvaluation::getResult() const
{
  return result_;
}


std::string ModelEvaluation::dump() const
{
  std::string result;

  OSS oss;
  oss << "values = [";
  for (UnsignedInteger i=0; i<inputsValues_.getSize(); ++i)
  {
    oss << inputsValues_[i];
    if (i < inputsValues_.getSize()-1)
      oss << ", ";
  }
  oss << "]\n";
  result += oss.str();
  result += getName()+ " = otguibase.ModelEvaluation('" + getName() + "', " + getPhysicalModel().getName();
  result += ", values)\n";
  return result;
}


bool ModelEvaluation::analysisLaunched() const
{
//   return getResult().getOutputSample().getSize()!=0;
}
}