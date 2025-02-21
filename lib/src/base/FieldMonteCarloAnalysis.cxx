//                                               -*- C++ -*-
/**
 *  @brief Computes Monte Carlo analysis
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/FieldMonteCarloAnalysis.hxx"

#include <openturns/RandomGenerator.hxx>
#include <openturns/PersistentObjectFactory.hxx>

#include <limits>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(FieldMonteCarloAnalysis)

static Factory<FieldMonteCarloAnalysis> Factory_FieldMonteCarloAnalysis;

/* Default constructor */
FieldMonteCarloAnalysis::FieldMonteCarloAnalysis()
  : SimulationAnalysis()
  , WithStopCriteriaAnalysis()
  , karhunenLoeveAnalysis_()
{
  setMaximumCalls(1000);
  isDeterministicAnalysis_ = false;
}


/* Constructor with parameters */
FieldMonteCarloAnalysis::FieldMonteCarloAnalysis(const String& name, const PhysicalModel& physicalModel)
  : SimulationAnalysis(name, physicalModel)
  , WithStopCriteriaAnalysis()
  , karhunenLoeveAnalysis_()
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
}


void FieldMonteCarloAnalysis::launch()
{
  // check
  if (!getPhysicalModel().getRestrictedFunction(getInterestVariables()).getOutputDescription().getSize())
    throw InvalidDimensionException(HERE) << "The outputs of interest "
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

  const Scalar maxTime = getMaximumElapsedTime() > 0 ? getMaximumElapsedTime() : std::numeric_limits<double>::max();
  Scalar elapsedTime = 0.0;
  const Scalar startTime = TimeCriteria::Now();
  UnsignedInteger outerSampling = 0;

  // We loop if there remains some outer sampling is greater than the limit or has not been computed yet.
  ProcessSample processSample(getPhysicalModel().getMeshModel().getMesh(), 0, getInterestVariables().getSize());
  while (!stopRequested_
         && (outerSampling < maximumOuterSampling)
         && (elapsedTime < maxTime))
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

  // set KL analysis
  DataFieldModel model("model", getPhysicalModel().getMeshModel(), processSample);
  karhunenLoeveAnalysis_ = FieldKarhunenLoeveAnalysis("analysis", model);

  // set design of experiments
  karhunenLoeveAnalysis_.result_.designOfExperiment_.setInputSample(effectiveInputSample);

  informationMessage_ = "Post processing in progress";
  notify("informationMessageUpdated");

  // Compute mean/quantiles process sample
  karhunenLoeveAnalysis_.result_.meanSample_ = processSample.computeMean().getValues();
  karhunenLoeveAnalysis_.result_.lowerQuantileSample_ = processSample.computeQuantilePerComponent(karhunenLoeveAnalysis_.quantileLevel_).getValues();
  karhunenLoeveAnalysis_.result_.upperQuantileSample_ = processSample.computeQuantilePerComponent(1 - karhunenLoeveAnalysis_.quantileLevel_).getValues();

  karhunenLoeveAnalysis_.launch();
  // set result
  karhunenLoeveAnalysis_.result_.elapsedTime_ = TimeCriteria::Now() - startTime;

}

bool FieldMonteCarloAnalysis::hasValidResult() const
{
  return getResult().getProcessSample().getSize() != 0;
}


Parameters FieldMonteCarloAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "Monte Carlo");
  param.add("Outputs of interest", getInterestVariables().__str__());
  param.add(WithStopCriteriaAnalysis::getParameters(false));
  param.add("Karhunen-Loeve threshold", getKarhunenLoeveThreshold());
  param.add(SimulationAnalysis::getParameters());

  return param;
}


String FieldMonteCarloAnalysis::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);
  oss << getName() << " = persalys.FieldMonteCarloAnalysis('" << getName() << "', " << getPhysicalModel().getName() << ")\n";
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
  oss << getName() << ".setKarhunenLoeveThreshold("
      << getKarhunenLoeveThreshold() << ")\n";
  oss << getName() << ".setQuantileLevel("
      << getQuantileLevel() << ")\n";

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
      << karhunenLoeveAnalysis_.__repr__();

  return oss;
}


/* Method save() stores the object through the StorageManager */
void FieldMonteCarloAnalysis::save(Advocate & adv) const
{
  SimulationAnalysis::save(adv);
  WithStopCriteriaAnalysis::save(adv);
  adv.saveAttribute("karhunenLoeveAnalysis_", karhunenLoeveAnalysis_);
}


/* Method load() reloads the object from the StorageManager */
void FieldMonteCarloAnalysis::load(Advocate & adv)
{
  SimulationAnalysis::load(adv);
  WithStopCriteriaAnalysis::load(adv);
  if (adv.hasAttribute("karhunenLoeveAnalysis_"))
    adv.loadAttribute("karhunenLoeveAnalysis_", karhunenLoeveAnalysis_);
  else
  {
    karhunenLoeveAnalysis_ = FieldKarhunenLoeveAnalysis();
    Scalar karhunenLoeveThreshold;
    adv.loadAttribute("karhunenLoeveThreshold_", karhunenLoeveThreshold);
    Scalar quantileLevel;
    adv.loadAttribute("quantileLevel_", quantileLevel);
    FieldMonteCarloResult result;
    adv.loadAttribute("result_", result);
    karhunenLoeveAnalysis_.result_ = result;
    setKarhunenLoeveThreshold(karhunenLoeveThreshold);
    setQuantileLevel(quantileLevel);
  }
}
}
