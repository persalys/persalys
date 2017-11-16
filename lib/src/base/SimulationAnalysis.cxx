//                                               -*- C++ -*-
/**
 *  @brief SimulationAnalysis evaluates the models
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
#include "otgui/SimulationAnalysis.hxx"

using namespace OT;

namespace OTGUI
{

/* Default constructor */
SimulationAnalysis::SimulationAnalysis()
  : PhysicalModelAnalysis()
  , designOfExperiment_()
  , failedInputSample_()
  , blockSize_(1)
  , seed_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
{
}


/* Constructor with parameters */
SimulationAnalysis::SimulationAnalysis(const String & name, const PhysicalModel & physicalModel)
  : PhysicalModelAnalysis(name, physicalModel)
  , designOfExperiment_(name, physicalModel)
  , failedInputSample_()
  , blockSize_(1)
  , seed_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
{
}


DesignOfExperiment SimulationAnalysis::getDesignOfExperiment() const
{
  return designOfExperiment_;
}


Sample SimulationAnalysis::getFailedInputSample() const
{
  return failedInputSample_;
}


UnsignedInteger SimulationAnalysis::getBlockSize() const
{
  return blockSize_;
}


void SimulationAnalysis::setBlockSize(const UnsignedInteger size)
{
  if (size < 1)
    throw InvalidValueException(HERE) << "The block size must be superior to 0";

  blockSize_ = size;
}


UnsignedInteger SimulationAnalysis::getSeed() const
{
  return seed_;
}


void SimulationAnalysis::setSeed(const UnsignedInteger seed)
{
  seed_ = seed;
}


Sample SimulationAnalysis::generateInputSample(const UnsignedInteger nbSimu) const
{
  Sample inputSample(getPhysicalModel().getInputRandomVector().getSample(nbSimu));
  inputSample.setDescription(getPhysicalModel().getStochasticInputNames());
  return inputSample;
}


Sample SimulationAnalysis::computeOutputSample(const Sample& inputSample) const
{
  Sample outputSample(getPhysicalModel().getRestrictedFunction(getInterestVariables())(inputSample));
  return outputSample;
}


Sample SimulationAnalysis::computeOutputSample(const Point& inputValues) const
{
  return computeOutputSample(Sample(1, inputValues));
}


void SimulationAnalysis::initialize()
{
  PhysicalModelAnalysis::initialize();
  designOfExperiment_.getImplementation()->initialize();
}


bool SimulationAnalysis::hasValidResult() const
{
  return designOfExperiment_.getOutputSample().getSize() != 0;
}


/* Method save() stores the object through the StorageManager */
void SimulationAnalysis::save(Advocate & adv) const
{
  PhysicalModelAnalysis::save(adv);
  adv.saveAttribute("designOfExperiment_", designOfExperiment_);
  adv.saveAttribute("failedInputSample_", failedInputSample_);
  adv.saveAttribute("blockSize_", blockSize_);
  adv.saveAttribute("seed_", seed_);
}


/* Method load() reloads the object from the StorageManager */
void SimulationAnalysis::load(Advocate & adv)
{
  PhysicalModelAnalysis::load(adv);
  adv.loadAttribute("designOfExperiment_", designOfExperiment_);
  adv.loadAttribute("failedInputSample_", failedInputSample_);
  adv.loadAttribute("blockSize_", blockSize_);
  adv.loadAttribute("seed_", seed_);
}
}
