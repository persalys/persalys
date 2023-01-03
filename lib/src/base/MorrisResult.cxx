//                                               -*- C++ -*-
/**
 *  @brief MorrisResult
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/MorrisResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(MorrisResult)

static Factory<MorrisResult> Factory_MorrisResult;
static Factory<PersistentCollection<Indices > > Factory_PersistentCollection_Indices;

/* Default constructor */
MorrisResult::MorrisResult()
  : EvaluationResult()
  , inputsSelection_()
  , noEffectBoundary_()
  , elementaryEffectsMean_()
  , elementaryEffectsStandardDeviation_()
  , absoluteElementaryEffectsMean_()
{
}


/* Virtual constructor */
MorrisResult* MorrisResult::clone() const
{
  return new MorrisResult(*this);
}


Indices MorrisResult::getInputsSelection(const UnsignedInteger marginal) const
{
  if (marginal >= inputsSelection_.getSize())
    throw InvalidArgumentException(HERE) << "Cannot exceed dimension";
  return inputsSelection_[marginal];
}


void MorrisResult::setInputsSelection(const UnsignedInteger marginal, const Indices& values)
{
  if (marginal < inputsSelection_.getSize() && values.getSize() == inputsSelection_[marginal].getSize())
    inputsSelection_[marginal] = values;
}


Scalar MorrisResult::getNoEffectBoundary(const UnsignedInteger marginal) const
{
  if (marginal >= noEffectBoundary_.getSize())
    throw InvalidArgumentException(HERE) << "Cannot exceed dimension";
  return noEffectBoundary_[marginal];
}


void MorrisResult::setNoEffectBoundary(const UnsignedInteger marginal, const Scalar value)
{
  if (marginal >= noEffectBoundary_.getSize())
    throw InvalidArgumentException(HERE) << "Cannot exceed dimension";
  noEffectBoundary_[marginal] = value;
}


/* Mean effects */
Point MorrisResult::getMeanAbsoluteElementaryEffects(const UnsignedInteger marginal) const
{
  if (marginal >= absoluteElementaryEffectsMean_.getSize())
    throw InvalidArgumentException(HERE) << "Cannot exceed dimension";
  return absoluteElementaryEffectsMean_[marginal];
}

/* Mean effects */
Point MorrisResult::getMeanElementaryEffects(const UnsignedInteger marginal) const
{
  if (marginal >= elementaryEffectsMean_.getSize())
    throw InvalidArgumentException(HERE) << "Cannot exceed dimension";
  return elementaryEffectsMean_[marginal];
}

/* Standard deviation effects */
Point MorrisResult::getStandardDeviationElementaryEffects(const UnsignedInteger marginal) const
{
  if (marginal >= elementaryEffectsStandardDeviation_.getSize())
    throw InvalidArgumentException(HERE) << "Cannot exceed dimension";
  return elementaryEffectsStandardDeviation_[marginal];
}


/* String converter */
String MorrisResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " inputs selections =" << inputsSelection_
      << " no effect boundaries =" << noEffectBoundary_
      << " ee mean= " << elementaryEffectsMean_
      << " absolute ee mean= " << absoluteElementaryEffectsMean_
      << " ee std= " << elementaryEffectsStandardDeviation_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void MorrisResult::save(Advocate & adv) const
{
  EvaluationResult::save(adv);
  adv.saveAttribute("inputsSelection_", inputsSelection_);
  adv.saveAttribute("noEffectBoundary_", noEffectBoundary_);
  adv.saveAttribute("elementaryEffectsMean_", elementaryEffectsMean_ );
  adv.saveAttribute("elementaryEffectsStandardDeviation_", elementaryEffectsStandardDeviation_ );
  adv.saveAttribute("absoluteElementaryEffectsMean_", absoluteElementaryEffectsMean_ );
}


/* Method load() reloads the object from the StorageManager */
void MorrisResult::load(Advocate & adv)
{
  EvaluationResult::load(adv);
  adv.loadAttribute("inputsSelection_", inputsSelection_);
  adv.loadAttribute("noEffectBoundary_", noEffectBoundary_);
  adv.loadAttribute("elementaryEffectsMean_", elementaryEffectsMean_ );
  adv.loadAttribute("elementaryEffectsStandardDeviation_", elementaryEffectsStandardDeviation_ );
  adv.loadAttribute("absoluteElementaryEffectsMean_", absoluteElementaryEffectsMean_ );
  // can open older xml files
  if (!designOfExperiment_.getSample().getSize())
  {
    Sample inS;
    Sample outS;
    adv.loadAttribute("inputSample_", inS);
    adv.loadAttribute("outputSample_", outS);
    designOfExperiment_.setInputSample(inS);
    designOfExperiment_.setOutputSample(outS);
  }
}
}
