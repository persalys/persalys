//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#include "otgui/FixedDesignOfExperiment.hxx"

#include <openturns/Box.hxx>
#include <openturns/TruncatedDistribution.hxx>
#include <openturns/TruncatedNormal.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(FixedDesignOfExperiment);

static Factory<FixedDesignOfExperiment> Factory_FixedDesignOfExperiment;

/* Default constructor */
FixedDesignOfExperiment::FixedDesignOfExperiment()
  : DesignOfExperimentImplementation()
  , type_(FixedDesignOfExperiment::FromBoundsAndLevels)
{
}


/* Constructor with parameters */
FixedDesignOfExperiment::FixedDesignOfExperiment(const String & name, const PhysicalModel & physicalModel)
  : DesignOfExperimentImplementation(name, physicalModel)
  , type_(FixedDesignOfExperiment::FromBoundsAndLevels)
{
  initializeParameters();
}


/* Constructor with parameters */
FixedDesignOfExperiment::FixedDesignOfExperiment(const String & name,
                                                 const PhysicalModel & physicalModel,
                                                 const Point & lowerBounds,
                                                 const Point & upperBounds,
                                                 const Indices & levels,
                                                 const Point & values)
  : DesignOfExperimentImplementation(name, physicalModel)
  , type_(FixedDesignOfExperiment::FromBoundsAndLevels)
  , inputNames_(physicalModel.getInputNames())
  , values_(values)
{
  const UnsignedInteger nbInputs = physicalModel.getInputs().getSize();
  if (!nbInputs)
    throw InvalidArgumentException(HERE) << "The physical model does not have input variables";

  setLowerBounds(lowerBounds);
  setUpperBounds(upperBounds);
  setLevels(levels);

  if (!values_.getSize())
  {
    values_ = Point(nbInputs);
    for (UnsignedInteger i = 0; i < nbInputs; ++i)
      values_[i] = physicalModel.getInputs()[i].getValue();
  }
//   TODO: check if lowerBounds[i] <= upperBounds[i]
}


/* Virtual constructor */
FixedDesignOfExperiment* FixedDesignOfExperiment::clone() const
{
  return new FixedDesignOfExperiment(*this);
}


Sample FixedDesignOfExperiment::getOriginalInputSample() const
{
  if (!originalInputSample_.getSize())
    originalInputSample_ = GenerateInputSample(getLowerBounds(), getUpperBounds(), getLevels(), getValues(), inputNames_);

  return originalInputSample_;
}


FixedDesignOfExperiment::Type FixedDesignOfExperiment::getTypeDesignOfExperiment() const
{
  return type_;
}


void FixedDesignOfExperiment::initializeParameters()
{
  // clear samples
  originalInputSample_.clear();
  initialize();

  inputNames_ = getPhysicalModel().getInputNames();
  const InputCollection inputs = getPhysicalModel().getInputs();

  const UnsignedInteger nbInputs = inputs.getSize();
  values_ = Point(nbInputs);
  lowerBounds_ = Point(nbInputs);
  upperBounds_ = Point(nbInputs);
  levels_ = Indices(nbInputs, 1);
  deltas_ = Point(nbInputs);

  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    values_[i] = inputs[i].getValue();
    if (!inputs[i].isStochastic())
    {
      lowerBounds_[i] = 0.9;
      upperBounds_[i] = 1.1;
      if (values_[i] != 0)
      {
        lowerBounds_[i] = 0.9 * values_[i];
        upperBounds_[i] = 1.1 * values_[i];
      }
    }
    else
    {
      const Distribution distribution = inputs[i].getDistribution();
      const String distributionName = distribution.getImplementation()->getClassName();
      
      if (distributionName == "TruncatedDistribution")
      {
        const TruncatedDistribution truncatedDist = *dynamic_cast<TruncatedDistribution*>(distribution.getImplementation().get());
        if (truncatedDist.getFiniteLowerBound())
          lowerBounds_[i] = truncatedDist.getLowerBound();
        else
          lowerBounds_[i] = distribution.computeQuantile(0.05)[0];
        if (truncatedDist.getFiniteUpperBound())
          upperBounds_[i] = truncatedDist.getUpperBound();
        else
          upperBounds_[i] = distribution.computeQuantile(0.95)[0];
      }
      else if (distributionName == "TruncatedNormal")
      {
        const TruncatedNormal truncatedDist = *dynamic_cast<TruncatedNormal*>(distribution.getImplementation().get());
        lowerBounds_[i] = truncatedDist.getA();
        upperBounds_[i] = truncatedDist.getB();
      }
      else
      {
        lowerBounds_[i] = distribution.computeQuantile(0.05)[0];
        upperBounds_[i] = distribution.computeQuantile(0.95)[0];
      }
    }
  }
}


Sample FixedDesignOfExperiment::GenerateInputSample(const Point& lowerBounds,
                                                    const Point& upperBounds,
                                                    const Indices& levels,
                                                    const Point& values,
                                                    const Description& inputNames)
{
  const UnsignedInteger nbInputs = inputNames.getSize();

  // check arguments size
  if (levels.getSize() != nbInputs ||
      lowerBounds.getSize() != nbInputs ||
      upperBounds.getSize() != nbInputs ||
      values.getSize() != nbInputs)
  {
    throw InvalidArgumentException(HERE) << "All the arguments must have the same size";
  }

  if (!nbInputs)
    return Sample();

  Point scale;
  Point transvec;
  Point otLevels;
  Indices variableInputsIndices;

  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    if (levels[i] > 1)
    {
      const double inf = lowerBounds[i];
      const double sup = upperBounds[i];
      scale.add(sup - inf);
      transvec.add(inf);
      variableInputsIndices.add(i);
      otLevels.add(levels[i]-2);
    }
  }

  Sample inputSample;

  // if there is at least an input which varies
  if (otLevels.getSize())
  {
    Sample sample;
    try
    {
      sample = Box(otLevels).generate();
    }
    catch (std::exception & ex)
    {
      throw InvalidValueException(HERE) << "Impossible to generate the design of experiment\n " << ex.what();
    }

    sample *= scale;
    sample += transvec;

    // if all the inputs vary
    if (sample.getDimension() == nbInputs)
    {
      inputSample = sample;
    }
    // if there is at least an input which is constant
    else
    {
      inputSample = Sample(sample.getSize(), values);
      for (UnsignedInteger j=0; j<sample.getSize(); ++j)
      {
        for (UnsignedInteger i=0; i<variableInputsIndices.getSize(); ++i)
        {
          inputSample[j][variableInputsIndices[i]] = sample[j][i];
        }
      }
    }
  }
  // if all the inputs are constant
  else
  {
    inputSample = Sample(1, values);
  }

  inputSample.setDescription(inputNames);

  return inputSample;
}


void FixedDesignOfExperiment::updateParameters()
{
  const Description inputNames(inputNames_);
  const Point values(values_);
  const Point infBounds(lowerBounds_);
  const Point supBounds(upperBounds_);
  const Indices levels(levels_);
  const Point deltas(deltas_);

  initializeParameters();

  for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++ i)
  {
    const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
    if (it != inputNames.end())
    {
      values_[i] = values[it - inputNames.begin()];
      lowerBounds_[i] = infBounds[it - inputNames.begin()];
      upperBounds_[i] = supBounds[it - inputNames.begin()];
      levels_[i] = levels[it - inputNames.begin()];
      deltas_[i] = deltas[it - inputNames.begin()];
    }
  }
}


Point FixedDesignOfExperiment::getValues() const
{
  return values_;
}


void FixedDesignOfExperiment::setValues(const Point & values)
{
  if (values.getSize() != physicalModel_.getInputs().getSize())
  {
    OSS oss;
    oss << "FixedDesignOfExperiment::setValues : The dimension of the list of the values has to be equal to the number of inputs of the physical model: ";
    oss << physicalModel_.getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  values_ = values;

  // clear samples
  originalInputSample_.clear();
  initialize();
}


Point FixedDesignOfExperiment::getLowerBounds() const
{
  return lowerBounds_;
}


void FixedDesignOfExperiment::setLowerBounds(const Point & lowerBounds)
{
  if (lowerBounds.getSize() != physicalModel_.getInputs().getSize())
  {
    OSS oss;
    oss << "FixedDesignOfExperiment::setLowerBounds : The dimension of the list of the lower bounds has to be equal to the number of inputs of the physical model: ";
    oss << physicalModel_.getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  lowerBounds_ = lowerBounds;

  // clear samples
  originalInputSample_.clear();
  initialize();
}


Point FixedDesignOfExperiment::getUpperBounds() const
{
  return upperBounds_;
}


void FixedDesignOfExperiment::setUpperBounds(const Point & upperBounds)
{
  if (upperBounds.getSize() != physicalModel_.getInputs().getSize())
  {
    OSS oss;
    oss << "FixedDesignOfExperiment::setUpperBounds : The dimension of the list of the upper bounds has to be equal to the number of inputs of the physical model: ";
    oss << physicalModel_.getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  upperBounds_ = upperBounds;

  // clear samples
  originalInputSample_.clear();
  initialize();
}


Indices FixedDesignOfExperiment::getLevels() const
{
  return levels_;
}


void FixedDesignOfExperiment::setLevels(const Indices & levels)
{
  if (levels.getSize() != physicalModel_.getInputs().getSize())
  {
    OSS oss;
    oss << "FixedDesignOfExperiment::setLevels : The dimension of the list of the levels has to be equal to the number of inputs of the physical model: ";
    oss << physicalModel_.getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  Point deltas(physicalModel_.getInputs().getSize());
  for (UnsignedInteger i = 0; i < levels.getSize(); ++i)
  {
    if (levels[i] == 1)
      deltas[i] = 0;
    else
    {
      if (upperBounds_.getSize() != physicalModel_.getInputs().getSize() ||
          lowerBounds_.getSize() != physicalModel_.getInputs().getSize())
        throw InvalidValueException(HERE) << "Set the upper bounds and the lower bounds before this step";
      deltas[i] = (upperBounds_[i] - lowerBounds_[i]) / (levels[i] - 1);
    }
  }

  type_ = FixedDesignOfExperiment::FromBoundsAndLevels;
  levels_ = levels;
  deltas_ = deltas;

  // clear samples
  originalInputSample_.clear();
  initialize();
}


Point FixedDesignOfExperiment::getDeltas() const
{
  return deltas_;
}


void FixedDesignOfExperiment::setDeltas(const Point & deltas)
{
  if (deltas.getSize() != physicalModel_.getInputs().getSize())
  {
    OSS oss;
    oss << "FixedDesignOfExperiment::setDeltas : The dimension of the list of the deltas has to be equal to the number of inputs of the physical model: ";
    oss << physicalModel_.getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  for (UnsignedInteger i = 0; i < deltas.getSize(); ++i)
    if (deltas[i] < 0.)
      throw InvalidArgumentException(HERE) << "FixedDesignOfExperiment::setDeltas : All the deltas must be superior or equal to 0.";

  Indices levels(physicalModel_.getInputs().getSize());
  for (UnsignedInteger i = 0; i < deltas.getSize(); ++i)
  {
    if (deltas[i] > 0.)
    {
      if (upperBounds_.getSize() != physicalModel_.getInputs().getSize() ||
          lowerBounds_.getSize() != physicalModel_.getInputs().getSize())
        throw InvalidValueException(HERE) << "Set the upper bounds and the lower bounds before this step";
      levels[i] = (upperBounds_[i] - lowerBounds_[i]) / deltas[i] + 1;
    }
    else
      levels[i] = 1;
  }

  type_ = FixedDesignOfExperiment::FromBoundsAndDeltas;
  deltas_ = deltas;
  levels_ = levels;

  // clear samples
  originalInputSample_.clear();
  initialize();
}


Description FixedDesignOfExperiment::getVariableInputNames() const
{
  if (!levels_.contains(1))
    return inputNames_;

  Description variableInputsNames;
  for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++i)
    if (levels_[i] > 1)
      variableInputsNames.add(inputNames_[i]);
  return variableInputsNames;
}


String FixedDesignOfExperiment::getPythonScript() const
{
  OSS oss;

  oss << "values = [";
  for (UnsignedInteger i = 0; i < values_.getSize(); ++ i)
  {
    oss << values_[i];
    if (i < values_.getSize()-1)
      oss << ", ";
  }
  oss << "]\n";
  oss << "lowerBounds = [";
  for (UnsignedInteger i = 0; i < lowerBounds_.getSize(); ++ i)
  {
    oss << lowerBounds_[i];
    if (i < lowerBounds_.getSize()-1)
      oss << ", ";
  }
  oss << "]\n";
  oss << "upperBounds = [";
  for (UnsignedInteger i = 0; i < upperBounds_.getSize(); ++ i)
  {
    oss << upperBounds_[i];
    if (i < upperBounds_.getSize()-1)
      oss << ", ";
  }
  oss << "]\n";
  oss << "levels = [";
  for (UnsignedInteger i = 0; i < levels_.getSize(); ++ i)
  {
    oss << levels_[i];
    if (i < levels_.getSize()-1)
      oss << ", ";
  }
  oss << "]\n";

  oss << getName() << " = otguibase.FixedDesignOfExperiment('" << getName() << "', " << getPhysicalModel().getName() << ", ";
  oss << "lowerBounds, upperBounds, levels, values)\n";

  return oss;
}


/* String converter */
String FixedDesignOfExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " physicalModel=" << getPhysicalModel().getName()
      << " deltas=" << getDeltas()
      << " levels=" << getLevels()
      << " lowerBounds=" << getLowerBounds()
      << " upperBounds=" << getUpperBounds()
      << " values=" << getValues();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void FixedDesignOfExperiment::save(Advocate & adv) const
{
  DesignOfExperimentImplementation::save(adv);
  adv.saveAttribute("deltas_", deltas_);
  adv.saveAttribute("levels_", levels_);
  adv.saveAttribute("lowerBounds_", lowerBounds_);
  adv.saveAttribute("upperBounds_", upperBounds_);
  adv.saveAttribute("values_", values_);
}


/* Method load() reloads the object from the StorageManager */
void FixedDesignOfExperiment::load(Advocate & adv)
{
  DesignOfExperimentImplementation::load(adv);
  adv.loadAttribute("deltas_", deltas_);
  adv.loadAttribute("levels_", levels_);
  adv.loadAttribute("lowerBounds_", lowerBounds_);
  adv.loadAttribute("upperBounds_", upperBounds_);
  adv.loadAttribute("values_", values_);
  inputNames_ = getPhysicalModel().getInputNames();
}
}
