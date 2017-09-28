//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis using simulation methods
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
#include "otgui/SimulationReliabilityAnalysis.hxx"

#include <openturns/RandomGenerator.hxx>

using namespace OT;

namespace OTGUI
{

/* Default constructor */
SimulationReliabilityAnalysis::SimulationReliabilityAnalysis()
  : ReliabilityAnalysis()
  , WithStopCriteriaAnalysis()
  , seed_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
  , blockSize_(ResourceMap::GetAsScalar("Simulation-DefaultBlockSize"))
  , timeCriteria_()
{
}


/* Constructor with parameters */
SimulationReliabilityAnalysis::SimulationReliabilityAnalysis(const String& name,
    const LimitState& limitState)
  : ReliabilityAnalysis(name, limitState)
  , WithStopCriteriaAnalysis()
  , seed_(ResourceMap::GetAsUnsignedInteger("RandomGenerator-InitialSeed"))
  , blockSize_(ResourceMap::GetAsScalar("Simulation-DefaultBlockSize"))
  , timeCriteria_()
{
}


/* Virtual constructor */
SimulationReliabilityAnalysis* SimulationReliabilityAnalysis::clone() const
{
  return new SimulationReliabilityAnalysis(*this);
}


struct AnalysisStruct
{
  AnalysisStruct(SimulationReliabilityAnalysis* analysis, SimulationInterface simulation)
    : analysis_(analysis)
    , simulation_(simulation)
  {
  };

  virtual ~AnalysisStruct() {};

  SimulationReliabilityAnalysis * analysis_;
  SimulationInterface simulation_;
};

void SimulationReliabilityAnalysis::UpdateProgressValue(double percent, void * data)
{
  AnalysisStruct * analysisStruct = static_cast<AnalysisStruct*>(data);
  if (!analysisStruct)
    return;

  // set progress value
  if (analysisStruct->analysis_->getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
  {
    analysisStruct->analysis_->progressValue_ = (int) percent;
    analysisStruct->analysis_->notify("progressValueChanged");
  }

  // set information message
  OSS oss;
  oss << "Number of iterations = " << analysisStruct->simulation_.getResult().getOuterSampling() << "\n";
  oss << "Coefficient of variation = " << analysisStruct->simulation_.getResult().getCoefficientOfVariation() << "\n";
  oss << "Elapsed time = " << (Scalar) analysisStruct->analysis_->timeCriteria_.elapsedTime_ / CLOCKS_PER_SEC << " s\n";
  analysisStruct->analysis_->informationMessage_ = oss;
  analysisStruct->analysis_->notify("informationMessageUpdated");
}


SimulationInterface SimulationReliabilityAnalysis::getSimulationAlgorithm(const OT::Event& event)
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
    throw InvalidValueException(HERE) << "The block size must be superior to 0";
  if (size > getMaximumCalls())
    throw InvalidValueException(HERE) << "The block size can not be superior to the maximum calls " << getMaximumCalls();
  blockSize_ = size;
}


void SimulationReliabilityAnalysis::run()
{
  isRunning_ = true;
  try
  {
    // clear result
    initialize();
    timeCriteria_ = TimeCriteria();
    result_ = SimulationReliabilityResult();

    // initialization
    RandomGenerator::SetSeed(getSeed());

    Description outputName(1);
    outputName[0] = getLimitState().getOutputName();

    // get function
    Function function(getPhysicalModel().getRestrictedFunction(outputName));
    function.enableHistory();
    function.clearHistory();

    // create OT::Event
    Event event(RandomVector(function, getPhysicalModel().getInputRandomVector()), getLimitState().getOperator(), getLimitState().getThreshold());
    event.setDescription(outputName);

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

    timeCriteria_.setStartTime(clock());
    timeCriteria_.setMaxElapsedTime(getMaximumElapsedTime());
    algo.setStopCallback(&Stop, &timeCriteria_);
    AnalysisStruct analysisStruc(this, algo);
    algo.setProgressCallback(&UpdateProgressValue, &analysisStruc);

    // run algo
    algo.run();

    // set results
    // get convergence graph at level 0.95
    const Graph graph = algo.drawProbabilityConvergence();
    result_ = SimulationReliabilityResult(algo.getResult(),
                                          function.getHistoryOutput().getSample(),
                                          graph.getDrawables()[0].getData(),
                                          graph.getDrawables()[1].getData(),
                                          graph.getDrawables()[2].getData());

    result_.elapsedTime_ = (Scalar) timeCriteria_.elapsedTime_ / CLOCKS_PER_SEC;

    function.disableHistory();

    notify("analysisFinished");
  }
  catch (std::exception & ex)
  {
    errorMessage_ = ex.what();
    notify("analysisBadlyFinished");
  }
  isRunning_ = false;
}


void SimulationReliabilityAnalysis::stop()
{
  AnalysisImplementation::stop();
  timeCriteria_.stop();
}


SimulationReliabilityResult SimulationReliabilityAnalysis::getResult() const
{
  return result_;
}


String SimulationReliabilityAnalysis::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);
  oss << getName() << " = otguibase." << getClassName() << "('" << getName() << "', " << getLimitState().getName() << ")\n";

  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumCalls(" << getMaximumCalls() << ")\n";
  oss << getName() << ".setMaximumCoefficientOfVariation(" << getMaximumCoefficientOfVariation() << ")\n";
  if (getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumElapsedTime(" << getMaximumElapsedTime() << ")\n";
  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";

  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  return oss;
}


bool SimulationReliabilityAnalysis::analysisLaunched() const
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
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void SimulationReliabilityAnalysis::load(Advocate & adv)
{
  ReliabilityAnalysis::load(adv);
  WithStopCriteriaAnalysis::load(adv);
  adv.loadAttribute("seed_", seed_);
  adv.loadAttribute("blockSize_", blockSize_);
  adv.loadAttribute("result_", result_);
}
}
