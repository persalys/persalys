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
  : AnalysisImplementation()
//   , outputs_()
  , seed_(ResourceMap::GetAsNumericalScalar("RandomGenerator-InitialSeed"))
{
}


/* Constructor with parameters */
SimulationAnalysis::SimulationAnalysis(const String & name, const PhysicalModel & physicalModel)
  : AnalysisImplementation(name, physicalModel)
//  TODO , outputs_(physicalModel.getOutputs())
  , seed_(ResourceMap::GetAsNumericalScalar("RandomGenerator-InitialSeed"))
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
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


NumericalSample SimulationAnalysis::generateInputSample(const UnsignedInteger nbSimu)
{
  NumericalSample inputSample(getPhysicalModel().getInputRandomVector().getSample(nbSimu));
  inputSample.setDescription(getPhysicalModel().getStochasticInputNames());
  return inputSample;
}


NumericalSample SimulationAnalysis::computeOutputSample(NumericalSample inputSample) const
{
//   TODO
//   NumericalSample outputSample(getPhysicalModel().getRestrictedFunction(getOutputNames())(inputSample));
//   outputSample.setDescription(getOutputNames());
//   return outputSample;
  NumericalSample outputSample(getPhysicalModel().getRestrictedFunction()(inputSample));
  outputSample.setDescription(getPhysicalModel().getOutputNames());
  return outputSample;
}


NumericalSample SimulationAnalysis::computeOutputSample(NumericalSample inputSample, const Description & outputNames) const
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
  adv.saveAttribute("seed_", seed_);
}


/* Method load() reloads the object from the StorageManager */
void SimulationAnalysis::load(Advocate & adv)
{
  AnalysisImplementation::load(adv);
//   adv.loadAttribute("outputs_", outputs_);
  adv.loadAttribute("seed_", seed_);
}
}