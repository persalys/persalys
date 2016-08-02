//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#include "otgui/FixedDesignOfExperiment.hxx"

#include "openturns/Box.hxx"
#include "openturns/TruncatedDistribution.hxx"
#include "openturns/TruncatedNormal.hxx"
#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(FixedDesignOfExperiment);

static Factory<FixedDesignOfExperiment> RegisteredFactory;

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
                                                  const NumericalPoint & lowerBounds,
                                                  const NumericalPoint & upperBounds,
                                                  const Indices & levels,
                                                  const OT::NumericalPoint & values)
  : DesignOfExperimentImplementation(name, physicalModel)
  , type_(FixedDesignOfExperiment::FromBoundsAndLevels)
  , inputNames_(physicalModel.getInputNames())
  , values_(values)
  , lowerBounds_(lowerBounds)
  , upperBounds_(upperBounds)
  , deltas_(NumericalPoint(physicalModel.getInputs().getSize()))
{
  setLevels(levels);
  if (!values_.getSize())
  {
    values_ = NumericalPoint(physicalModel.getInputs().getSize());
    for (UnsignedInteger i=0; i<physicalModel.getInputs().getSize(); ++i)
      values_[i] = physicalModel.getInputs()[i].getValue();
  }
//   TODO: check if lowerBounds[i] <= upperBounds[i]
}


/* Virtual constructor */
FixedDesignOfExperiment* FixedDesignOfExperiment::clone() const
{
  return new FixedDesignOfExperiment(*this);
}


FixedDesignOfExperiment::Type FixedDesignOfExperiment::getTypeDesignOfExperiment() const
{
  return type_;
}


void FixedDesignOfExperiment::initializeParameters()
{
  inputSample_.clear();
  inputNames_ = getPhysicalModel().getInputNames();
  const InputCollection inputs = getPhysicalModel().getInputs();

  int inputSize = inputs.getSize();
  values_ = NumericalPoint(inputSize);
  lowerBounds_ = NumericalPoint(inputSize);
  upperBounds_ = NumericalPoint(inputSize);
  levels_ = Indices(inputSize);
  deltas_ = NumericalPoint(inputSize);

  for (int i=0; i<inputSize; ++i)
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
      Distribution distribution = inputs[i].getDistribution();
      String distributionName = distribution.getImplementation()->getClassName();
      
      if (distributionName == "TruncatedDistribution")
      {
        TruncatedDistribution truncatedDist = *dynamic_cast<TruncatedDistribution*>(&*distribution.getImplementation());
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
        TruncatedNormal truncatedDist = *dynamic_cast<TruncatedNormal*>(&*distribution.getImplementation());
        lowerBounds_[i] = truncatedDist.getA();
        upperBounds_[i] = truncatedDist.getB();
      }
      else
      {
        lowerBounds_[i] = distribution.computeQuantile(0.05)[0];
        upperBounds_[i] = distribution.computeQuantile(0.95)[0];
      }
    }
    levels_[i] = 2;
    deltas_[i] = upperBounds_[i] - lowerBounds_[i];
  }
}


void FixedDesignOfExperiment::updateParameters()
{
  Description inputNames(inputNames_);
  NumericalPoint values(values_);
  NumericalPoint infBounds(lowerBounds_);
  NumericalPoint supBounds(upperBounds_);
  Indices levels(levels_);
  NumericalPoint deltas(deltas_);

  initializeParameters();

  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++ i)
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


NumericalPoint FixedDesignOfExperiment::getValues() const
{
  return values_;
}


void FixedDesignOfExperiment::setValues(const NumericalPoint & values)
{
  values_ = values;
}


NumericalPoint FixedDesignOfExperiment::getLowerBounds() const
{
  return lowerBounds_;
}


void FixedDesignOfExperiment::setLowerBounds(const NumericalPoint & lowerBounds)
{
  lowerBounds_ = lowerBounds;
}


NumericalPoint FixedDesignOfExperiment::getUpperBounds() const
{
  return upperBounds_;
}


void FixedDesignOfExperiment::setUpperBounds(const NumericalPoint & upperBounds)
{
  upperBounds_ = upperBounds;
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

  type_ = FixedDesignOfExperiment::FromBoundsAndLevels;
  levels_ = levels;
  for (UnsignedInteger i=0; i<levels_.getSize(); ++i)
  {
    if (levels_[i] == 1)
      deltas_[i] = 0;
    else
      deltas_[i] = (upperBounds_[i] - lowerBounds_[i])/(levels_[i]-1);
  }
}


NumericalPoint FixedDesignOfExperiment::getDeltas() const
{
  return deltas_;
}


void FixedDesignOfExperiment::setDeltas(const NumericalPoint & deltas)
{
  if (deltas.getSize() != physicalModel_.getInputs().getSize())
  {
    OSS oss;
    oss << "FixedDesignOfExperiment::setDeltas : The dimension of the list of the deltas has to be equal to the number of inputs of the physical model: ";
    oss << physicalModel_.getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  for (UnsignedInteger i=0; i<deltas.getSize(); ++i)
    if (deltas[i] < 0.)
      throw InvalidArgumentException(HERE) << "FixedDesignOfExperiment::setDeltas : All the deltas must be superior or equal to 0.";

  type_ = FixedDesignOfExperiment::FromBoundsAndDeltas;
  deltas_ = deltas;
  for (UnsignedInteger i=0; i<deltas_.getSize(); ++i)
  {
    if (deltas_[i] > 0)
      levels_[i] = (upperBounds_[i] - lowerBounds_[i])/deltas_[i] + 1;
    else
      levels_[i] = 1.;
  }
}


NumericalSample FixedDesignOfExperiment::getInputSample()
{
  if (!inputSample_.getSize())
  {
    inputSample_.clear();
    NumericalPoint scale;
    NumericalPoint transvec;
    NumericalPoint otLevels;
    Description variableInputsNames;

    for (UnsignedInteger i=0; i<inputNames_.getSize(); ++i)
    {
      if (levels_[i] > 1)
      {
        double inf = lowerBounds_[i];
        double sup = upperBounds_[i];
        scale.add(sup - inf);
        transvec.add(inf);
        variableInputsNames.add(inputNames_[i]);
        otLevels.add(levels_[i]-2);
      }
    }

    if (otLevels.getSize())
    {
      NumericalSample sample = Box(otLevels).generate();
      sample *= scale;
      sample += transvec;

      if (sample.getDimension() == inputNames_.getSize())
        inputSample_ = sample;
      else
      {
        inputSample_ = NumericalSample(sample.getSize(), inputNames_.getSize());
        for (UnsignedInteger i=0; i<inputNames_.getSize(); ++i)
        {
          if (levels_[i] == 1)
            for (UnsignedInteger j=0; j<sample.getSize(); ++j)
              inputSample_[j][i] = values_[i];
          else
          {
            const Description::const_iterator it = std::find(variableInputsNames.begin(), variableInputsNames.end(), inputNames_[i]);
            if (it != variableInputsNames.end())
              for (UnsignedInteger j=0; j<sample.getSize(); ++j)
                inputSample_[j][i] = sample[j][it - variableInputsNames.begin()];
          }
        }
      }
    }
    else
      inputSample_ = NumericalSample(1, values_);
    inputSample_.setDescription(inputNames_);
  }
  return inputSample_;
}


Description FixedDesignOfExperiment::getVariableInputNames() const
{
  if (!levels_.contains(1))
    return inputNames_;

  Description variableInputsNames;
  for (UnsignedInteger i=0; i<inputNames_.getSize(); ++i)
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

  oss << getName()+ " = otguibase.FixedDesignOfExperiment('" + getName() + "', "+getPhysicalModel().getName()+", ";
  oss << "lowerBounds, upperBounds, levels, values)\n";

  return oss.str();
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