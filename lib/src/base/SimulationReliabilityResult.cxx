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

#include <set>
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
  if (sharedInputs && inSamples.getSize() > 1)
  {
    // MC with system event: all components share the same input points.
    // Build a DOE with N rows, one output column per component (no row duplication).
    UnsignedInteger N = 0;
    Sample inSample;
    for (UnsignedInteger i = 0; i < inSamples.getSize(); ++i)
      if (inSamples[i].getSize() > N)
      {
        N = inSamples[i].getSize();
        inSample = inSamples[i];
      }
    
    const UnsignedInteger k = outSamples.getSize();

    Description outDescription;
    for (const auto & s : outSamples)
      outDescription.add(s.getDescription()[0]);

    Sample outSample(N, Point(k, std::numeric_limits<Scalar>::quiet_NaN()));
    outSample.setDescription(outDescription);

    // Each component may have fewer rows than the reference sample: some input
    // points are skipped for a given output (e.g. short-circuit evaluation in
    // union/intersection).  Both the reference sample and each component sample
    // are in the same RNG order, so we use a two-pointer merge to locate the
    // position of each component row inside the reference.
    const UnsignedInteger dim = inSample.getDimension();
    for (UnsignedInteger j = 0; j < k; ++j)
    {
      const Sample & inJ  = inSamples[j];
      const Sample & outJ = outSamples[j];
      UnsignedInteger refIdx  = 0;
      UnsignedInteger compIdx = 0;
      while (refIdx < N && compIdx < inJ.getSize())
      {
        bool match = true;
        for (UnsignedInteger d = 0; d < dim; ++d)
        {
          if (inSample(refIdx, d) != inJ(compIdx, d))
          {
            match = false;
            break;
          }
        }
        if (match)
        {
          outSample(refIdx, j) = outJ(compIdx, 0);
          ++refIdx;
          ++compIdx;
        }
        else
        {
          // This reference row was not evaluated for component j — leave NaN.
          ++refIdx;
        }
      }
    }

    designOfExperiment_.setInputSample(inSample);
    designOfExperiment_.setOutputSample(outSample);
  }
  else
  {
    // IS with system event (or single event): each component may have different
    // input points. Stack rows, NaN-pad output columns.
    UnsignedInteger totalSize = 0;
    std::set<String> outputDescriptions;
    for (const auto & sample : outSamples)
    {
      totalSize += sample.getSize();
      outputDescriptions.insert(sample.getDescription()[0]);
    }

    Sample inSample(totalSize, Point(inSamples[0].getDimension(), std::numeric_limits<Scalar>::quiet_NaN()));
    Sample outSample(totalSize, Point(outputDescriptions.size(), std::numeric_limits<Scalar>::quiet_NaN()));
    inSample.setDescription(inSamples[0].getDescription());
    Description outDescription;
    for(const auto & desc : outputDescriptions)
      outDescription.add(desc);
    outSample.setDescription(outDescription);

    UnsignedInteger offset = 0;
    for (const auto & sample : inSamples)
    {
      for(UnsignedInteger i = 0; i < sample.getSize(); ++i)
        for (UnsignedInteger j = 0; j < sample.getDimension(); ++j)
          inSample(i + offset, j) = sample(i, j);
      offset += sample.getSize();
    }

    offset = 0;
    for (const auto & sample : outSamples)
    {
      UnsignedInteger descIndex = outDescription.find(sample.getDescription()[0]);
      for(UnsignedInteger i = 0; i < sample.getSize(); ++i)
        outSample(i + offset, descIndex) = sample(i, 0);
      offset += sample.getSize();
    }

    designOfExperiment_.setInputSample(inSample);
    designOfExperiment_.setOutputSample(outSample);
  }
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
  }
  else
  {
    inputSamples_ = Collection<Sample>(1, designOfExperiment_.getInputSample());
    outputSamples_ = Collection<Sample>(1, designOfExperiment_.getOutputSample());
  }
}
}
