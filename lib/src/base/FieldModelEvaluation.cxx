//                                               -*- C++ -*-
/**
 *  @brief Evaluates the physical models
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/FieldModelEvaluation.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(FieldModelEvaluation)

static Factory<FieldModelEvaluation> Factory_FieldModelEvaluation;

/* Default constructor */
FieldModelEvaluation::FieldModelEvaluation()
  : ModelEvaluation()
  , processSample_()
{
}


/* Constructor with parameters */
FieldModelEvaluation::FieldModelEvaluation(const String& name, const PhysicalModel& physicalModel)
  : ModelEvaluation(name, physicalModel)
  , processSample_()
{
  initializeParameters();
  setInterestVariables(getPhysicalModel().getSelectedOutputsNames());
}


/* Constructor with parameters */
FieldModelEvaluation::FieldModelEvaluation(const String& name,
                                 const PhysicalModel& physicalModel,
                                 const Point& inP)
  : ModelEvaluation(name, physicalModel, inP)
  , processSample_()
{
  setInterestVariables(getPhysicalModel().getSelectedOutputsNames());
}


/* Virtual constructor */
FieldModelEvaluation* FieldModelEvaluation::clone() const
{
  return new FieldModelEvaluation(*this);
}


void FieldModelEvaluation::launch()
{
  if (getValues().getSize() != getPhysicalModel().getInputDimension())
    throw InvalidArgumentException(HERE) << "Wrong input point dimension";

  // output = f(input)
  ProcessSample processSample(getPhysicalModel().getMeshModel().getMesh(), 1, getInterestVariables().getSize());
  processSample[0] = getPhysicalModel().getPointToFieldFunction(getInterestVariables())(getValues());

  processSample_ = processSample;

  // set design of experiments
  // input sample
  Sample inputSample(1, getValues());
  inputSample.setDescription(inputNames_);
  result_.designOfExperiment_.setInputSample(inputSample);
}


ProcessSample FieldModelEvaluation::getProcessSample() const
{
  return processSample_;
}


String FieldModelEvaluation::getPythonScript() const
{
  String result;

  OSS oss;
  oss << "values = " << getValues().__str__() << "\n";
  oss << getName() << " = otguibase.FieldModelEvaluation('" << getName() << "', " << getPhysicalModel().getName();
  oss << ", values)\n";
  if (getInterestVariables().getSize() < getPhysicalModel().getSelectedOutputsNames().getSize())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }

  return oss;
}


bool FieldModelEvaluation::hasValidResult() const
{
  return processSample_.getSize() != 0;
}


/* Method save() stores the object through the StorageManager */
void FieldModelEvaluation::save(Advocate & adv) const
{
  ModelEvaluation::save(adv);
  adv.saveAttribute("processSample_", processSample_);
}


/* Method load() reloads the object from the StorageManager */
void FieldModelEvaluation::load(Advocate & adv)
{
  ModelEvaluation::load(adv);
  adv.loadAttribute("processSample_", processSample_);
}
}
