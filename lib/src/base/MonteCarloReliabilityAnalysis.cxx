//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis with the Monte Carlo method
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
#include "otgui/MonteCarloReliabilityAnalysis.hxx"

#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MonteCarloReliabilityAnalysis);

static Factory<MonteCarloReliabilityAnalysis> RegisteredFactory;

/* Default constructor */
MonteCarloReliabilityAnalysis::MonteCarloReliabilityAnalysis()
  : ReliabilityAnalysis()
  , WithStopCriteriaAnalysis()
  , seed_(ResourceMap::GetAsNumericalScalar("RandomGenerator-InitialSeed"))
  , timeCriteria_()
{
}


/* Constructor with parameters */
MonteCarloReliabilityAnalysis::MonteCarloReliabilityAnalysis(const String & name,
                                                             const LimitState & limitState)
  : ReliabilityAnalysis(name, limitState)
  , WithStopCriteriaAnalysis()
  , seed_(ResourceMap::GetAsNumericalScalar("RandomGenerator-InitialSeed"))
  , timeCriteria_()
{
}


/* Virtual constructor */
MonteCarloReliabilityAnalysis* MonteCarloReliabilityAnalysis::clone() const
{
  return new MonteCarloReliabilityAnalysis(*this);
}


void MonteCarloReliabilityAnalysis::UpdateProgressValue(double percent, void * data)
{
  AnalysisStruct * analysisStruct = (AnalysisStruct*) data;
  if (!analysisStruct)
    return;

  if (analysisStruct->analysis_->getMaximumCalls()< (UnsignedInteger)std::numeric_limits<int>::max())
  {
    analysisStruct->analysis_->progressValue_ = (int) percent;
    analysisStruct->analysis_->notify("progressValueChanged");
  }

  OSS oss;
  oss << "Number of iterations = " << analysisStruct->simulation_->getResult().getOuterSampling() << "\n";
  oss << "Coefficient of variation = " << analysisStruct->simulation_->getResult().getCoefficientOfVariation() << "\n";
  oss << "Elapsed time = " << (float) analysisStruct->analysis_->timeCriteria_.elapsedTime_ / CLOCKS_PER_SEC << " s\n";
  analysisStruct->analysis_->informationMessage_ = oss;
  analysisStruct->analysis_->notify("informationMessageUpdated");
}


void MonteCarloReliabilityAnalysis::run()
{
  try
  {
    // clear result
    stopRequested_ = false;
    timeCriteria_ = TimeCriteria();
    result_ = MonteCarloReliabilityResult();

    // initialization
    RandomGenerator::SetSeed(getSeed());

    Description outputName(1);
    outputName[0] = getLimitState().getOutputName();
    NumericalMathFunction function(getPhysicalModel().getRestrictedFunction(outputName));
    function.enableHistory();
    function.clearHistory();

    Event event(RandomVector(function, getPhysicalModel().getInputRandomVector()), getLimitState().getOperator(), getLimitState().getThreshold());
    event.setDescription(outputName);

    // create algo
    MonteCarlo algo = MonteCarlo(event);
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
    AnalysisStruct analysisStruc(this, &algo);
    algo.setProgressCallback(&UpdateProgressValue, &analysisStruc);

    algo.run();

    // set results
    // get convergence graph at level 0.95
    Graph graph = algo.drawProbabilityConvergence();
    result_ = MonteCarloReliabilityResult(algo.getResult(),
                                          function.getHistoryOutput().getSample(),
                                          graph.getDrawables()[0].getData(),
                                          graph.getDrawables()[1].getData(),
                                          graph.getDrawables()[2].getData());

    result_.elapsedTime_ = (float)timeCriteria_.elapsedTime_/CLOCKS_PER_SEC;

    function.disableHistory();

    notify("analysisFinished");
  }
  catch (std::exception & ex)
  {
    errorMessage_ = ex.what();
    notify("analysisBadlyFinished");
  }
}


void MonteCarloReliabilityAnalysis::stop()
{
  AnalysisImplementation::stop();
  timeCriteria_.stop();
}


UnsignedInteger MonteCarloReliabilityAnalysis::getSeed() const
{
  return seed_;
}


void MonteCarloReliabilityAnalysis::setSeed(const UnsignedInteger seed)
{
  seed_ = seed;
}


MonteCarloReliabilityResult MonteCarloReliabilityAnalysis::getResult() const
{
  return result_;
}


String MonteCarloReliabilityAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.MonteCarloReliabilityAnalysis('" << getName() << "', " << getLimitState().getName() << ")\n";
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumCalls(" << getMaximumCalls() << ")\n";
  oss << getName() << ".setMaximumCoefficientOfVariation(" << getMaximumCoefficientOfVariation() << ")\n";
  if (getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumElapsedTime(" << getMaximumElapsedTime() << ")\n";
  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  return oss;
}


bool MonteCarloReliabilityAnalysis::analysisLaunched() const
{
  return getResult().getSimulationResult().getOuterSampling() != 0;
}


/* String converter */
String MonteCarloReliabilityAnalysis::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " limitState=" << getLimitState()
      << " maximumCalls=" << getMaximumCalls()
      << " maximumCoefficientOfVariation=" << getMaximumCoefficientOfVariation()
      << " maximumElapsedTime=" << getMaximumElapsedTime()
      << " blockSize=" << getBlockSize()
      << " seed=" << getSeed();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void MonteCarloReliabilityAnalysis::save(Advocate & adv) const
{
  ReliabilityAnalysis::save(adv);
  WithStopCriteriaAnalysis::save(adv);
  adv.saveAttribute("seed_", seed_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void MonteCarloReliabilityAnalysis::load(Advocate & adv)
{
  ReliabilityAnalysis::load(adv);
  WithStopCriteriaAnalysis::load(adv);
  adv.loadAttribute("seed_", seed_);
  adv.loadAttribute("result_", result_);
}
}