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
#include "otgui/GridDesignOfExperiment.hxx"

#include <openturns/Box.hxx>
#include <openturns/TruncatedDistribution.hxx>
#include <openturns/TruncatedNormal.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(GridDesignOfExperiment)

static Factory<GridDesignOfExperiment> Factory_GridDesignOfExperiment;

/* Default constructor */
GridDesignOfExperiment::GridDesignOfExperiment()
  : DesignOfExperimentEvaluation()
  , type_(GridDesignOfExperiment::FromBoundsAndLevels)
{
}


/* Constructor with parameters */
GridDesignOfExperiment::GridDesignOfExperiment(const String& name, const PhysicalModel& physicalModel)
  : DesignOfExperimentEvaluation(name, physicalModel)
  , type_(GridDesignOfExperiment::FromBoundsAndLevels)
{
  initializeParameters();
}


/* Constructor with parameters */
GridDesignOfExperiment::GridDesignOfExperiment(const String& name,
    const PhysicalModel& physicalModel,
    const Point& lowerBounds,
    const Point& upperBounds,
    const Indices& levels,
    const Point& values)
  : DesignOfExperimentEvaluation(name, physicalModel)
  , type_(GridDesignOfExperiment::FromBoundsAndLevels)
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
GridDesignOfExperiment* GridDesignOfExperiment::clone() const
{
  return new GridDesignOfExperiment(*this);
}


GridDesignOfExperiment::Type GridDesignOfExperiment::getTypeDesignOfExperiment() const
{
  return type_;
}


void GridDesignOfExperiment::initializeParameters()
{
  // clear sample
  originalInputSample_.clear();

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

      // lower bound
      if (distribution.getRange().getFiniteLowerBound()[0])
        lowerBounds_[i] = distribution.getRange().getLowerBound()[0];
      else
        lowerBounds_[i] = distribution.computeQuantile(0.05)[0];
      // upper bound
      if (distribution.getRange().getFiniteUpperBound()[0])
        upperBounds_[i] = distribution.getRange().getUpperBound()[0];
      else
        upperBounds_[i] = distribution.computeQuantile(0.95)[0];
    }
  }
}


Sample GridDesignOfExperiment::generateInputSample(const UnsignedInteger /*nbSimu*/) const
{
  const UnsignedInteger nbInputs = inputNames_.getSize();

  // check arguments size
  if (levels_.getSize() != nbInputs ||
      lowerBounds_.getSize() != nbInputs ||
      upperBounds_.getSize() != nbInputs ||
      values_.getSize() != nbInputs)
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
    if (levels_[i] > 1)
    {
      const double inf = lowerBounds_[i];
      const double sup = upperBounds_[i];
      scale.add(sup - inf);
      transvec.add(inf);
      variableInputsIndices.add(i);
      otLevels.add(levels_[i] - 2);
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
      throw InvalidValueException(HERE) << "Impossible to generate the design of experiments\n " << ex.what();
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
      inputSample = Sample(sample.getSize(), values_);
      for (UnsignedInteger j = 0; j < sample.getSize(); ++j)
      {
        for (UnsignedInteger i = 0; i < variableInputsIndices.getSize(); ++i)
        {
          inputSample[j][variableInputsIndices[i]] = sample[j][i];
        }
      }
    }
  }
  // if all the inputs are constant
  else
  {
    inputSample = Sample(1, values_);
  }

  inputSample.setDescription(inputNames_);

  return inputSample;
}


void GridDesignOfExperiment::updateParameters()
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


Point GridDesignOfExperiment::getValues() const
{
  return values_;
}


void GridDesignOfExperiment::setValues(const Point & values)
{
  if (values.getSize() != getPhysicalModel().getInputs().getSize())
  {
    OSS oss;
    oss << "GridDesignOfExperiment::setValues : The dimension of the list of the values has to be equal to the number of inputs of the physical model: ";
    oss << getPhysicalModel().getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  values_ = values;

  // clear sample
  originalInputSample_.clear();
}


Point GridDesignOfExperiment::getLowerBounds() const
{
  return lowerBounds_;
}


void GridDesignOfExperiment::setLowerBounds(const Point & lowerBounds)
{
  if (lowerBounds.getSize() != getPhysicalModel().getInputs().getSize())
  {
    OSS oss;
    oss << "GridDesignOfExperiment::setLowerBounds : The dimension of the list of the lower bounds has to be equal to the number of inputs of the physical model: ";
    oss << getPhysicalModel().getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  lowerBounds_ = lowerBounds;

  // clear sample
  originalInputSample_.clear();
}


Point GridDesignOfExperiment::getUpperBounds() const
{
  return upperBounds_;
}


void GridDesignOfExperiment::setUpperBounds(const Point & upperBounds)
{
  if (upperBounds.getSize() != getPhysicalModel().getInputs().getSize())
  {
    OSS oss;
    oss << "GridDesignOfExperiment::setUpperBounds : The dimension of the list of the upper bounds has to be equal to the number of inputs of the physical model: ";
    oss << getPhysicalModel().getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  upperBounds_ = upperBounds;

  // clear sample
  originalInputSample_.clear();
}


Indices GridDesignOfExperiment::getLevels() const
{
  return levels_;
}


void GridDesignOfExperiment::setLevels(const Indices & levels)
{
  if (levels.getSize() != getPhysicalModel().getInputs().getSize())
  {
    OSS oss;
    oss << "GridDesignOfExperiment::setLevels : The dimension of the list of the levels has to be equal to the number of inputs of the physical model: ";
    oss << getPhysicalModel().getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  Point deltas(getPhysicalModel().getInputs().getSize());
  for (UnsignedInteger i = 0; i < levels.getSize(); ++i)
  {
    if (levels[i] == 1)
      deltas[i] = 0;
    else
    {
      if (upperBounds_.getSize() != getPhysicalModel().getInputs().getSize() ||
          lowerBounds_.getSize() != getPhysicalModel().getInputs().getSize())
        throw InvalidValueException(HERE) << "Set the upper bounds and the lower bounds before this step";
      deltas[i] = (upperBounds_[i] - lowerBounds_[i]) / (levels[i] - 1);
    }
  }

  type_ = GridDesignOfExperiment::FromBoundsAndLevels;
  levels_ = levels;
  deltas_ = deltas;

  // clear sample
  originalInputSample_.clear();
}


Point GridDesignOfExperiment::getDeltas() const
{
  return deltas_;
}


void GridDesignOfExperiment::setDeltas(const Point & deltas)
{
  if (deltas.getSize() != getPhysicalModel().getInputs().getSize())
  {
    OSS oss;
    oss << "GridDesignOfExperiment::setDeltas : The dimension of the list of the deltas has to be equal to the number of inputs of the physical model: ";
    oss << getPhysicalModel().getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  for (UnsignedInteger i = 0; i < deltas.getSize(); ++i)
    if (deltas[i] < 0.)
      throw InvalidArgumentException(HERE) << "GridDesignOfExperiment::setDeltas : All the deltas must be superior or equal to 0.";

  Indices levels(getPhysicalModel().getInputs().getSize());
  for (UnsignedInteger i = 0; i < deltas.getSize(); ++i)
  {
    if (deltas[i] > 0.)
    {
      if (upperBounds_.getSize() != getPhysicalModel().getInputs().getSize() ||
          lowerBounds_.getSize() != getPhysicalModel().getInputs().getSize())
        throw InvalidValueException(HERE) << "Set the upper bounds and the lower bounds before this step";
      levels[i] = (upperBounds_[i] - lowerBounds_[i]) / deltas[i] + 1;
    }
    else
      levels[i] = 1;
  }

  type_ = GridDesignOfExperiment::FromBoundsAndDeltas;
  deltas_ = deltas;
  levels_ = levels;

  // clear sample
  originalInputSample_.clear();
}


Description GridDesignOfExperiment::getVariableInputNames() const
{
  if (!levels_.contains(1))
    return inputNames_;

  Description variableInputsNames;
  for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++i)
    if (levels_[i] > 1)
      variableInputsNames.add(inputNames_[i]);
  return variableInputsNames;
}


String GridDesignOfExperiment::getPythonScript() const
{
  OSS oss;

  oss << "values = " << values_.__str__() << "\n";
  oss << "lowerBounds = " << lowerBounds_.__str__() << "\n";
  oss << "upperBounds = " << upperBounds_.__str__() << "\n";
  oss << "levels = " << levels_.__str__() << "\n";

  oss << getName() << " = otguibase.GridDesignOfExperiment('" << getName() << "', " << getPhysicalModel().getName() << ", ";
  oss << "lowerBounds, upperBounds, levels, values)\n";

  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
  oss << getName() << ".setInterestVariables(interestVariables)\n";

  return oss;
}


/* String converter */
String GridDesignOfExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " physicalModel=" << getPhysicalModel().getName()
      << " deltas=" << getDeltas()
      << " levels=" << getLevels()
      << " lowerBounds=" << getLowerBounds()
      << " upperBounds=" << getUpperBounds()
      << " values=" << getValues()
      << " blockSize=" << getBlockSize();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void GridDesignOfExperiment::save(Advocate & adv) const
{
  DesignOfExperimentEvaluation::save(adv);
  adv.saveAttribute("deltas_", deltas_);
  adv.saveAttribute("levels_", levels_);
  adv.saveAttribute("lowerBounds_", lowerBounds_);
  adv.saveAttribute("upperBounds_", upperBounds_);
  adv.saveAttribute("values_", values_);
}


/* Method load() reloads the object from the StorageManager */
void GridDesignOfExperiment::load(Advocate & adv)
{
  DesignOfExperimentEvaluation::load(adv);
  adv.loadAttribute("deltas_", deltas_);
  adv.loadAttribute("levels_", levels_);
  adv.loadAttribute("lowerBounds_", lowerBounds_);
  adv.loadAttribute("upperBounds_", upperBounds_);
  adv.loadAttribute("values_", values_);
  inputNames_ = getPhysicalModel().getInputNames();
}
}
