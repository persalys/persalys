//                                               -*- C++ -*-
/**
 *  @brief SRCAnalysis computes the Standard Regression Coefficients
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
#include "persalys/SRCAnalysis.hxx"

#include <openturns/RandomGenerator.hxx>
#include <openturns/CorrelationAnalysis.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/BootstrapExperiment.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(SRCAnalysis)

static Factory<SRCAnalysis> Factory_SRCAnalysis;

/* Default constructor */
SRCAnalysis::SRCAnalysis()
  : SimulationAnalysis()
  , simulationsNumber_(10000)
  , result_()
{
  isDeterministicAnalysis_ = false;
}


/* Constructor with parameters */
SRCAnalysis::SRCAnalysis(const String & name, const PhysicalModel & physicalModel, const UnsignedInteger nbSimu)
  : SimulationAnalysis(name, physicalModel)
  , simulationsNumber_(nbSimu)
  , result_()
{
  isDeterministicAnalysis_ = false;
}


/* Virtual constructor */
SRCAnalysis* SRCAnalysis::clone() const
{
  return new SRCAnalysis(*this);
}


UnsignedInteger SRCAnalysis::getSimulationsNumber() const
{
  return simulationsNumber_;
}


void SRCAnalysis::setSimulationsNumber(const UnsignedInteger number)
{
  simulationsNumber_ = number;
}


void SRCAnalysis::initialize()
{
  SimulationAnalysis::initialize();
  result_ = SRCResult();
  notify("progressValueChanged");
}


void SRCAnalysis::launch()
{
  // check
  if (getBlockSize() > getSimulationsNumber())
    throw InvalidValueException(HERE) << "The block size (" << getBlockSize()
                                      << ") cannot be greater than the input sample size (" << getSimulationsNumber() << ")";

  // initialization
  RandomGenerator::SetSeed(getSeed());

  // generate input sample
  const Sample inputSample(generateInputSample(getSimulationsNumber()));

  const UnsignedInteger nbInputs = inputSample.getDimension();
  const UnsignedInteger nbOutputs = getInterestVariables().getSize();
  Function function(getPhysicalModel().getRestrictedFunction(getInterestVariables()));

  // number of iterations
  const UnsignedInteger nbIter = static_cast<UnsignedInteger>(ceil(1.0 * getSimulationsNumber() / getBlockSize()));
  // last block size
  const UnsignedInteger modulo = getSimulationsNumber() % getBlockSize();
  const UnsignedInteger lastBlockSize = modulo == 0 ? getBlockSize() : modulo;

  TimeCriteria timeCriteria;
  // evaluate output sample
  Sample outputSample(0, nbOutputs);
  for (UnsignedInteger i = 0; i < nbIter; ++i)
  {
    if (stopRequested_ && i > 1)
      break;

    // information message
    informationMessage_ = OSS() << "Elapsed time = " << timeCriteria.getElapsedTime() << " s\n";
    notify("informationMessageUpdated");

    progressValue_ = (int) (i * 100 / nbIter);
    notify("progressValueChanged");

    // the last block can be smaller
    const UnsignedInteger effectiveBlockSize = i < (nbIter - 1) ? getBlockSize() : lastBlockSize;

    // get input sample of size effectiveBlockSize
    const UnsignedInteger blockFirstIndex =  i * getBlockSize();
    const Sample blockInputSample(inputSample, blockFirstIndex, blockFirstIndex + effectiveBlockSize);

    // Perform a block of simulations
    outputSample.add(function(blockInputSample));

    timeCriteria.incrementElapsedTime();
  }

  // get input points really evaluated
  const Sample effectiveInputSample(inputSample, 0, outputSample.getSize());

  // compute signed SRC indices
  Sample indices(0, inputSample.getDimension());
  Sample signedIndices(0, inputSample.getDimension());
  Point r2(nbOutputs);

  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    const Point signedSRC(CorrelationAnalysis::SignedSRC(effectiveInputSample, outputSample.getMarginal(i)));
    signedIndices.add(signedSRC);
    Point unscaledSRC;
    for (UnsignedInteger j = 0; j < nbInputs; ++j)
    {
      unscaledSRC.add(signedSRC[j] * signedSRC[j]);
      r2[i] += signedSRC[j] * signedSRC[j];
    }
    indices.add(unscaledSRC);
  }

  // set results
  indices.setDescription(inputSample.getDescription());
  result_.indices_ = indices;
  result_.signedIndices_ = signedIndices;
  result_.r2_ = r2;
  result_.outputNames_ = getInterestVariables();
  result_.callsNumber_ = effectiveInputSample.getSize();

  // information message
  timeCriteria.incrementElapsedTime();
  informationMessage_ = OSS() << "Computation of the confidence intervals\n" << "Elapsed time = " << timeCriteria.getElapsedTime() << " s\n";
  notify("informationMessageUpdated");

  // Compute bootstrap confidence intervals
  const UnsignedInteger bootstrapSize = ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize");
  const Scalar alpha = ResourceMap::GetAsScalar("SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel");
  Indices inIndices(nbInputs);
  inIndices.fill();

  // - get bootstrap experiment
  Sample sample(effectiveInputSample);
  sample.stack(outputSample);
  BootstrapExperiment bootstrapExp(sample);
  // - compute signed SRC
  Collection<Sample> signedSRCBootstrap(nbOutputs, Sample(0, nbInputs));
  Collection<Sample> unscaledSRCBootstrap(nbOutputs, Sample(0, nbInputs));
  for (UnsignedInteger i = 0; i < bootstrapSize; ++i)
  {
    informationMessage_ = OSS() << "Computation of the confidence intervals\n" << "Elapsed time = " << timeCriteria.getElapsedTime() << " s\n";
    notify("informationMessageUpdated");
    const Sample bootstrapSample(bootstrapExp.generate());
    for (UnsignedInteger j = 0; j < nbOutputs; ++j)
    {
      const Point signedSRC(CorrelationAnalysis::SignedSRC(bootstrapSample.getMarginal(inIndices), bootstrapSample.getMarginal(nbInputs + j)));
      signedSRCBootstrap[j].add(signedSRC);

      Point unscaledSRC;
      for (UnsignedInteger k = 0; k < nbInputs; ++k)
        unscaledSRC.add(signedSRC[k] * signedSRC[k]);
      unscaledSRCBootstrap[j].add(unscaledSRC);
    }
    timeCriteria.incrementElapsedTime();
  }
  // - compute bounds
  PersistentCollection<Interval> signedSRCInterval(nbOutputs);
  PersistentCollection<Interval> unscaledSRCInterval(nbOutputs);
  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    Point upperBound(signedSRCBootstrap[i].computeQuantilePerComponent(alpha));
    Point lowerBound(signedSRCBootstrap[i].computeQuantilePerComponent(1-alpha));
    signedSRCInterval[i] = Interval(lowerBound, upperBound);

    Point upperBound2(unscaledSRCBootstrap[i].computeQuantilePerComponent(alpha));
    Point lowerBound2(unscaledSRCBootstrap[i].computeQuantilePerComponent(1-alpha));
    unscaledSRCInterval[i] = Interval(lowerBound2, upperBound2);
  }
  result_.indicesInterval_ = unscaledSRCInterval;
  result_.signedIndicesInterval_ = signedSRCInterval;
  timeCriteria.incrementElapsedTime();
  result_.elapsedTime_ = timeCriteria.getElapsedTime();

  // add warning if the model does not have an independent copula
  if (!getPhysicalModel().getCopula().hasIndependentCopula())
  {
    LOGWARN("The model does not have an independent copula, the result of the sensitivity analysis could be false.");
    warningMessage_ = "The model does not have an independent copula, the result of the sensitivity analysis could be false.";
  }
}


SRCResult SRCAnalysis::getResult() const
{
  return result_;
}


Parameters SRCAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "Standardized Regression Coefficients");
  param.add("Outputs of interest", getInterestVariables().__str__());
  param.add("Sample size", getSimulationsNumber());
  param.add(SimulationAnalysis::getParameters());

  return param;
}


String SRCAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = persalys.SRCAnalysis('" << getName() << "', " << getPhysicalModel().getName();
  oss << ", " << getSimulationsNumber() << ")\n";
  if (getInterestVariables().getSize() < getPhysicalModel().getSelectedOutputsNames().getSize())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }
  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  return oss;
}


bool SRCAnalysis::hasValidResult() const
{
  return result_.getIndices().getSize() != 0;
}


bool SRCAnalysis::canBeLaunched(String &errorMessage) const
{
  const bool canBeLaunched = PhysicalModelAnalysis::canBeLaunched(errorMessage);
  if (!canBeLaunched)
    return false;
  // pm must have independent copula
  if (!getPhysicalModel().getCopula().hasIndependentCopula())
    errorMessage = "The model must have an independent copula to compute a sensitivity analysis but here inputs are dependent.";
  return errorMessage.empty();
}


/* String converter */
String SRCAnalysis::__repr__() const
{
  OSS oss;
  oss << PhysicalModelAnalysis::__repr__()
      << " simulationsNumber=" << getSimulationsNumber()
      << " seed=" << getSeed()
      << " blockSize=" << getBlockSize();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SRCAnalysis::save(Advocate & adv) const
{
  SimulationAnalysis::save(adv);
  adv.saveAttribute("simulationsNumber_", simulationsNumber_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void SRCAnalysis::load(Advocate & adv)
{
  SimulationAnalysis::load(adv);
  adv.loadAttribute("simulationsNumber_", simulationsNumber_);
  adv.loadAttribute("result_", result_);
}
}
