//                                               -*- C++ -*-
/**
 *  @brief Results of the reliability analysis using simulation methods
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#include "persalys/SimulationReliabilityResult.hxx"

#include <persalys/EvaluationResult.hxx>

#include <openturns/PersistentObjectFactory.hxx>

#include <map>
#include <limits>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(SimulationReliabilityResult)

const static Factory<SimulationReliabilityResult> Factory_SimulationReliabilityResult;


/* Constructor with parameters */
SimulationReliabilityResult::SimulationReliabilityResult(const ProbabilitySimulationResult& simulationResults,
    const Collection<Sample> &inSamples,
    const Collection<Sample> &outSamples,
    const Sample& convergenceSample,
    const Sample& convergenceSampleLowerBound,
    const Sample& convergenceSampleUpperBound,
    bool sharedInputs)
  : EvaluationResult()
  , inputSamples_(inSamples)
  , outputSamples_(outSamples)
  , simulationResult_(simulationResults)
  , convergenceSample_(convergenceSample)
  , convergenceSampleLowerBound_(convergenceSampleLowerBound)
  , convergenceSampleUpperBound_(convergenceSampleUpperBound)
  , sharedInputs_(sharedInputs)
{
  if (inSamples.getSize() != outSamples.getSize())
    throw InvalidArgumentException(HERE) << "Input and output sample collections must have the same size.";

  std::map<Point, UnsignedInteger> inputPointToRow;
  std::map<String, UnsignedInteger> outputDescriptionToColumn;

  for (const auto & sample : outSamples)
  {
    const String desc = sample.getDescription()[0];
    if (outputDescriptionToColumn.find(desc) == outputDescriptionToColumn.end())
      outputDescriptionToColumn[desc] = outputDescriptionToColumn.size();
  }

  const UnsignedInteger numberOfEvents = inSamples.getSize();
  const UnsignedInteger numberOfOutputs = outputDescriptionToColumn.size();

  Sample mergedInSample(0, inSamples[0].getDimension());
  Sample mergedOutSample(0, numberOfOutputs);

  for (UnsignedInteger k = 0; k < numberOfEvents; ++k)
  {
    Sample inSample   = inSamples[k];
    Sample outSample  = outSamples[k];

    if (inSample.getSize() != outSample.getSize())
      throw InvalidArgumentException(HERE) << "Input and output samples must have the same size for each event.";

    for (UnsignedInteger i = 0; i < inSample.getSize(); ++i)
    {
      Point inputPoint = inSample[i];
      if (inputPointToRow.find(inputPoint) == inputPointToRow.end())
      {
        inputPointToRow[inputPoint] = inputPointToRow.size();
        mergedInSample.add(inputPoint);
        mergedOutSample.add(Point(numberOfOutputs, std::numeric_limits<Scalar>::quiet_NaN()));
      }

      mergedOutSample(inputPointToRow[inputPoint], outputDescriptionToColumn[outSample.getDescription()[0]]) = outSample[i][0];
    }
  }

  Description mergedOutDescription(numberOfOutputs);
  for (const auto & [str, idx] : outputDescriptionToColumn)
    mergedOutDescription[idx] = str;
  
  mergedOutSample.setDescription(mergedOutDescription);
  mergedInSample.setDescription(inSamples[0].getDescription());

  designOfExperiment_.setInputSample(mergedInSample);
  designOfExperiment_.setOutputSample(mergedOutSample);

}

/* Virtual constructor */
SimulationReliabilityResult* SimulationReliabilityResult::clone() const
{
  return new SimulationReliabilityResult(*this);
}


ProbabilitySimulationResult SimulationReliabilityResult::getSimulationResult() const
{
  return simulationResult_;
}


Sample SimulationReliabilityResult::getConvergenceSample() const
{
  return convergenceSample_;
}


Sample SimulationReliabilityResult::getConvergenceSampleLowerBound() const
{
  return convergenceSampleLowerBound_;
}


Sample SimulationReliabilityResult::getConvergenceSampleUpperBound() const
{
  return convergenceSampleUpperBound_;
}

Collection<Sample> SimulationReliabilityResult::getInputSamples() const
{
  return inputSamples_;
}

Collection<Sample> SimulationReliabilityResult::getOutputSamples() const
{
  return outputSamples_;
}

ProbabilitySimulationResult SimulationReliabilityResult::getPerEventSimulationResult(UnsignedInteger eventIndex) const
{
  if (eventIndex >= perEventSimulationResults_.getSize())
    throw InvalidArgumentException(HERE) << "Event index out of range: " << eventIndex;
  
  return perEventSimulationResults_[eventIndex];
}


bool SimulationReliabilityResult::hasPerEventSimulationResults() const
{
  return perEventSimulationResults_.getSize() > 0;
}


/* String converter */
String SimulationReliabilityResult::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " name=" << getName()
      << " simulationResult=" << getSimulationResult();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SimulationReliabilityResult::save(Advocate& adv) const
{
  EvaluationResult::save(adv);
  adv.saveAttribute("simulationResult_", simulationResult_);
  adv.saveAttribute("convergenceSample_", convergenceSample_);
  adv.saveAttribute("convergenceSampleLowerBound_", convergenceSampleLowerBound_);
  adv.saveAttribute("convergenceSampleUpperBound_", convergenceSampleUpperBound_);
  adv.saveAttribute("inputSamples_", inputSamples_);
  adv.saveAttribute("outputSamples_", outputSamples_);
  adv.saveAttribute("sharedInputs_", sharedInputs_);
  adv.saveAttribute("perEventSimulationResults_", perEventSimulationResults_);
}


/* Method load() reloads the object from the StorageManager */
void SimulationReliabilityResult::load(Advocate& adv)
{
  EvaluationResult::load(adv);
  adv.loadAttribute("simulationResult_", simulationResult_);
  adv.loadAttribute("convergenceSample_", convergenceSample_);
  adv.loadAttribute("convergenceSampleLowerBound_", convergenceSampleLowerBound_);
  adv.loadAttribute("convergenceSampleUpperBound_", convergenceSampleUpperBound_);
  // can open older xml files
  if (!designOfExperiment_.getSample().getSize())
  {
    Sample outS;
    adv.loadAttribute("outputSample_", outS);
    designOfExperiment_.setOutputSample(outS);
  }
  if (adv.hasAttribute("inputSamples_"))
  {
    // Persalys 20.1+
    adv.loadAttribute("inputSamples_", inputSamples_);
    adv.loadAttribute("outputSamples_", outputSamples_);
    adv.loadAttribute("sharedInputs_", sharedInputs_);
    if (adv.hasAttribute("perEventSimulationResults_"))
      adv.loadAttribute("perEventSimulationResults_", perEventSimulationResults_);
  }
  else
  {
    inputSamples_ = Collection<Sample>(1, designOfExperiment_.getInputSample());
    outputSamples_ = Collection<Sample>(1, designOfExperiment_.getOutputSample());
  }
}
}
