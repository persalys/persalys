//                                               -*- C++ -*-
/**
 *  @brief Evaluates the physical models
 *
 *  Copyright 2015-2016 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/ModelEvaluation.hxx"

#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(ModelEvaluation);

static Factory<ModelEvaluation> RegisteredFactory;

/* Default constructor */
ModelEvaluation::ModelEvaluation()
  : PhysicalModelAnalysis()
{
}


/* Constructor with parameters */
ModelEvaluation::ModelEvaluation(const String & name, const PhysicalModel & physicalModel)
  : PhysicalModelAnalysis(name, physicalModel)
{
  initializeParameters(physicalModel.getInputs());
}


/* Constructor with parameters */
ModelEvaluation::ModelEvaluation(const String & name, const PhysicalModel & physicalModel,
                                 const NumericalPoint & inputsValues)
  : PhysicalModelAnalysis(name, physicalModel)
  , inputNames_(getPhysicalModel().getInputNames())
  , inputValues_(inputsValues)
{
}


/* Virtual constructor */
ModelEvaluation* ModelEvaluation::clone() const
{
  return new ModelEvaluation(*this);
}


void ModelEvaluation::initializeParameters(const InputCollection & inputs)
{
  inputValues_.clear();
  inputNames_ = getPhysicalModel().getInputNames();

  UnsignedInteger inputSize = inputs.getSize();
  inputValues_ = NumericalPoint(inputSize);

  for (UnsignedInteger i=0; i<inputSize; ++i)
    inputValues_[i] = inputs[i].getValue();
}


void ModelEvaluation::updateParameters()
{
  Description inputNames(inputNames_);
  NumericalPoint values(inputValues_);

  initializeParameters(getPhysicalModel().getInputs());

  for (UnsignedInteger i=0; i<inputNames.getSize(); ++i)
  {
    const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
    if (it != inputNames.end())
      inputValues_[i] = values[it - inputNames.begin()];
  }
}


void ModelEvaluation::run()
{
  // clear result
  result_ = ModelEvaluationResult();

  // output = f(input)
  NumericalSample inputSample(1, getInputValues());
  inputSample.setDescription(inputNames_);
  NumericalSample outputSample(1, 0);
  for (UnsignedInteger i=0; i<getPhysicalModel().getSelectedOutputsNames().getSize(); ++i)
  {
    try
    {
      outputSample.stack(getPhysicalModel().getFunction(getPhysicalModel().getSelectedOutputsNames()[i])(inputSample));
    }
    catch (std::exception & ex)
    {
      throw AnalysisExecutionFailedException(HERE) << "Impossible to evaluate the output "
                                                   << getPhysicalModel().getSelectedOutputsNames()[i]
                                                   << ".\n" << ex.what();
    }
  }
  outputSample.setDescription(getPhysicalModel().getSelectedOutputsNames());
  result_ = ModelEvaluationResult(inputSample, outputSample);
  notify("analysisFinished");
}


NumericalPoint ModelEvaluation::getInputValues() const
{
  return inputValues_;
}


void ModelEvaluation::setInputValue(const int & index, const double & value)
{
  // TODO error if index > nbInputs
  inputValues_[index] = value;
}


ModelEvaluationResult ModelEvaluation::getResult() const
{
  return result_;
}


String ModelEvaluation::getPythonScript() const
{
  String result;

  OSS oss;
  oss << "values = [";
  for (UnsignedInteger i=0; i<inputValues_.getSize(); ++i)
  {
    oss << inputValues_[i];
    if (i < inputValues_.getSize()-1)
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
  return getResult().getOutputSample().getSize() != 0;
}


/* String converter */
String ModelEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " physicalModel=" << getPhysicalModel().getName()
      << " inputValues=" << getInputValues();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void ModelEvaluation::save(Advocate & adv) const
{
  PhysicalModelAnalysis::save(adv);
  adv.saveAttribute("inputNames_", inputNames_);
  adv.saveAttribute("inputValues_", inputValues_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void ModelEvaluation::load(Advocate & adv)
{
  PhysicalModelAnalysis::load(adv);
  adv.loadAttribute("inputNames_", inputNames_);
  adv.loadAttribute("inputValues_", inputValues_);
  adv.loadAttribute("result_", result_);
}
}