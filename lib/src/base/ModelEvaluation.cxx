//                                               -*- C++ -*-
/**
 *  @brief Evaluates the physical models
 *
 *  Copyright 2015-2017 EDF-Phimeca
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

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(ModelEvaluation)

static Factory<ModelEvaluation> Factory_ModelEvaluation;

/* Default constructor */
ModelEvaluation::ModelEvaluation()
  : PhysicalModelAnalysis()
  , inputNames_()
  , inputValues_()
  , designOfExperiment_()
{
}


/* Constructor with parameters */
ModelEvaluation::ModelEvaluation(const String& name, const PhysicalModel& physicalModel)
  : PhysicalModelAnalysis(name, physicalModel)
  , inputNames_()
  , inputValues_()
  , designOfExperiment_(name, physicalModel)
{
  initializeParameters(physicalModel.getInputs());
  setInterestVariables(getPhysicalModel().getSelectedOutputsNames());
}


/* Constructor with parameters */
ModelEvaluation::ModelEvaluation(const String& name,
                                 const PhysicalModel& physicalModel,
                                 const Point& inputsValues)
  : PhysicalModelAnalysis(name, physicalModel)
  , inputNames_(getPhysicalModel().getInputNames())
  , inputValues_(inputsValues)
  , designOfExperiment_(name, physicalModel)
{
  setInterestVariables(getPhysicalModel().getSelectedOutputsNames());
}


/* Virtual constructor */
ModelEvaluation* ModelEvaluation::clone() const
{
  return new ModelEvaluation(*this);
}


void ModelEvaluation::initializeParameters(const InputCollection& inputs)
{
  inputValues_.clear();
  inputNames_ = getPhysicalModel().getInputNames();

  UnsignedInteger inputSize = inputs.getSize();
  inputValues_ = Point(inputSize);

  for (UnsignedInteger i = 0; i < inputSize; ++i)
    inputValues_[i] = inputs[i].getValue();
}


void ModelEvaluation::updateParameters()
{
  Description inputNames(inputNames_);
  Point values(inputValues_);

  initializeParameters(getPhysicalModel().getInputs());

  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++i)
  {
    const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
    if (it != inputNames.end())
      inputValues_[i] = values[it - inputNames.begin()];
  }
}


void ModelEvaluation::run()
{
  isRunning_ = true;
  try
  {
    // clear result
    initialize();
    designOfExperiment_.getImplementation()->initialize();

    // output = f(input)
    Sample inputSample(1, getInputValues());
    inputSample.setDescription(inputNames_);

    Sample outputSample = getPhysicalModel().getFunction(getInterestVariables())(inputSample);
    outputSample.setDescription(getInterestVariables());

    // set design of experiments
    designOfExperiment_.setInputSample(inputSample);
    designOfExperiment_.setOutputSample(outputSample);

    notify("analysisFinished");
  }
  catch (std::exception & ex)
  {
    errorMessage_ = ex.what();
    notify("analysisBadlyFinished");
  }
  isRunning_ = false;
}


Point ModelEvaluation::getInputValues() const
{
  return inputValues_;
}


void ModelEvaluation::setInputValue(const UnsignedInteger index, const double value)
{
  if (index >= inputValues_.getSize())
    throw InvalidArgumentException(HERE) << "Wrong index value. Must be inferior to " << inputValues_.getSize();

  inputValues_[index] = value;
}


DesignOfExperiment ModelEvaluation::getDesignOfExperiment() const
{
  return designOfExperiment_;
}


Point ModelEvaluation::getOutputValues() const
{
  if (analysisLaunched())
    return getDesignOfExperiment().getOutputSample()[0];
  return Point();
}


String ModelEvaluation::getPythonScript() const
{
  String result;

  OSS oss;
  oss << "values = [";
  for (UnsignedInteger i = 0; i < inputValues_.getSize(); ++i)
  {
    oss << inputValues_[i];
    if (i < inputValues_.getSize() - 1)
      oss << ", ";
  }
  oss << "]\n";
  oss << oss.str();
  oss << getName() << " = otguibase.ModelEvaluation('" << getName() << "', " << getPhysicalModel().getName();
  oss << ", values)\n";

  return oss;
}


bool ModelEvaluation::analysisLaunched() const
{
  return getDesignOfExperiment().getOutputSample().getSize() != 0;
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
  adv.saveAttribute("inputValues_", inputValues_);
  adv.saveAttribute("designOfExperiment_", designOfExperiment_);
}


/* Method load() reloads the object from the StorageManager */
void ModelEvaluation::load(Advocate & adv)
{
  PhysicalModelAnalysis::load(adv);
  adv.loadAttribute("inputValues_", inputValues_);
  adv.loadAttribute("designOfExperiment_", designOfExperiment_);
  inputNames_ = getPhysicalModel().getInputNames();
}
}
