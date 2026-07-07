//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis using simulation methods
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
#include "persalys/SimulationReliabilityAnalysis.hxx"

#include <openturns/RandomGenerator.hxx>
#include <openturns/CompositeRandomVector.hxx>
#include <openturns/MemoizeFunction.hxx>
#include <openturns/ThresholdEvent.hxx>

#include <limits>

using namespace OT;

namespace PERSALYS
{

struct SimuReliabilityAnalysisStruct
{
  SimuReliabilityAnalysisStruct(SimulationReliabilityAnalysis* analysis, SimulationInterface simulation)
    : analysis_(analysis)
    , simulation_(simulation)
  {
  };

  virtual ~SimuReliabilityAnalysisStruct() = default;

  SimulationReliabilityAnalysis * analysis_;
  SimulationInterface simulation_;
};

/* Default constructor */
SimulationReliabilityAnalysis::SimulationReliabilityAnalysis()
  : ReliabilityAnalysis()
  , WithStopCriteriaAnalysis()
  , seed_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
{
  setMaximumCoefficientOfVariation(0.1);
}


/* Constructor with parameters */
SimulationReliabilityAnalysis::SimulationReliabilityAnalysis(const String& name,
    const LimitState& limitState)
  : ReliabilityAnalysis(name, limitState)
  , WithStopCriteriaAnalysis()
  , seed_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
{
  setMaximumCoefficientOfVariation(0.1);
}


/* Virtual constructor */
SimulationReliabilityAnalysis* SimulationReliabilityAnalysis::clone() const
{
  return new SimulationReliabilityAnalysis(*this);
}

void SimulationReliabilityAnalysis::UpdateProgressValue(double percent, void * data)
{
  auto * analysisStruct = static_cast<SimuReliabilityAnalysisStruct*>(data);
  if (!analysisStruct)
    return;

  // set progress value
  if (analysisStruct->analysis_->getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
  {
    analysisStruct->analysis_->progressValue_ = (int) percent;
    analysisStruct->analysis_->notifyProgress();
  }

  // set information message
  OSS oss;
  oss << "Number of iterations = " << analysisStruct->simulation_.getResult().getOuterSampling() << "\n";
  oss << "Coefficient of variation = " << analysisStruct->simulation_.getResult().getCoefficientOfVariation() << "\n";
  oss << "Elapsed time = " << analysisStruct->simulation_.getResult().getTimeDuration() << " s\n";
  analysisStruct->analysis_->informationMessage_ = oss;
  analysisStruct->analysis_->notifyMessageUpdated(20);
}


SimulationInterface SimulationReliabilityAnalysis::getSimulationAlgorithm(const OT::RandomVector & /*event*/)
{
  throw NotYetImplementedException(HERE) << "In SimulationReliabilityAnalysis::getSimulationAlgorithm()";
}


UnsignedInteger SimulationReliabilityAnalysis::getSeed() const
{
  return seed_;
}


void SimulationReliabilityAnalysis::setSeed(const UnsignedInteger seed)
{
  seed_ = seed;
}


UnsignedInteger SimulationReliabilityAnalysis::getBlockSize() const
{
  return blockSize_;
}


void SimulationReliabilityAnalysis::setBlockSize(const UnsignedInteger size)
{
  if (size < 1)
    throw InvalidValueException(HERE) << "The block size must be greater than 0";
  blockSize_ = size;
}


bool SimulationReliabilityAnalysis::getComputeIndividualEventProbabilities() const
{
  return computeIndividualEventProbabilities_;
}


void SimulationReliabilityAnalysis::setComputeIndividualEventProbabilities(bool compute)
{
  computeIndividualEventProbabilities_ = compute;
}


void SimulationReliabilityAnalysis::initialize()
{
  AnalysisImplementation::initialize();
  result_ = SimulationReliabilityResult();
}


void SimulationReliabilityAnalysis::launch()
{
  // initialization
  RandomGenerator::SetSeed(getSeed());

  // create OT::Event
  Collection<Function> functions;
  RandomVector event = getLimitState().getThresholdEvent(functions);

  Collection<MemoizeFunction> memoFunctions;
  for (auto & func : functions)
  {
    const auto * memoFunc = dynamic_cast<MemoizeFunction*>(func.getImplementation().get());
    if (!memoFunc)
      throw InternalException(HERE) << "Expected a MemoizeFunction in getThresholdEvent";
    memoFunc->enableHistory();
    memoFunc->clearHistory();
    memoFunctions.add(*memoFunc);
  }

  // create OT::Simulation
  SimulationInterface algo = getSimulationAlgorithm(event);

  // set algo parameters
  UnsignedInteger maximumOuterSampling = (UnsignedInteger)std::numeric_limits<int>::max();
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
  {
    algo.setConvergenceStrategy(Compact(getMaximumCalls())); // TODO: propose in wizard the convergence sample's size?
    maximumOuterSampling = static_cast<UnsignedInteger>(ceil(1.0 * getMaximumCalls() / getBlockSize()));
  }
  
  algo.setMaximumOuterSampling(maximumOuterSampling);
  algo.setMaximumCoefficientOfVariation(getMaximumCoefficientOfVariation());
  algo.setBlockSize(getBlockSize());
  if (getMaximumElapsedTime() > 0)
    algo.setMaximumTimeDuration(getMaximumElapsedTime());
  algo.setStopCallback(&AnalysisImplementation::Stop, this);
  SimuReliabilityAnalysisStruct analysisStruc(this, algo);
  algo.setProgressCallback(&UpdateProgressValue, &analysisStruc);

  // run algo
  algo.run();

  Scalar elapsedTime = algo.getResult().getTimeDuration();

  Collection<ProbabilitySimulationResult> individualEventResults;
  if (getLimitState().isSystemLimitState() && computeIndividualEventProbabilities_)
    individualEventResults = computeIndividualEventResults(functions, elapsedTime);

  // set results
  // get convergence graph at level 0.95
  const Graph graph = algo.drawProbabilityConvergence();
  for(UnsignedInteger i = 0; i < graph.getDrawables().getSize(); ++i)
  {
    for(UnsignedInteger j = 0; j < graph.getDrawables()[i].getData().getSize(); ++j)
    {
      graph.getDrawables()[i].getData().getImplementation()->operator()(j, 0) *= getBlockSize();
    }
  }

  Collection<Sample> inSamples(memoFunctions.getSize());
  Collection<Sample> outSamples(memoFunctions.getSize());

  for (UnsignedInteger i = 0; i < memoFunctions.getSize(); ++i)
  {
    inSamples[i] = memoFunctions[i].getInputHistory();
    inSamples[i].setDescription(memoFunctions[i].getInputDescription());
    outSamples[i] = memoFunctions[i].getOutputHistory();
    outSamples[i].setDescription(memoFunctions[i].getOutputDescription());
  }
  
  result_ = SimulationReliabilityResult(algo.getResult(),
  inSamples,
  outSamples,
  graph.getDrawables()[0].getData(),
  graph.getDrawables()[1].getData(),
  graph.getDrawables()[2].getData(),
  useSharedInputSamples());

  result_.designOfExperiment_.setPhysicalModel(getPhysicalModel());
  result_.designOfExperiment_.setType(DesignOfExperiment::Type::MC);
  result_.elapsedTime_ = elapsedTime;

  result_.perEventSimulationResults_ = individualEventResults;

  for (const auto & memoFunc : memoFunctions)
  {
    memoFunc.disableHistory();
  }
}

Collection<ProbabilitySimulationResult> SimulationReliabilityAnalysis::computeIndividualEventResults(Collection<Function> functions, Scalar & elapsedTime)
{
  Collection<MemoizeFunction> memoFunctions;
  UnsignedInteger maxSampleSize = 0;
  for (auto & func : functions)
  {
    const auto * memoFunc = dynamic_cast<MemoizeFunction*>(func.getImplementation().get());
    if (!memoFunc)
      throw InternalException(HERE) << "Expected a MemoizeFunction in getThresholdEvent";
    if (memoFunc->getOutputHistory().getSize() > maxSampleSize)
      maxSampleSize = memoFunc->getOutputHistory().getSize();
    memoFunctions.add(*memoFunc);
  }

  UnsignedInteger maxOuterSampling = maxSampleSize / getBlockSize();
  if (maxSampleSize % getBlockSize() != 0)
    maxOuterSampling += 1;

  Collection<ProbabilitySimulationResult> individualEventResults(memoFunctions.getSize());

  const Description outputNames = getLimitState().getOutputNames();
  const Collection<ComparisonOperator> operators = getLimitState().getOperators();
  const Point thresholds = getLimitState().getThresholds();

  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++i)
  {
    RandomGenerator::SetSeed(getSeed());

    RandomVector subEvent = ThresholdEvent(CompositeRandomVector(memoFunctions[i], getPhysicalModel().getInputRandomVector()), operators[i], thresholds[i]);
    subEvent.setDescription(Description(1, outputNames[i]));
    SimulationInterface algo = getSimulationAlgorithm(subEvent);
    
    algo.setMaximumOuterSampling(maxOuterSampling);
    algo.setBlockSize(getBlockSize());
    if (getMaximumElapsedTime() > 0)
      algo.setMaximumTimeDuration(getMaximumElapsedTime() - elapsedTime);
    algo.setStopCallback(&AnalysisImplementation::Stop, this);

    algo.run();

    individualEventResults[i] = algo.getResult();
    elapsedTime += algo.getResult().getTimeDuration();
  }

  return individualEventResults;
}


SimulationReliabilityResult SimulationReliabilityAnalysis::getResult() const
{
  return result_;
}


Parameters SimulationReliabilityAnalysis::getParameters() const
{
  Parameters param;

  param.add("Block size", getBlockSize());
  param.add("Seed", getSeed());
  param.add("Compute individual event probabilities", computeIndividualEventProbabilities_ ? "true" : "false");

  return param;
}


String SimulationReliabilityAnalysis::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);
  oss << getName() << " = persalys." << getClassName() << "('" << getName() << "', " << getLimitState().getName() << ")\n";

  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumCalls(" << getMaximumCalls() << ")\n";
  oss << getName() << ".setMaximumCoefficientOfVariation(" << getMaximumCoefficientOfVariation() << ")\n";
  if (getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumElapsedTime(" << getMaximumElapsedTime() << ")\n";
  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";

  oss << getName() << ".setSeed(" << getSeed() << ")\n";
  if (computeIndividualEventProbabilities_)
    oss << getName() << ".setComputeIndividualEventProbabilities(True)\n";

  return oss;
}


bool SimulationReliabilityAnalysis::hasValidResult() const
{
  return getResult().getSimulationResult().getOuterSampling() != 0;
}


/* String converter */
String SimulationReliabilityAnalysis::__repr__() const
{
  OSS oss;
  oss << ReliabilityAnalysis::__repr__()
      << WithStopCriteriaAnalysis::__repr__()
      << " seed=" << getSeed()
      << " blockSize=" << getBlockSize();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SimulationReliabilityAnalysis::save(Advocate & adv) const
{
  ReliabilityAnalysis::save(adv);
  WithStopCriteriaAnalysis::save(adv);
  adv.saveAttribute("seed_", seed_);
  adv.saveAttribute("blockSize_", blockSize_);
  adv.saveAttribute("computeIndividualEventProbabilities_", computeIndividualEventProbabilities_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void SimulationReliabilityAnalysis::load(Advocate & adv)
{
  ReliabilityAnalysis::load(adv);
  WithStopCriteriaAnalysis::load(adv);
  adv.loadAttribute("seed_", seed_);
  adv.loadAttribute("blockSize_", blockSize_);
  if (adv.hasAttribute("computeIndividualEventProbabilities_"))
    adv.loadAttribute("computeIndividualEventProbabilities_", computeIndividualEventProbabilities_);
  adv.loadAttribute("result_", result_);
}
}
