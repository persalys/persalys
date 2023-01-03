//                                               -*- C++ -*-
/**
 *  @brief MorrisAnalysis
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
#include "persalys/MorrisAnalysis.hxx"

#include <openturns/RandomGenerator.hxx>
#include <openturns/SpecFunc.hxx>
#include <openturns/PersistentObjectFactory.hxx>

#include <algorithm>

#ifdef PERSALYS_HAVE_OTMORRIS
#include <otmorris/Morris.hxx>
#include <otmorris/MorrisExperimentGrid.hxx>
using namespace OTMORRIS;
#endif

using namespace OT;


namespace PERSALYS
{

CLASSNAMEINIT(MorrisAnalysis)

static Factory<MorrisAnalysis> Factory_MorrisAnalysis;

/* Default constructor */
MorrisAnalysis::MorrisAnalysis()
  : SimulationAnalysis()
{
}


/* Constructor with parameters */
MorrisAnalysis::MorrisAnalysis(const String& name, const PhysicalModel& physicalModel)
  : SimulationAnalysis(name, physicalModel)
{
  initializeParameters();
}


/* Virtual constructor */
MorrisAnalysis* MorrisAnalysis::clone() const
{
  return new MorrisAnalysis(*this);
}


void MorrisAnalysis::initializeParameters()
{
  inputNames_ = getPhysicalModel().getInputNames();

  Point fakePoint;
  Tools::ComputeBounds(getPhysicalModel().getInputs(), fakePoint, bounds_);
}


void MorrisAnalysis::updateParameters()
{
  const Description inputNames(inputNames_);
  const Point lowerBounds(bounds_.getLowerBound());
  const Point upperBounds(bounds_.getUpperBound());

  initializeParameters();

  Point newLowerBounds(bounds_.getLowerBound());
  Point newUpperBounds(bounds_.getUpperBound());

  for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++ i)
  {
    const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
    if (it != inputNames.end())
    {
      newLowerBounds[i] = lowerBounds[it - inputNames.begin()];
      newUpperBounds[i] = upperBounds[it - inputNames.begin()];
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
#ifdef PERSALYS_HAVE_OTMORRIS
  // check
  if (getPhysicalModel().getInputDimension() < 2)
    throw InvalidDimensionException(HERE) << "The model must contain at least two input variables";
  if (bounds_.isEmpty())
    throw InvalidArgumentException(HERE) << "The interval has to be not empty";
  if (bounds_.getDimension() != getPhysicalModel().getInputDimension())
  {
    OSS oss;
    oss << "The dimension of the interval " << bounds_.getDimension();
    oss << " has to be equal to the number of inputs of the physical model: ";
    oss << getPhysicalModel().getInputDimension();
    throw InvalidArgumentException(HERE) << oss.str();
  }
  UnsignedInteger counter = 0;
  for (UnsignedInteger i = 0; i < bounds_.getDimension(); ++i)
  {
    if (!bounds_.getMarginal(i).isNumericallyEmpty())
      ++counter;
    if (counter == 2)
      break;
  }
  if (counter < 2)
    throw InvalidArgumentException(HERE) << "At least two variables must vary.";

  // build a Morris experiment
  const UnsignedInteger nbInputs = getPhysicalModel().getInputDimension();
  MorrisExperimentGrid morris_experiment(Indices(nbInputs, level_), bounds_, trajectoriesNumber_);
  morris_experiment.setJumpStep(Indices(nbInputs, std::floor(level_ * 0.5)));

  // generate input sample
  RandomGenerator::SetSeed(getSeed());
  Sample inSample = morris_experiment.generate();
  inSample.setDescription(getPhysicalModel().getInputNames());

  // compute output sample
  Sample outSample(0, getInterestVariables().getSize());
  outSample.setDescription(getInterestVariables());
  Function function(getPhysicalModel().getFunction(getInterestVariables()));

  const UnsignedInteger nbIter = static_cast<UnsignedInteger>(ceil(1.0 * inSample.getSize() / getBlockSize()));

  Sample effectiveInSample(0, nbInputs);
  effectiveInSample.setDescription(getPhysicalModel().getInputNames());

  // time
  TimeCriteria timeCriteria;

  for (UnsignedInteger i = 0; i < nbIter; ++ i)
  {
    if (stopRequested_)
      break;

    progressValue_ = (int) (i * 100 / nbIter);
    notify("progressValueChanged");

    // information message
    informationMessage_ = OSS() << "Elapsed time = " << timeCriteria.getElapsedTime() << " s\n";
    notify("informationMessageUpdated");

    const Sample blockInputSample(inSample, i * getBlockSize(), std::min((i + 1) * getBlockSize(), inSample.getSize()));

    effectiveInSample.add(blockInputSample);
    outSample.add(function(blockInputSample));

    timeCriteria.incrementElapsedTime();
  }

  // Compute effects
  Morris morris(effectiveInSample, outSample, bounds_);

  // set result
  const UnsignedInteger nbOutputs = outSample.getDimension();

  result_.designOfExperiment_ = DesignOfExperiment("name", getPhysicalModel());
  result_.designOfExperiment_.setInputSample(effectiveInSample);
  result_.designOfExperiment_.setOutputSample(outSample);
  result_.inputsSelection_ = PersistentCollection<Indices >(nbOutputs, Indices(nbInputs, 1));
  result_.noEffectBoundary_ = Point(nbOutputs);
  result_.elementaryEffectsMean_ = Sample(nbOutputs, nbInputs);
  result_.elementaryEffectsStandardDeviation_ = Sample(nbOutputs, nbInputs);
  result_.absoluteElementaryEffectsMean_ = Sample(nbOutputs, nbInputs);

  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    result_.elementaryEffectsMean_[i] = morris.getMeanElementaryEffects(i);
    result_.elementaryEffectsStandardDeviation_[i] = morris.getStandardDeviationElementaryEffects(i);
    Point meanAee(morris.getMeanAbsoluteElementaryEffects(i));
    result_.absoluteElementaryEffectsMean_[i] = meanAee;

    // no effect boundary
    std::pair<Collection<Scalar>::iterator, Collection<Scalar>::iterator> p = std::minmax_element(meanAee.begin(), meanAee.end());
    const Scalar minMeanAbsoluteEffect = (*p.first);
    const Scalar maxMeanAbsoluteEffect = (*p.second);
    result_.noEffectBoundary_[i] = (minMeanAbsoluteEffect + maxMeanAbsoluteEffect) * 0.05;
    // selected inputs
    for (UnsignedInteger j = 0; j < meanAee.getDimension(); ++j)
    {
      if (meanAee[j] < result_.noEffectBoundary_[i])
        result_.inputsSelection_[i][j] = 0;
    }
  }
#else
  throw NotYetImplementedException(HERE) << "No otmorris support";
#endif
}


MorrisResult MorrisAnalysis::getResult() const
{
  return result_;
}

void MorrisAnalysis::setBlockSize(const UnsignedInteger blockSize)
{
  blockSize_ = blockSize;
}

UnsignedInteger MorrisAnalysis::getBlockSize() const
{
  return blockSize_;
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
  if (getBounds().getDimension() == inputNames_.getSize())
  {
    String bounds;
    const Description allInputsBoundsStr(Parameters::GetOTIntervalDescription(getBounds()));
    for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++i)
    {
      bounds += inputNames_[i] + " : " + allInputsBoundsStr[i];
      if (i < inputNames_.getSize() - 1)
        bounds += "\n";
    }
    param.add("Bounds", bounds);
  }

  return param;
}


String MorrisAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = persalys.MorrisAnalysis('" << getName() << "', " << getPhysicalModel().getName() << ")\n";
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
  return result_.getDesignOfExperiment().getSample().getSize() != 0;
}


bool MorrisAnalysis::canBeLaunched(String &errorMessage) const
{
  const bool canBeLaunched = PhysicalModelAnalysis::canBeLaunched(errorMessage);
  if (!canBeLaunched)
    return false;
  // pm must have more than 2 inputs
  if (getPhysicalModel().getInputDimension() < 2)
    errorMessage = "The physical model must have at least two inputs.";
  return errorMessage.empty();
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
  adv.saveAttribute("inputNames_", inputNames_);
}


/* Method load() reloads the object from the StorageManager */
void MorrisAnalysis::load(Advocate & adv)
{
  SimulationAnalysis::load(adv);
  adv.loadAttribute("trajectoriesNumber_", trajectoriesNumber_);
  adv.loadAttribute("bounds_", bounds_);
  adv.loadAttribute("level_", level_);
  adv.loadAttribute("result_", result_);
  adv.loadAttribute("inputNames_", inputNames_);
}
}
