//                                               -*- C++ -*-
/**
 *  @brief Evaluates the physical models
 *
 *  Copyright 2015-2016 EDF
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

#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(ModelEvaluation);

static Factory<ModelEvaluation> RegisteredFactory("ModelEvaluation");

/* Default constructor */
ModelEvaluation::ModelEvaluation()
  : AnalysisImplementation()
{
}


/* Constructor with parameters */
ModelEvaluation::ModelEvaluation(const String & name, const PhysicalModel & physicalModel)
  : AnalysisImplementation(name, physicalModel)
{
  initializeParameters(physicalModel.getInputs());
}


/* Constructor with parameters */
ModelEvaluation::ModelEvaluation(const String & name, const PhysicalModel & physicalModel,
                                 const NumericalPoint & inputsValues)
  : AnalysisImplementation(name, physicalModel)
  , inputNames_(getPhysicalModel().getInputNames())
  , inputsValues_(inputsValues)
{
}


/* Virtual constructor */
ModelEvaluation* ModelEvaluation::clone() const
{
  return new ModelEvaluation(*this);
}


void ModelEvaluation::initializeParameters(const InputCollection & inputs)
{
  inputsValues_.clear();
  inputNames_ = getPhysicalModel().getInputNames();

  UnsignedInteger inputSize = inputs.getSize();
  inputsValues_ = NumericalPoint(inputSize);

  for (UnsignedInteger i=0; i<inputSize; ++i)
    inputsValues_[i] = inputs[i].getValue();
}


void ModelEvaluation::updateParameters()
{
  Description inputNames(inputNames_);
  NumericalPoint values(inputsValues_);

  initializeParameters(getPhysicalModel().getInputs());

  for (UnsignedInteger i=0; i<inputNames.getSize(); ++i)
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
  NumericalSample outputSample(1, 0);
  for (UnsignedInteger i=0; i<getPhysicalModel().getOutputNames().getSize(); ++i)
  {
    try
    {
      outputSample.stack(getPhysicalModel().getFunction(getPhysicalModel().getOutputNames()[i])(inputSample));
    }
    catch (std::exception & ex)
    {
      throw InvalidArgumentException(HERE) << "Impossible to evaluate the output "
                                           << getPhysicalModel().getOutputNames()[i] << ".\n" << ex.what();
    }
  }
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


String ModelEvaluation::dump() const
{
  String result;

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
  return getResult().getOutputSample().getSize() != 0;
}


/* Method save() stores the object through the StorageManager */
void ModelEvaluation::save(Advocate & adv) const
{
  AnalysisImplementation::save(adv);
  adv.saveAttribute("inputNames_", inputNames_);
  adv.saveAttribute("inputsValues_", inputsValues_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void ModelEvaluation::load(Advocate & adv)
{
  AnalysisImplementation::load(adv);
  adv.loadAttribute("inputNames_", inputNames_);
  adv.loadAttribute("inputsValues_", inputsValues_);
  adv.loadAttribute("result_", result_);
}
}