//                                               -*- C++ -*-
/**
 *  @brief SimulationAnalysis evaluates the models
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
#include "otgui/SimulationAnalysis.hxx"

using namespace OT;

namespace OTGUI {

/* Default constructor */
SimulationAnalysis::SimulationAnalysis()
  : PhysicalModelAnalysis()
  , seed_(ResourceMap::GetAsNumericalScalar("RandomGenerator-InitialSeed"))
{
}


/* Constructor with parameters */
SimulationAnalysis::SimulationAnalysis(const String & name, const PhysicalModel & physicalModel)
  : PhysicalModelAnalysis(name, physicalModel)
  , seed_(ResourceMap::GetAsNumericalScalar("RandomGenerator-InitialSeed"))
{
}


NumericalSample SimulationAnalysis::generateInputSample(const UnsignedInteger nbSimu)
{
  NumericalSample inputSample(getPhysicalModel().getInputRandomVector().getSample(nbSimu));
  inputSample.setDescription(getPhysicalModel().getStochasticInputNames());
  return inputSample;
}


NumericalSample SimulationAnalysis::computeOutputSample(NumericalSample inputSample) const
{
  NumericalSample outputSample(getPhysicalModel().getRestrictedFunction(getInterestVariables())(inputSample));
  outputSample.setDescription(getInterestVariables());
  return outputSample;
}


NumericalSample SimulationAnalysis::computeOutputSample(NumericalSample inputSample, const Description & outputNames) const
{
  NumericalSample outputSample(getPhysicalModel().getRestrictedFunction(outputNames)(inputSample));
  outputSample.setDescription(outputNames);
  return outputSample;
}


UnsignedInteger SimulationAnalysis::getSeed() const
{
  return seed_;
}


void SimulationAnalysis::setSeed(const UnsignedInteger seed)
{
  seed_ = seed;
}


/* Method save() stores the object through the StorageManager */
void SimulationAnalysis::save(Advocate & adv) const
{
  PhysicalModelAnalysis::save(adv);
  adv.saveAttribute("seed_", seed_);
}


/* Method load() reloads the object from the StorageManager */
void SimulationAnalysis::load(Advocate & adv)
{
  PhysicalModelAnalysis::load(adv);
//   adv.loadAttribute("outputs_", outputs_);
  adv.loadAttribute("seed_", seed_);
}
}