//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#include "persalys/GridDesignOfExperiment.hxx"

#include <openturns/Box.hxx>
#include <openturns/TruncatedDistribution.hxx>
#include <openturns/TruncatedNormal.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(GridDesignOfExperiment)

static Factory<GridDesignOfExperiment> Factory_GridDesignOfExperiment;

/* Default constructor */
GridDesignOfExperiment::GridDesignOfExperiment()
  : DesignOfExperimentEvaluation()
  , inputNames_()
  , values_()
  , type_(GridDesignOfExperiment::FromBoundsAndLevels)
  , bounds_()
  , levels_()
  , deltas_()
{
}


/* Constructor with parameters */
GridDesignOfExperiment::GridDesignOfExperiment(const String& name, const PhysicalModel& physicalModel)
  : DesignOfExperimentEvaluation(name, physicalModel)
  , inputNames_()
  , values_()
  , type_(GridDesignOfExperiment::FromBoundsAndLevels)
  , bounds_()
  , levels_()
  , deltas_()
{
  initializeParameters();
}


/* Constructor with parameters */
GridDesignOfExperiment::GridDesignOfExperiment(const String& name,
    const PhysicalModel& physicalModel,
    const Interval& bounds,
    const Indices& levels,
    const Point& values)
  : DesignOfExperimentEvaluation(name, physicalModel)
  , inputNames_(physicalModel.getInputNames())
  , values_(values)
  , type_(GridDesignOfExperiment::FromBoundsAndLevels)
  , bounds_()
  , levels_()
  , deltas_()
{
  const UnsignedInteger nbInputs = physicalModel.getInputDimension();
  if (!nbInputs)
    throw InvalidArgumentException(HERE) << "The physical model does not have input variables";

  setBounds(bounds);
  setLevels(levels);

  if (!values_.getSize())
  {
    values_ = Point(nbInputs);
    for (UnsignedInteger i = 0; i < nbInputs; ++i)
      values_[i] = physicalModel.getInputs()[i].getValue();
  }
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

  const UnsignedInteger nbInputs = getPhysicalModel().getInputDimension();
  levels_ = Indices(nbInputs, 1);
  deltas_ = Point(nbInputs);

  Tools::ComputeBounds(getPhysicalModel().getInputs(), values_, bounds_);
}


Sample GridDesignOfExperiment::generateInputSample(const UnsignedInteger /*nbSimu*/) const
{
  const UnsignedInteger nbInputs = inputNames_.getSize();

  // check arguments size
  if (levels_.getSize() != nbInputs ||
      bounds_.getDimension() != nbInputs ||
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
      if (bounds_.getMarginal(i).isEmpty())
        throw InvalidArgumentException(HERE) << "The interval must be not empty";
      scale.add(bounds_.getMarginal(i).getVolume());
      transvec.add(bounds_.getMarginal(i).getLowerBound()[0]);
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
          inputSample(j, variableInputsIndices[i]) = sample(j, i);
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
  const Point lowerBounds(bounds_.getLowerBound());
  const Point upperBounds(bounds_.getUpperBound());
  const Indices levels(levels_);
  const Point deltas(deltas_);

  initializeParameters();

  Point newLowerBounds(bounds_.getLowerBound());
  Point newUpperBounds(bounds_.getUpperBound());

  for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++ i)
  {
    const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
    if (it != inputNames.end())
    {
      values_[i] = values[it - inputNames.begin()];
      newLowerBounds[i] = lowerBounds[it - inputNames.begin()];
      newUpperBounds[i] = upperBounds[it - inputNames.begin()];
      levels_[i] = levels[it - inputNames.begin()];
      deltas_[i] = deltas[it - inputNames.begin()];
    }
  }
  bounds_.setLowerBound(newLowerBounds);
  bounds_.setUpperBound(newUpperBounds);
}


Point GridDesignOfExperiment::getValues() const
{
  return values_;
}


void GridDesignOfExperiment::setValues(const Point & values)
{
  if (values.getSize() != getPhysicalModel().getInputDimension())
  {
    OSS oss;
    oss << "GridDesignOfExperiment::setValues : The dimension of the list of the values has to be equal to the number of inputs of the physical model: ";
    oss << getPhysicalModel().getInputDimension();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  values_ = values;

  // clear sample
  originalInputSample_.clear();
}


Interval GridDesignOfExperiment::getBounds() const
{
  return bounds_;
}


void GridDesignOfExperiment::setBounds(const Interval & bounds)
{
  bounds_ = bounds;
  // clear sample
  originalInputSample_.clear();
}


Indices GridDesignOfExperiment::getLevels() const
{
  return levels_;
}


void GridDesignOfExperiment::setLevels(const Indices & levels)
{
  if (levels.getSize() != getPhysicalModel().getInputDimension())
  {
    OSS oss;
    oss << "GridDesignOfExperiment::setLevels : The dimension of the list of the levels has to be equal to the number of inputs of the physical model: ";
    oss << getPhysicalModel().getInputDimension();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  Point deltas(getPhysicalModel().getInputDimension());
  for (UnsignedInteger i = 0; i < levels.getSize(); ++i)
  {
    if (levels[i] == 1)
      deltas[i] = 0;
    else
    {
      if (bounds_.getDimension() != getPhysicalModel().getInputDimension())
        throw InvalidValueException(HERE) << "Define the bounds for each input variable";
      deltas[i] = (bounds_.getMarginal(i).getVolume()) / (levels[i] - 1);
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
  if (deltas.getSize() != getPhysicalModel().getInputDimension())
  {
    OSS oss;
    oss << "GridDesignOfExperiment::setDeltas : The dimension of the list of the deltas has to be equal to the number of inputs of the physical model: ";
    oss << getPhysicalModel().getInputDimension();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  for (UnsignedInteger i = 0; i < deltas.getSize(); ++i)
    if (deltas[i] < 0. || deltas[i] > bounds_.getMarginal(i).getVolume())
      throw InvalidArgumentException(HERE) << "GridDesignOfExperiment::setDeltas : All the deltas must be greater or equal to 0 and less than the interval length.";

  Indices levels(getPhysicalModel().getInputDimension());
  for (UnsignedInteger i = 0; i < deltas.getSize(); ++i)
  {
    if (deltas[i] > 0.)
    {
      if (bounds_.getDimension() != getPhysicalModel().getInputDimension())
        throw InvalidValueException(HERE) << "Define the bounds for each input variable";
      levels[i] = (bounds_.getMarginal(i).getVolume()) / deltas[i] + 1;
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


Parameters GridDesignOfExperiment::getParameters() const
{
  Parameters param;

  param.add("Design type", "Grid");
  param.add("Outputs of interest", getInterestVariables().__str__());
  param.add("Sample size", getOriginalInputSample().getSize());

  Description allInputsBoundsStr = Parameters::GetOTIntervalDescription(getBounds());
  OSS bounds;
  for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++i)
  {
    if (levels_[i] > 1)
      bounds << inputNames_[i] << " : " << allInputsBoundsStr[i] << "levels" << " = " << levels_[i];
    else
      bounds << inputNames_[i] << " : " << values_[i];
    if (i < inputNames_.getSize() - 1)
      bounds << "\n";
  }
  param.add("Bounds", bounds);

  param.add("Block size", getBlockSize());

  return param;
}


String GridDesignOfExperiment::getPythonScript() const
{
  OSS oss;

  oss << "values = " << values_.__str__() << "\n";
  oss << "bounds = ot.Interval(" << getBounds().getLowerBound().__str__() << ", " << getBounds().getUpperBound().__str__() << ")\n";
  oss << "levels = " << levels_.__str__() << "\n";

  oss << getName() << " = persalys.GridDesignOfExperiment('" << getName() << "', " << getPhysicalModel().getName() << ", ";
  oss << "bounds, levels, values)\n";

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
      << " bounds=" << getBounds()
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
  adv.saveAttribute("bounds_", bounds_);
  adv.saveAttribute("values_", values_);
  adv.saveAttribute("inputNames_", inputNames_);
}


/* Method load() reloads the object from the StorageManager */
void GridDesignOfExperiment::load(Advocate & adv)
{
  DesignOfExperimentEvaluation::load(adv);
  adv.loadAttribute("deltas_", deltas_);
  adv.loadAttribute("levels_", levels_);
  adv.loadAttribute("bounds_", bounds_);
  if (!bounds_.getDimension())
  {
    Point lowerBounds;
    Point upperBounds;
    adv.loadAttribute("lowerBounds_", lowerBounds);
    adv.loadAttribute("upperBounds_", upperBounds);
    bounds_ = Interval(lowerBounds, upperBounds);
  }
  adv.loadAttribute("values_", values_);
  adv.loadAttribute("inputNames_", inputNames_);
}
}
