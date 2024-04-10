//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
{
}


/* Constructor with parameters */
GridDesignOfExperiment::GridDesignOfExperiment(const String &name, const PhysicalModel &physicalModel)
  : DesignOfExperimentEvaluation(name, physicalModel)
  , inputNames_()
  , values_()
{
  initializeParameters();
}


/* Constructor with parameters */
GridDesignOfExperiment::GridDesignOfExperiment(const String &name,
    const PhysicalModel &physicalModel,
    const Collection<Point> &values)
  : DesignOfExperimentEvaluation(name, physicalModel)
  , inputNames_(physicalModel.getInputNames())
  , values_()
{
  const UnsignedInteger nbInputs = physicalModel.getInputDimension();
  if (!nbInputs)
    throw InvalidArgumentException(HERE) << "The physical model does not have input variables";

  setValues(values);
}


/* Virtual constructor */
GridDesignOfExperiment* GridDesignOfExperiment::clone() const
{
  return new GridDesignOfExperiment(*this);
}


void GridDesignOfExperiment::initializeParameters()
{
  // clear sample
  originalInputSample_.clear();

  inputNames_ = getPhysicalModel().getInputNames();

  const UnsignedInteger nbInputs = getPhysicalModel().getInputDimension();

  values_ = Collection<Point>(nbInputs);
  for (UnsignedInteger i = 0; i < nbInputs; ++i)
    values_[i] = Point(1, getPhysicalModel().getInputs()[i].getValue());
}


Sample GridDesignOfExperiment::generateInputSample(const UnsignedInteger /*nbSimu*/) const
{
  const UnsignedInteger nbInputs = inputNames_.getSize();
  if (nbInputs != getPhysicalModel().getInputDimension())
    throw InvalidArgumentException(HERE) << "Wrong dimension of the input name list";
  if (values_.getSize() != nbInputs)
    throw InvalidArgumentException(HERE) << "Wrong values list dimension";

  UnsignedInteger nbPts = 1;
  for (UnsignedInteger i = 0; i < nbInputs; ++i)
    nbPts *= values_[i].getSize();

  if (!nbPts)
    throw InvalidArgumentException(HERE) << "Input sample size null";

  // build input sample
  Sample inputSample(nbPts, nbInputs);

  UnsignedInteger prec = nbPts;
  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    const UnsignedInteger nb_i = values_[i].getSize();
    for (UnsignedInteger j = 0; j < nbPts; j++)
    {
      const UnsignedInteger indice = (j % prec) / (prec / nb_i);
      inputSample(j, i) = values_[i][indice];
    }
    prec = prec / nb_i;
  }

  inputSample.setDescription(inputNames_);

  return inputSample;
}


void GridDesignOfExperiment::updateParameters()
{
  const Description inputNames(inputNames_);

  const Collection<Point> values(values_);

  initializeParameters();

  for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++ i)
  {
    const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
    if (it != inputNames.end())
      values_[i] = values[it - inputNames.begin()];
  }
}


Collection<Point> GridDesignOfExperiment::getValues() const
{
  return values_;
}


void GridDesignOfExperiment::setValues(const Collection<Point> &values)
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


Interval GridDesignOfExperiment::GetDefaultBounds(const PhysicalModel &model)
{
  const UnsignedInteger nbInputs = model.getInputDimension();
  Point fixValues(nbInputs);
  Interval interval(nbInputs);
  Tools::ComputeBounds(model.getInputs(), fixValues, interval);
  return interval;
}


Parameters GridDesignOfExperiment::getParameters() const
{
  Parameters param;

  param.add("Design type", "Grid");
  param.add("Outputs of interest", getInterestVariables().__str__());
  param.add("Sample size", getOriginalInputSample().getSize());

  OSS values;
  if (inputNames_.getSize() == values_.getSize())
  {
    for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++i)
    {
      Point values_i(values_[i]);
      if (values_[i].getSize() > 1)
      {
        std::pair<Collection<Scalar>::iterator, Collection<Scalar>::iterator> p = std::minmax_element(values_i.begin(), values_i.end());
        values << inputNames_[i] << " : [" << (*p.first) << ", " << (*p.second) << "]";
        values << " levels" << " = " << values_i.getSize();
      }
      else
        values << inputNames_[i] << " : " << values_i[0];
      if (i < inputNames_.getSize() - 1)
        values << "\n";
    }
  }
  param.add("Values", values);

  param.add("Block size", getBlockSize());

  return param;
}


String GridDesignOfExperiment::getPythonScript() const
{
  OSS oss;
  oss << "values = [";
  for (UnsignedInteger i = 0; i < values_.getSize(); ++i)
    oss << Parameters::GetOTPointStr(values_[i]) << (i < values_.getSize()-1 ? ",\n" : "]\n");
  oss << getName() << " = persalys.GridDesignOfExperiment('" << getName() << "', " << getPhysicalModel().getName() << ", values)\n";
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
      << " values=" << getValues()
      << " blockSize=" << getBlockSize();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void GridDesignOfExperiment::save(Advocate & adv) const
{
  DesignOfExperimentEvaluation::save(adv);
  adv.saveAttribute("values_", values_);
  adv.saveAttribute("inputNames_", inputNames_);
}


/* Method load() reloads the object from the StorageManager */
void GridDesignOfExperiment::load(Advocate & adv)
{
  DesignOfExperimentEvaluation::load(adv);
  // can load old file
  Indices levels;
  adv.loadAttribute("levels_", levels);
  if (levels.getSize())
  {
    Interval bounds;
    adv.loadAttribute("bounds_", bounds);
    if (bounds.getDimension() != levels.getSize())
    {
      Point lowerBounds;
      Point upperBounds;
      adv.loadAttribute("lowerBounds_", lowerBounds);
      adv.loadAttribute("upperBounds_", upperBounds);
      bounds = Interval(lowerBounds, upperBounds);
    }
    Point values;
    adv.loadAttribute("values_", values);
    values_ = Collection<Point>(levels.getSize());
    for (UnsignedInteger i = 0; i < levels.getSize(); ++i)
    {
      if (levels[i] < 2)
        values_[i] = Point(1, values[i]);
      else
        values_[i] = Box(Indices(1, levels[i] - 2), bounds.getMarginal(i)).generate().asPoint();
    }
  }
  else
  {
    adv.loadAttribute("values_", values_);
  }
  if (!values_.getSize() && getOriginalInputSample().getSize())
  {
    for (UnsignedInteger i = 0; i < getOriginalInputSample().getDimension(); ++i)
      values_.add(getOriginalInputSample().getMarginal(i).sortUnique().asPoint());
  }
  adv.loadAttribute("inputNames_", inputNames_);
}
}
