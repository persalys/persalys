//                                               -*- C++ -*-
/**
 *  @brief MorrisAnalysis
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/MorrisAnalysis.hxx"

#include <openturns/RandomGenerator.hxx>
#include <openturns/PersistentObjectFactory.hxx>

#include <otmorris/Morris.hxx>
#include <otmorris/MorrisExperimentGrid.hxx>

#include <boost/algorithm/minmax_element.hpp>

using namespace OT;
using namespace OTMORRIS;

namespace OTGUI
{

CLASSNAMEINIT(MorrisAnalysis)

static Factory<MorrisAnalysis> Factory_MorrisAnalysis;

/* Default constructor */
MorrisAnalysis::MorrisAnalysis()
  : SimulationAnalysis()
  , trajectoriesNumber_(10)
  , inputNames_()
  , bounds_()
  , level_(5)
  , result_()
{
}


/* Constructor with parameters */
MorrisAnalysis::MorrisAnalysis(const String& name, const PhysicalModel& physicalModel)
  : SimulationAnalysis(name, physicalModel)
  , trajectoriesNumber_(10)
  , inputNames_()
  , bounds_()
  , level_(5)
  , result_()
{
  initializeParameters();
}


/* Virtual constructor */
MorrisAnalysis* MorrisAnalysis::clone() const
{
  MorrisAnalysis * newAnalysis = new MorrisAnalysis(*this);
  newAnalysis->designOfExperiment_ = designOfExperiment_.getImplementation()->clone();
  return newAnalysis;
}


void MorrisAnalysis::initializeParameters()
{
  inputNames_ = getPhysicalModel().getInputNames();
  const UnsignedInteger nbInputs = inputNames_.getSize();
  bounds_ = Interval(nbInputs);

  Point lowerBounds(nbInputs, 0.9);
  Point upperBounds(nbInputs, 1.1);

  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    const Input input_i = getPhysicalModel().getInputs()[i];
    if (!input_i.isStochastic())
    {
      const Scalar value = input_i.getValue();
      if (value != 0)
      {
        lowerBounds[i] = 0.9 * value;
        upperBounds[i] = 1.1 * value;
      }
    }
    else
    {
      const Distribution distribution = input_i.getDistribution();

      // lower bound
      if (distribution.getRange().getFiniteLowerBound()[0])
        lowerBounds[i] = distribution.getRange().getLowerBound()[0];
      else
        lowerBounds[i] = distribution.computeQuantile(0.01)[0];
      // upper bound
      if (distribution.getRange().getFiniteUpperBound()[0])
        upperBounds[i] = distribution.getRange().getUpperBound()[0];
      else
        upperBounds[i] = distribution.computeQuantile(0.99)[0];
    }
  }
  bounds_.setLowerBound(lowerBounds);
  bounds_.setUpperBound(upperBounds);
}


void MorrisAnalysis::updateParameters()
{
  const Description inputNames(inputNames_);
  const Point lowBounds(bounds_.getLowerBound());
  const Point upBounds(bounds_.getUpperBound());

  initializeParameters();

  Point newLowerBounds(inputNames_.getSize());
  Point newUpperBounds(inputNames_.getSize());

  for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++ i)
  {
    const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
    if (it != inputNames.end())
    {
      newLowerBounds[i] = lowBounds[it - inputNames.begin()];
      newUpperBounds[i] = upBounds[it - inputNames.begin()];
    }
  }
  bounds_.setLowerBound(newLowerBounds);
  bounds_.setUpperBound(newUpperBounds);
}


UnsignedInteger MorrisAnalysis::getTrajectoriesNumber() const
{
  return trajectoriesNumber_;
}


void MorrisAnalysis::setTrajectoriesNumber(const UnsignedInteger number)
{
  trajectoriesNumber_ = number;
}


Interval MorrisAnalysis::getBounds() const
{
  return bounds_;
}


void MorrisAnalysis::setBounds(const Interval & bounds)
{
  if (bounds.getDimension() != getPhysicalModel().getInputDimension())
  {
    OSS oss;
    oss << "The dimension of the interval has to be equal to the number of inputs of the physical model: ";
    oss << getPhysicalModel().getInputDimension();
    throw InvalidArgumentException(HERE) << oss.str();
  }
  if (bounds.isEmpty())
    throw InvalidArgumentException(HERE) << "The interval has to be not empty";

  bounds_ = bounds;
}


UnsignedInteger MorrisAnalysis::getLevel() const
{
  return level_;
}


void MorrisAnalysis::setLevel(const UnsignedInteger & level)
{
  if (level < 2)
    throw InvalidArgumentException(HERE) << "Level should be at least 2; level = " << level;

  level_ = level;
}


void MorrisAnalysis::initialize()
{
  SimulationAnalysis::initialize();
  result_ = MorrisResult();
}


void MorrisAnalysis::launch()
{
  // check
  if (getPhysicalModel().getInputDimension() < 2)
    throw InvalidDimensionException(HERE) << "The model must contain at least two input variables";

  // build a Morris experiment
  const UnsignedInteger nbInputs = getPhysicalModel().getInputDimension();
  MorrisExperimentGrid morris_experiment(Indices(nbInputs, level_), bounds_, trajectoriesNumber_);

  // generate input sample
  RandomGenerator::SetSeed(getSeed());
  Sample inSample = morris_experiment.generate();
  inSample.setDescription(getPhysicalModel().getInputNames());

  // compute output sample
  Sample outSample = getPhysicalModel().getFunction(getInterestVariables())(inSample);
  outSample.setDescription(getInterestVariables());

  // Compute effects
  // set result
  result_ = MorrisResult(inSample, outSample);

  for (UnsignedInteger i = 0; i < outSample.getDimension(); ++i)
  {
    // no effect boundary
    Point a = result_.getMeanAbsoluteElementaryEffects(i);
    std::pair<Collection<Scalar>::iterator, Collection<Scalar>::iterator> p = boost::minmax_element(a.begin(), a.end());
    const Scalar minMeanAbsoluteEffect = (*p.first);
    const Scalar maxMeanAbsoluteEffect = (*p.second);
    result_.noEffectBoundary_[i] = (minMeanAbsoluteEffect + maxMeanAbsoluteEffect) * 0.5;
    // selected inputs
    for (UnsignedInteger j = 0; j < a.getDimension(); ++j)
    {
      if (a[j] < result_.noEffectBoundary_[i])
        result_.inputsSelection_[i][j] = 0;
    }
  }
}


MorrisResult MorrisAnalysis::getResult() const
{
  return result_;
}


MorrisResult& MorrisAnalysis::getResult()
{
  return result_;
}


Parameters MorrisAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "Morris");
  param.add("Outputs of interest", getInterestVariables().__str__());
  param.add("Number of trajectories", getTrajectoriesNumber());
  param.add("Level", getLevel());
  param.add("Seed", getSeed());
  String bounds;
  Description allInputsBoundsStr = Parameters::GetOTIntervalDescription(getBounds());
  for (UnsignedInteger i = 0; i < getPhysicalModel().getInputNames().getSize(); ++i)
  {
    bounds += getPhysicalModel().getInputNames()[i] + " : " + allInputsBoundsStr[i];
    if (i < getPhysicalModel().getInputNames().getSize() - 1)
       bounds += "\n";
  }
  param.add("Bounds", bounds);

  return param;
}


String MorrisAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.MorrisAnalysis('" << getName() << "', " << getPhysicalModel().getName() << ")\n";
  if (getInterestVariables().getSize() < getPhysicalModel().getSelectedOutputsNames().getSize())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }

  oss << getName() << ".setTrajectoriesNumber(" << getTrajectoriesNumber() << ")\n";
  oss << getName() << ".setBounds(ot.Interval(" << Parameters::GetOTPointStr(getBounds().getLowerBound())
      << ", " << Parameters::GetOTPointStr(getBounds().getUpperBound()) << "))\n";
  oss << getName() << ".setLevel(" << getLevel() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  return oss;
}


bool MorrisAnalysis::hasValidResult() const
{
  return result_.getInputSample().getSize() != 0;
}


/* String converter */
String MorrisAnalysis::__repr__() const
{
  OSS oss;
  oss << PhysicalModelAnalysis::__repr__()
      << " number of trajectories=" << getTrajectoriesNumber()
      << " bounds=" << getBounds()
      << " level=" << getLevel()
      << " seed=" << getSeed();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void MorrisAnalysis::save(Advocate & adv) const
{
  SimulationAnalysis::save(adv);
  adv.saveAttribute("trajectoriesNumber_", trajectoriesNumber_);
  adv.saveAttribute("bounds_", bounds_);
  adv.saveAttribute("level_", level_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void MorrisAnalysis::load(Advocate & adv)
{
  SimulationAnalysis::load(adv);
  adv.loadAttribute("trajectoriesNumber_", trajectoriesNumber_);
  adv.loadAttribute("bounds_", bounds_);
  adv.loadAttribute("level_", level_);
  adv.loadAttribute("result_", result_);
  inputNames_ = getPhysicalModel().getInputNames();
}
}
