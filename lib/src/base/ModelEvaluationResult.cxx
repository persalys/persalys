//                                               -*- C++ -*-
/**
 *  @brief Results of the evaluation of the physical models 
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
#include "otgui/ModelEvaluationResult.hxx"

#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(ModelEvaluationResult);

static Factory<ModelEvaluationResult> RegisteredFactory;

/* Default constructor */
ModelEvaluationResult::ModelEvaluationResult()
  : PersistentObject()
{
}


/* Constructor with parameters */
ModelEvaluationResult::ModelEvaluationResult(Sample inputSample, Sample outputSample)
  : PersistentObject()
  , inputSample_(inputSample)
  , outputSample_(outputSample)
{
}


/* Virtual constructor */
ModelEvaluationResult* ModelEvaluationResult::clone() const
{
  return new ModelEvaluationResult(*this);
}


Sample ModelEvaluationResult::getInputSample() const
{
  return inputSample_;
}


Sample ModelEvaluationResult::getOutputSample() const
{
  return outputSample_;
}


/* String converter */
String ModelEvaluationResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " inputSample=" << getInputSample()
      << " outputSample=" << getOutputSample();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void ModelEvaluationResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("outputSample_", outputSample_);
}


/* Method load() reloads the object from the StorageManager */
void ModelEvaluationResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("outputSample_", outputSample_);
}
}