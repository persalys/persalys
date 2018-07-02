//                                               -*- C++ -*-
/**
 *  @brief Computes Monte Carlo analysis
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/FieldMonteCarloAnalysis.hxx"

#include <openturns/RandomGenerator.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/KarhunenLoeveSVDAlgorithm.hxx>
#include <openturns/PiecewiseLinearEvaluation.hxx>
#include <openturns/InverseTrendTransform.hxx>
#include <openturns/NonStationaryCovarianceModelFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(FieldMonteCarloAnalysis)

static Factory<FieldMonteCarloAnalysis> Factory_FieldMonteCarloAnalysis;

CLASSNAMEINIT(CovFunctionEvaluation)
static Factory<CovFunctionEvaluation> Factory_CovFunctionEvaluation;


/* Default constructor */
FieldMonteCarloAnalysis::FieldMonteCarloAnalysis()
  : SimulationAnalysis()
  , WithStopCriteriaAnalysis()
  , karhunenLoeveThreshold_(1.e-5)
  , quantileLevel_(0.05)
  , result_()
{
  setMaximumCalls(1000);
  isDeterministicAnalysis_ = false;
}


/* Constructor with parameters */
FieldMonteCarloAnalysis::FieldMonteCarloAnalysis(const String& name, const PhysicalModel& physicalModel)
  : SimulationAnalysis(name, physicalModel)
  , WithStopCriteriaAnalysis()
  , karhunenLoeveThreshold_(1.e-5)
  , quantileLevel_(0.05)
  , result_()
{
  setMaximumCalls(1000);
  isDeterministicAnalysis_ = false;
}


/* Virtual constructor */
FieldMonteCarloAnalysis* FieldMonteCarloAnalysis::clone() const
{
  return new FieldMonteCarloAnalysis(*this);
}


void FieldMonteCarloAnalysis::initialize()
{
  SimulationAnalysis::initialize();
  result_ = FieldMonteCarloResult();
}


void FieldMonteCarloAnalysis::launch()
{
  // check
  if (getMaximumCalls() < getBlockSize())
    throw InvalidValueException(HERE) << "The maximum calls number (" << getMaximumCalls()
                                      << ") can not be lesser than the block size (" << getBlockSize() << ")";
  if (!getPhysicalModel().getRestrictedFunction(getInterestVariables()).getOutputDescription().getSize())
    throw InvalidDimensionException(HERE) << "The outputs to be analysed "
                                          << getInterestVariables() << " are not outputs of the model "
                                          << getPhysicalModel().getOutputNames();

  // initialization
  RandomGenerator::SetSeed(getSeed());

  Sample effectiveInputSample(0, getPhysicalModel().getStochasticInputNames().getSize());
  effectiveInputSample.setDescription(getPhysicalModel().getStochasticInputNames());

  const bool maximumOuterSamplingSpecified = getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max();
  const UnsignedInteger maximumOuterSampling = maximumOuterSamplingSpecified ? static_cast<UnsignedInteger>(ceil(1.0 * getMaximumCalls() / getBlockSize())) : (UnsignedInteger)std::numeric_limits<int>::max();
  const UnsignedInteger modulo = maximumOuterSamplingSpecified ? getMaximumCalls() % getBlockSize() : 0;
  const UnsignedInteger lastBlockSize = modulo == 0 ? getBlockSize() : modulo;

  Scalar elapsedTime = 0.0;
  const Scalar startTime = TimeCriteria::Now();
  UnsignedInteger outerSampling = 0;

  // We loop if there remains some outer sampling is greater than the limit or has not been computed yet.
  ProcessSample processSample(getPhysicalModel().getMeshModel().getMesh(), 0, getInterestVariables().getSize());
  while (!stopRequested_
         && (outerSampling < maximumOuterSampling)
         && (elapsedTime < getMaximumElapsedTime()))
  {
    // progress
    if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    {
      progressValue_ = (int) (outerSampling * 100 / maximumOuterSampling);
      notify("progressValueChanged");
    }
    // information message
    OSS oss;
    oss << "Number of iterations = " << processSample.getSize() << "\n";
    oss << "Elapsed time = " << elapsedTime << " s\n";
    informationMessage_ = oss;
    notify("informationMessageUpdated");

    // the last block can be smaller
    const UnsignedInteger effectiveBlockSize = outerSampling < (maximumOuterSampling - 1) ? getBlockSize() : lastBlockSize;

    // get block input sample
    const Sample blockInputSample(generateInputSample(effectiveBlockSize));

    // Perform a block of simulations
    try
    {
      ProcessSample processSample_block(getPhysicalModel().getRestrictedPointToFieldFunction(getInterestVariables())(blockInputSample));
      for (UnsignedInteger i = 0; i < processSample_block.getSize(); ++i)
        processSample.add(processSample_block[i]);
    }
    catch (std::exception & ex)
    {
      failedInputSample_ = blockInputSample;
      warningMessage_ = ex.what();
    }

    if (!failedInputSample_.getSize())
    {
      // if succeed fill samples
      effectiveInputSample.add(blockInputSample);
    }
    else
    {
      // exit the while section. Stop the analysis
      break;
    }

    // stop criteria
    elapsedTime = TimeCriteria::Now() - startTime;
    ++outerSampling;
  }

  // check
  if (!processSample.getSize())
    throw InvalidValueException(HERE) << "Monte Carlo analysis failed. The output process sample is empty. " << warningMessage_;

  // set design of experiments
  result_.designOfExperiment_.setInputSample(effectiveInputSample);

  informationMessage_ = "Post processing in progress";
  notify("informationMessageUpdated");

  // Compute mean/quantiles process sample
  result_.meanSample_ = processSample.computeMean().getValues();
  result_.lowerQuantileSample_ = processSample.computeQuantilePerComponent(quantileLevel_).getValues();
  result_.upperQuantileSample_ = processSample.computeQuantilePerComponent(1 - quantileLevel_).getValues();

  // Compute the KL decomposition of the output and correlation
  informationMessage_ = "Karhunen-Loeve algorithm in progress";
  notify("informationMessageUpdated");
  // get mesh info
  const Point vertices(getPhysicalModel().getMeshModel().getMesh().getVertices().asPoint());
  try
  {
    for (UnsignedInteger i = 0; i < getInterestVariables().getSize(); ++i)
    {
      ProcessSample ps_i(processSample.getMarginal(i));
      // Compute the KL decomposition
      KarhunenLoeveSVDAlgorithm algoKL(ps_i, getKarhunenLoeveThreshold());
      algoKL.run();
      result_.karhunenLoeveResults_.add(algoKL.getResult());
      // compute correlation
      TrendTransform meanTransform(PiecewiseLinearEvaluation(vertices, result_.meanSample_.getMarginal(i)), getPhysicalModel().getMeshModel().getMesh());
      InverseTrendTransform meanInverseTransform(meanTransform.getInverse());
      ProcessSample sample_centered(meanInverseTransform(ps_i));

      CovarianceModel covariance(NonStationaryCovarianceModelFactory().build(sample_centered, true));
      Function f(new CovFunctionEvaluation(covariance));
      result_.correlationFunction_.add(f);

      Sample xi_sample(algoKL.getResult().project(sample_centered));
      xi_sample.setDescription(Description::BuildDefault(algoKL.getResult().getEigenValues().getSize(), "Xi_"));
      result_.xiSamples_.add(xi_sample);
    }
  }
  catch (std::exception & ex)
  {
    warningMessage_ = ex.what();
  }

  // set result
  result_.processSample_ = processSample;
  result_.elapsedTime_ = TimeCriteria::Now() - startTime;
}


Scalar FieldMonteCarloAnalysis::getKarhunenLoeveThreshold() const
{
  return karhunenLoeveThreshold_;
}


void FieldMonteCarloAnalysis::setKarhunenLoeveThreshold(const Scalar threshold)
{
  karhunenLoeveThreshold_ = threshold;
}


Scalar FieldMonteCarloAnalysis::getQuantileLevel() const
{
  return quantileLevel_;
}


void FieldMonteCarloAnalysis::setQuantileLevel(const Scalar level)
{
  if (!(level >= 0.0) || !(level <= 1.0))
    throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";
  quantileLevel_ = level;
}


FieldMonteCarloResult FieldMonteCarloAnalysis::getResult() const
{
  return result_;
}


bool FieldMonteCarloAnalysis::hasValidResult() const
{
  return result_.getProcessSample().getSize() != 0;
}


Parameters FieldMonteCarloAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "Monte Carlo");
  param.add("Outputs of interest", getInterestVariables().__str__());
  String time = "- (s)";
  if (getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
    time = (OSS() << getMaximumElapsedTime()).str() + "(s)";
  param.add("Maximum elapsed time", time);
  String maxCalls = "-";
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    maxCalls = (OSS() << getMaximumCalls()).str();
  param.add("Maximum calls", maxCalls);
  param.add("Block size", getBlockSize());
  param.add("Karhunen-Loeve threshold", getKarhunenLoeveThreshold());
  param.add("Seed", getSeed());

  return param;
}


String FieldMonteCarloAnalysis::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);
  oss << getName() << " = otguibase.FieldMonteCarloAnalysis('" << getName() << "', " << getPhysicalModel().getName() << ")\n";
  if (getInterestVariables().getSize() < getPhysicalModel().getSelectedOutputsNames().getSize())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumCalls(" << getMaximumCalls() << ")\n";
  if (getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumElapsedTime(" << getMaximumElapsedTime() << ")\n";
  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";
  oss << getName() << ".setKarhunenLoeveThreshold(" << getKarhunenLoeveThreshold() << ")\n";
  oss << getName() << ".setQuantileLevel(" << getQuantileLevel() << ")\n";

  return oss;
}


/* String converter */
String FieldMonteCarloAnalysis::__repr__() const
{
  OSS oss;
  oss << PhysicalModelAnalysis::__repr__()
      << WithStopCriteriaAnalysis::__repr__()
      << " seed=" << getSeed()
      << " blockSize=" << getBlockSize()
      << " Karhunen-Loeve threshold=" << getKarhunenLoeveThreshold()
      << " quantile level=" << getQuantileLevel();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void FieldMonteCarloAnalysis::save(Advocate & adv) const
{
  SimulationAnalysis::save(adv);
  WithStopCriteriaAnalysis::save(adv);
  adv.saveAttribute("karhunenLoeveThreshold_", karhunenLoeveThreshold_);
  adv.saveAttribute("quantileLevel_", quantileLevel_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void FieldMonteCarloAnalysis::load(Advocate & adv)
{
  SimulationAnalysis::load(adv);
  WithStopCriteriaAnalysis::load(adv);
  adv.loadAttribute("karhunenLoeveThreshold_", karhunenLoeveThreshold_);
  adv.loadAttribute("quantileLevel_", quantileLevel_);
  adv.loadAttribute("result_", result_);
}
}
