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

#include "RandomGenerator.hxx"
#include "CorrelationAnalysis.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SimulationAnalysis);

/* Default constructor */
SimulationAnalysis::SimulationAnalysis()
  : AnalysisImplementation()
//   , outputs_()
  , nbSimulations_(ResourceMap::GetAsNumericalScalar("Simulation-DefaultMaximumOuterSampling"))
  , seed_(ResourceMap::GetAsNumericalScalar("RandomGenerator-InitialSeed"))
{
}


/* Constructor with parameters */
SimulationAnalysis::SimulationAnalysis(const String & name, const PhysicalModel & physicalModel,
                                       const UnsignedInteger nbSimulation)
  : AnalysisImplementation(name, physicalModel)
//  TODO , outputs_(physicalModel.getOutputs())
  , nbSimulations_(nbSimulation)
  , seed_(ResourceMap::GetAsNumericalScalar("RandomGenerator-InitialSeed"))
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


/* Virtual constructor */
SimulationAnalysis* SimulationAnalysis::clone() const
{
  return new SimulationAnalysis(*this);
}

// TODO
// OutputCollection SimulationAnalysis::getOutputs() const
// {
//   return outputs_;
// }
// 
// 
// void SimulationAnalysis::setOutputs(const OutputCollection & outputs)
// {
//   outputs_ = outputs;
// }


UnsignedInteger SimulationAnalysis::getNbSimulations() const
{
  return nbSimulations_;
}


void SimulationAnalysis::setNbSimulations(const UnsignedInteger nbSimu)
{
  nbSimulations_ = nbSimu;
}


NumericalSample SimulationAnalysis::getInputSample()
{
  NumericalSample inputSample(getPhysicalModel().getInputRandomVector().getSample(nbSimulations_));
  inputSample.setDescription(getPhysicalModel().getStochasticInputNames());
  return inputSample;
}


NumericalSample SimulationAnalysis::getOutputSample(NumericalSample inputSample) const
{
//   TODO
//   NumericalSample outputSample(getPhysicalModel().getRestrictedFunction(getOutputNames())(inputSample));
//   outputSample.setDescription(getOutputNames());
//   return outputSample;
  NumericalSample outputSample(getPhysicalModel().getRestrictedFunction()(inputSample));
  outputSample.setDescription(getPhysicalModel().getOutputNames());
  return outputSample;
}


NumericalSample SimulationAnalysis::getOutputSample(NumericalSample inputSample, const Description & outputNames) const
{
  NumericalSample outputSample(getPhysicalModel().getRestrictedFunction(outputNames)(inputSample));
  outputSample.setDescription(outputNames);
  return outputSample;
}


Description SimulationAnalysis::getOutputNames() const
{
//   TODO
//   int nbOutputs = outputs_.getSize();
//   Description outputNames(nbOutputs);
//   for (int i=0; i<nbOutputs; ++i)
//     outputNames[i] = outputs_[i].getName();
//   return outputNames;
  return getPhysicalModel().getOutputNames();
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
  AnalysisImplementation::save(adv);
//   adv.saveAttribute("outputs_", outputs_);
  adv.saveAttribute("nbSimulations_", nbSimulations_);
  adv.saveAttribute("seed_", seed_);
}


/* Method load() reloads the object from the StorageManager */
void SimulationAnalysis::load(Advocate & adv)
{
  AnalysisImplementation::load(adv);
//   adv.loadAttribute("outputs_", outputs_);
  adv.loadAttribute("nbSimulations_", nbSimulations_);
  adv.loadAttribute("seed_", seed_);
}
}