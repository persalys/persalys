//                                               -*- C++ -*-
/**
 *  @brief SobolAnalysis computes the Sobol indices
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/SobolAnalysis.hxx"

#include <openturns/RandomGenerator.hxx>
#include <openturns/SobolSimulationAlgorithm.hxx>
#include <openturns/MartinezSensitivityAlgorithm.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/ResourceMap.hxx>
#include <openturns/MemoizeFunction.hxx>
#include <openturns/DatabaseFunction.hxx>

#include <limits>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(SobolAnalysis)

static Factory<SobolAnalysis> Factory_SobolAnalysis;

/* Default constructor */
SobolAnalysis::SobolAnalysis()
  : SimulationAnalysis()
  , WithStopCriteriaAnalysis()
  , maximumConfidenceIntervalLength_(ResourceMap::GetAsScalar("SobolSimulationAlgorithm-DefaultIndexQuantileEpsilon"))
  , replicationSize_(ResourceMap::GetAsUnsignedInteger("SobolSimulationAlgorithm-DefaultBlockSize"))
  , confidenceLevel_(1 - ResourceMap::GetAsScalar("SobolSimulationAlgorithm-DefaultIndexQuantileLevel"))
  , result_()
{
  isDeterministicAnalysis_ = false;
}


/* Constructor with parameters */
SobolAnalysis::SobolAnalysis(const String& name, const PhysicalModel& physicalModel)
  : SimulationAnalysis(name, physicalModel)
  , maximumConfidenceIntervalLength_(ResourceMap::GetAsScalar("SobolSimulationAlgorithm-DefaultIndexQuantileEpsilon"))
  , replicationSize_(ResourceMap::GetAsUnsignedInteger("SobolSimulationAlgorithm-DefaultBlockSize"))
  , confidenceLevel_(1 - ResourceMap::GetAsScalar("SobolSimulationAlgorithm-DefaultIndexQuantileLevel"))
  , result_()
{
  isDeterministicAnalysis_ = false;
}


/* Virtual constructor */
SobolAnalysis* SobolAnalysis::clone() const
{
  return new SobolAnalysis(*this);
}


Scalar SobolAnalysis::getMaximumConfidenceIntervalLength() const
{
  return maximumConfidenceIntervalLength_;
}


void SobolAnalysis::setMaximumConfidenceIntervalLength(const Scalar length)
{
  maximumConfidenceIntervalLength_ = length;
}


UnsignedInteger SobolAnalysis::getReplicationSize() const
{
  return replicationSize_;
}


void SobolAnalysis::setReplicationSize(const UnsignedInteger size)
{
  if (size < 2)
    throw InvalidValueException(HERE) << "The replication size must be greater than 2";

  replicationSize_ = size;
}


Scalar SobolAnalysis::getConfidenceLevel() const
{
  return confidenceLevel_;
}


void SobolAnalysis::setConfidenceLevel(const Scalar level)
{
  if (!(level >= 0.0) || !(level < 1.0))
    throw InvalidArgumentException(HERE) << "Confidence level value should be in ]0,1[. Here, confidence level=" << level;
  confidenceLevel_ = level;
 }


void SobolAnalysis::initialize()
{
  SimulationAnalysis::initialize();
  result_ = SobolResult();
}


struct SobolAnalysisStruct
{
  SobolAnalysisStruct(SobolAnalysis* analysis, SobolSimulationAlgorithm* simulation)
    : analysis_(analysis)
    , simulation_(simulation)
  {
  };

  virtual ~SobolAnalysisStruct() {};

  SobolAnalysis * analysis_;
  SobolSimulationAlgorithm * simulation_;
};


void SobolAnalysis::UpdateProgressValue(double percent, void * data)
{
  SobolAnalysisStruct * analysisStruct = static_cast<SobolAnalysisStruct*>(data);
  if (!analysisStruct)
    return;

  OSS oss;

  // display confidence intervals length
  const UnsignedInteger nbInputs = analysisStruct->analysis_->getPhysicalModel().getStochasticInputNames().getSize();
  if (analysisStruct->simulation_->getResult().getFirstOrderIndicesDistribution().getDimension() == nbInputs)
  {
    const Scalar level = 1 - analysisStruct->analysis_->getConfidenceLevel();
    Scalar foCILength = 0.;
    Scalar toCILength = 0.;
    for (UnsignedInteger i = 0; i < nbInputs; ++i)
    {
      const Distribution distFO(analysisStruct->simulation_->getResult().getFirstOrderIndicesDistribution().getMarginal(i));
      foCILength = std::max(foCILength, distFO.computeScalarQuantile(level, true) - distFO.computeScalarQuantile(level));

      const Distribution distTO(analysisStruct->simulation_->getResult().getTotalOrderIndicesDistribution().getMarginal(i));
      toCILength = std::max(toCILength, distTO.computeScalarQuantile(level, true) - distTO.computeScalarQuantile(level));
    }
    oss << "First order indices confidence interval length = " << foCILength << "\n";
    oss << "Total indices confidence interval length = " << toCILength << "\n";
  }

  // set progress value
  if (analysisStruct->analysis_->getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
  {
    analysisStruct->analysis_->progressValue_ = (int) percent;
    analysisStruct->analysis_->notify("progressValueChanged");
    oss << "Number of evaluations = " << (int)((int)percent * analysisStruct->analysis_->getMaximumCalls() * 0.01) << "\n";
  }
  else
  {
    oss << "Number of iterations = " << analysisStruct->simulation_->getResult().getOuterSampling() << "\n";
  }

  // set information message
  oss << "Elapsed time = " << analysisStruct->analysis_->timeCriteria_.getElapsedTime() << " s\n";
  analysisStruct->analysis_->informationMessage_ = oss;
  analysisStruct->analysis_->notify("informationMessageUpdated");
}


void SobolAnalysis::launch()
{
  // check
  if (getMaximumCalls() < (getReplicationSize() * (getPhysicalModel().getStochasticInputNames().getSize() + 2)))
    throw InvalidValueException(HERE) << "The replication size (" << getReplicationSize()
                                      << ") cannot be greater than: max_calls (" << getMaximumCalls()
                                      << ") / (number_of_inputs (" << getPhysicalModel().getStochasticInputNames().getSize()
                                      << ") + 2)="
                                      << (getMaximumCalls() / (getPhysicalModel().getStochasticInputNames().getSize() + 2));

  const UnsignedInteger nbInputs = getPhysicalModel().getStochasticInputNames().getSize();
  const UnsignedInteger nbOutputs = getInterestVariables().getSize();

  const bool maximumOuterSamplingSpecified = getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max();
  const UnsignedInteger maximumOuterSampling = maximumOuterSamplingSpecified ? static_cast<UnsignedInteger>(ceil(1.0 * getMaximumCalls() / (getReplicationSize() * (2 + nbInputs)))) : (UnsignedInteger)std::numeric_limits<int>::max();
  const Scalar level = 1 - getConfidenceLevel();

  // get function
  MemoizeFunction function(getPhysicalModel().getRestrictedFunction(getInterestVariables()));
  function.enableHistory();
  function.clearHistory();

  // initialization
  RandomGenerator::SetSeed(getSeed());

  MartinezSensitivityAlgorithm estimator;
  estimator.setUseAsymptoticDistribution(true);
  SobolSimulationAlgorithm algo(getPhysicalModel().getDistribution(), function, estimator);

  algo.setMaximumOuterSampling(maximumOuterSampling);
  algo.setBatchSize(getBlockSize());
  algo.setBlockSize(getReplicationSize());
  algo.setIndexQuantileEpsilon(getMaximumConfidenceIntervalLength());
  algo.setIndexQuantileLevel(level);

  // set callbacks
  timeCriteria_.setStartTime(TimeCriteria::Now());
  timeCriteria_.setMaxElapsedTime(getMaximumElapsedTime() > 0 ? getMaximumElapsedTime() : std::numeric_limits<double>::max());
  algo.setStopCallback(&WithStopCriteriaAnalysis::Stop, &timeCriteria_);
  SobolAnalysisStruct analysisStruc(this, &algo);
  algo.setProgressCallback(&UpdateProgressValue, &analysisStruc);

  // run algo
  algo.run();

  // set results
  result_.outputNames_ = getInterestVariables();
  result_.callsNumber_ = function.getOutputHistory().getSize();
  result_.elapsedTime_ = timeCriteria_.getElapsedTime();

  result_.firstOrderIndices_ = Sample(0, nbInputs);
  result_.firstOrderIndices_.setDescription(getPhysicalModel().getStochasticInputNames());
  result_.totalIndices_ = Sample(0, nbInputs);

  SobolSimulationResult sobolResult(algo.getResult());
  const UnsignedInteger outerSampling = sobolResult.getOuterSampling();

  Scalar foCILength = 0.;
  Scalar toCILength = 0.;
  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    const Distribution distFO(sobolResult.getFirstOrderIndicesDistribution().getMarginal(i));
    foCILength = std::max(foCILength, distFO.computeScalarQuantile(level, true) - distFO.computeScalarQuantile(level));

    const Distribution distTO(sobolResult.getTotalOrderIndicesDistribution().getMarginal(i));
    toCILength = std::max(toCILength, distTO.computeScalarQuantile(level, true) - distTO.computeScalarQuantile(level));
  }
  result_.confidenceIntervalLength_ = std::max(foCILength, toCILength);

  // compute indices

  // if one output
  if (nbOutputs == 1)
  {
    result_.firstOrderIndices_.add(sobolResult.getFirstOrderIndicesEstimate());
    result_.totalIndices_.add(sobolResult.getTotalOrderIndicesEstimate());
    result_.firstOrderIndicesInterval_.add(sobolResult.getFirstOrderIndicesDistribution().computeBilateralConfidenceInterval(getConfidenceLevel()));
    result_.totalIndicesInterval_.add(sobolResult.getTotalOrderIndicesDistribution().computeBilateralConfidenceInterval(getConfidenceLevel()));
  }
  // if more than 1 output
  else
  {
    // for each output, relaunch the algo to get the indices
    // (we do not want the aggregated indices)
    // We use DatabaseFunction because no more evaluations are needed
    for (UnsignedInteger i = 0; i < nbOutputs; ++i)
    {
      informationMessage_ = OSS() << "Indices computation in progress for the output " << getInterestVariables()[i];
      notify("informationMessageUpdated");

      // initialization
      RandomGenerator::SetSeed(getSeed());

      const Description outputName(1, getInterestVariables()[i]);
      MartinezSensitivityAlgorithm estimator_i;
      estimator_i.setUseAsymptoticDistribution(true);
      SobolSimulationAlgorithm algo_i(getPhysicalModel().getDistribution(),
                                      DatabaseFunction(function.getInputHistory(), function.getOutputHistory().getMarginal(i)),
                                      estimator_i);

      algo_i.setMaximumOuterSampling(outerSampling);
      algo_i.setBatchSize(getReplicationSize());
      algo_i.setBlockSize(getReplicationSize());
      algo_i.setIndexQuantileEpsilon(-1.0);
      algo_i.setIndexQuantileLevel(1.0 - getConfidenceLevel());

      // run algo
      algo_i.run();

      // set results
      SobolSimulationResult sobolResult_i(algo_i.getResult());
      result_.firstOrderIndices_.add(sobolResult_i.getFirstOrderIndicesEstimate());
      result_.totalIndices_.add(sobolResult_i.getTotalOrderIndicesEstimate());
      result_.firstOrderIndicesInterval_.add(sobolResult_i.getFirstOrderIndicesDistribution().computeBilateralConfidenceInterval(getConfidenceLevel()));
      result_.totalIndicesInterval_.add(sobolResult_i.getTotalOrderIndicesDistribution().computeBilateralConfidenceInterval(getConfidenceLevel()));
    }
    result_.aggregatedFirstOrderIndices_ = sobolResult.getFirstOrderIndicesEstimate();
    result_.aggregatedTotalIndices_ = sobolResult.getTotalOrderIndicesEstimate();
    result_.aggregatedFirstOrderIndicesInterval_ = sobolResult.getFirstOrderIndicesDistribution().computeBilateralConfidenceInterval(getConfidenceLevel());
    result_.aggregatedTotalIndicesInterval_ = sobolResult.getTotalOrderIndicesDistribution().computeBilateralConfidenceInterval(getConfidenceLevel());
  }
  function.disableHistory();
}


void SobolAnalysis::stop()
{
  AnalysisImplementation::stop();
  timeCriteria_.stop();
}


SobolResult SobolAnalysis::getResult() const
{
  return result_;
}


Parameters SobolAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "Sobol");
  param.add("Outputs of interest", getInterestVariables().__str__());
  param.add("Maximum confidence interval length", getMaximumConfidenceIntervalLength());
  param.add(WithStopCriteriaAnalysis::getParameters(false));
  param.add("Replication size", getReplicationSize());
  param.add(SimulationAnalysis::getParameters());
  param.add("Confidence level", getConfidenceLevel());

  return param;
}


String SobolAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = persalys.SobolAnalysis('" << getName() << "', " << getPhysicalModel().getName() << ")\n";
  if (getInterestVariables().getSize() < getPhysicalModel().getSelectedOutputsNames().getSize())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumCalls(" << getMaximumCalls() << ")\n";
  oss << getName() << ".setMaximumConfidenceIntervalLength(" << getMaximumConfidenceIntervalLength() << ")\n";
  if (getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumElapsedTime(" << getMaximumElapsedTime() << ")\n";
  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << getName() << ".setReplicationSize(" << getReplicationSize() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";
  oss << getName() << ".setConfidenceLevel(" << getConfidenceLevel() << ")\n";

  return oss;
}


bool SobolAnalysis::hasValidResult() const
{
  return result_.getFirstOrderIndices().getSize() == getInterestVariables().getSize();
}


bool SobolAnalysis::canBeLaunched(String &errorMessage) const
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
String SobolAnalysis::__repr__() const
{
  OSS oss;
  oss << PhysicalModelAnalysis::__repr__()
      << WithStopCriteriaAnalysis::__repr__()
      << " seed=" << getSeed()
      << " maximum confidence interval length=" << getMaximumConfidenceIntervalLength()
      << " blockSize=" << getBlockSize()
      << " replicationSize=" << getReplicationSize()
      << " confidenceLevel=" << getConfidenceLevel();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SobolAnalysis::save(Advocate & adv) const
{
  SimulationAnalysis::save(adv);
  WithStopCriteriaAnalysis::save(adv);
  adv.saveAttribute("maximumConfidenceIntervalLength_", maximumConfidenceIntervalLength_);
  adv.saveAttribute("replicationSize_", replicationSize_);
  adv.saveAttribute("confidenceLevel_", confidenceLevel_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void SobolAnalysis::load(Advocate & adv)
{
  SimulationAnalysis::load(adv);
  WithStopCriteriaAnalysis::load(adv);
  adv.loadAttribute("maximumConfidenceIntervalLength_", maximumConfidenceIntervalLength_);
  adv.loadAttribute("replicationSize_", replicationSize_);
  adv.loadAttribute("confidenceLevel_", confidenceLevel_);
  adv.loadAttribute("result_", result_);
}
}
