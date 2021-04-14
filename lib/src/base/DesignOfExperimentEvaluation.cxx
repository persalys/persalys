//                                               -*- C++ -*-
/**
 *  @brief Evaluates the design of experiments
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/DesignOfExperimentEvaluation.hxx"

#include "persalys/BaseTools.hxx"
#include "persalys/DataAnalysis.hxx"

#ifdef PERSALYS_HAVE_YACS
#include "persalys/YACSEvaluation.hxx"
#endif

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/SpecFunc.hxx>
#include <openturns/MemoizeEvaluation.hxx>
#include <openturns/MarginalEvaluation.hxx>
#include <openturns/SymbolicEvaluation.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(DesignOfExperimentEvaluation)

static Factory<DesignOfExperimentEvaluation> Factory_DesignOfExperimentEvaluation;

/* Default constructor */
DesignOfExperimentEvaluation::DesignOfExperimentEvaluation()
  : SimulationAnalysis()
  , originalInputSample_()
  , result_()
{
}


/* Constructor with parameters */
DesignOfExperimentEvaluation::DesignOfExperimentEvaluation(const String& name, const PhysicalModel& physicalModel)
  : SimulationAnalysis(name, physicalModel)
  , originalInputSample_()
  , result_(DesignOfExperiment(name, physicalModel))
{
}


/* Virtual constructor */
DesignOfExperimentEvaluation* DesignOfExperimentEvaluation::clone() const
{
  return new DesignOfExperimentEvaluation(*this);
}


/** Object name accessor */
void DesignOfExperimentEvaluation::setName(const String& name)
{
  PersistentObject::setName(name);
  // the analysis and designOfExperiment_ must have the same name
  result_.designOfExperiment_.getImplementation()->setName(name);
  notify("nameChanged");
}


void DesignOfExperimentEvaluation::removeAllObservers()
{
  notifyAndRemove("AnalysisItem");
  notifyAndRemove("DesignOfExperimentDefinitionItem");
  notifyAndRemove("Study");
}


void DesignOfExperimentEvaluation::setDesignOfExperiment(const DesignOfExperiment& designOfExperiment)
{
  result_ = DataAnalysisResult(designOfExperiment);
  result_.designOfExperiment_.getImplementation()->initialize();
  SimulationAnalysis::initialize();
  notify("doeChanged");
}


Sample DesignOfExperimentEvaluation::getOriginalInputSample() const
{
  if (!originalInputSample_.getSize())
    originalInputSample_ = generateInputSample(0);
  return originalInputSample_;
}


void DesignOfExperimentEvaluation::initialize()
{
  SimulationAnalysis::initialize();
  // we want to keep the same result_.designOfExperiment_ pointer
  result_.designOfExperiment_.getImplementation()->initialize();
  result_ = DataAnalysisResult(result_.designOfExperiment_);
}


void DesignOfExperimentEvaluation::launch()
{
  // check
  if (!getInterestVariables().getSize())
    throw InvalidDimensionException(HERE) << "You have not defined output variable to be analysed. Set interest variables.";

  const UnsignedInteger inputSampleDimension = getOriginalInputSample().getDimension();
  const UnsignedInteger inputSampleSize = getOriginalInputSample().getSize();
  if (!inputSampleSize)
    throw InvalidArgumentException(HERE) << "The design of experiments input sample is empty";

  if (getBlockSize() > inputSampleSize)
    throw InvalidValueException(HERE) << "The block size (" << getBlockSize()
                                      << ") cannot be greater than the input sample size (" << inputSampleSize << ")";

  // input sample
  const Description inDescription(getOriginalInputSample().getDescription());
  Sample inputSample = Sample(0, inputSampleDimension);
  inputSample.setDescription(inDescription);

  // failed input sample
  failedInputSample_ = Sample(0, inputSampleDimension);
  failedInputSample_.setDescription(inDescription);

  // number of iterations
  const UnsignedInteger nbIter = static_cast<UnsignedInteger>(ceil(1.0 * inputSampleSize / getBlockSize()));
  // last block size
  const UnsignedInteger modulo = inputSampleSize % getBlockSize();
  const UnsignedInteger lastBlockSize = modulo == 0 ? getBlockSize() : modulo;

  // output = f(input)
  Sample outputSample(0, getInterestVariables().getSize());
  outputSample.setDescription(getInterestVariables());

  // time
  TimeCriteria timeCriteria;

  Function function(getPhysicalModel().getFunction(getInterestVariables()));

  // to avoid failing whole blocks we make failed points succeed but mark them with nan
  function.getEvaluation().getImplementation()->setCheckOutput(false);

  // iterations
  for (UnsignedInteger i = 0; i < nbIter; ++i)
  {
    if (stopRequested_)
      break;

    // progress
    progressValue_ = (int) (i * 100 / nbIter);
    notify("progressValueChanged");

    // information message
    informationMessage_ = OSS() << "Elapsed time = " << timeCriteria.getElapsedTime() << " s\n";
    notify("informationMessageUpdated");

    // the last block can be smaller
    const UnsignedInteger effectiveBlockSize = i < (nbIter - 1) ? getBlockSize() : lastBlockSize;

    // get input sample of size effectiveBlockSize
    const UnsignedInteger blockFirstIndex =  i * getBlockSize();
    const Sample blockInputSample(Sample(getOriginalInputSample(), blockFirstIndex, blockFirstIndex + effectiveBlockSize));

    // Perform a block of simulations
    Sample blockOutputSample;
    Sample failedSample;
    try
    {
      blockOutputSample = function(blockInputSample);
    }
    catch (InternalException & ex)
    {
      failedSample = blockInputSample;
      warningMessage_ = ex.what();
    }

    if (!failedSample.getSize())
    {
      outputSample.add(blockOutputSample);
      inputSample.add(blockInputSample);
    }
    else
    {
      failedInputSample_.add(failedSample);
    }
    timeCriteria.incrementElapsedTime();
  }

  // restore default
  function.getEvaluation().getImplementation()->setCheckOutput(true);

  // mark points evaluating to nan as failed
  Indices failedIndices;
  for (UnsignedInteger i = 0; i < outputSample.getSize(); ++i)
  {
    for (UnsignedInteger j = 0; j < outputSample.getDimension(); ++j)
    {
      if (!SpecFunc::IsNormal(outputSample(i, j)))
      {
        failedIndices.add(i);
        failedInputSample_.add(getOriginalInputSample()[i]);
        break;
      }
    }
  }
  for (UnsignedInteger i = 0; i < failedIndices.getSize(); ++ i)
  {
    inputSample.erase(failedIndices[failedIndices.getSize() - 1 - i]);
    outputSample.erase(failedIndices[failedIndices.getSize() - 1 - i]);
  }

  if (!outputSample.getSize())
    throw InvalidRangeException(HERE) << "All the evaluations have failed. Check the model. " << warningMessage_;

  // set design of experiments
  result_.designOfExperiment_.setInputSample(inputSample);
  result_.designOfExperiment_.setOutputSample(outputSample);

  // compute data analysis
  DataAnalysis dataAnalysis("", result_.designOfExperiment_);
  dataAnalysis.setIsConfidenceIntervalRequired(false);
  dataAnalysis.run();

  // set result
  result_ = dataAnalysis.getResult();
  timeCriteria.incrementElapsedTime();
  result_.elapsedTime_ = timeCriteria.getElapsedTime();
}


Sample DesignOfExperimentEvaluation::getNotEvaluatedInputSample() const
{
  const UnsignedInteger originalInputSampleSize = originalInputSample_.getSize();
  const UnsignedInteger inputSampleSize = result_.designOfExperiment_.getInputSample().getSize();
  const UnsignedInteger failedInputSampleSize = failedInputSample_.getSize();
  if ((inputSampleSize + failedInputSampleSize) < originalInputSampleSize)
    return Sample(originalInputSample_, inputSampleSize + failedInputSampleSize, originalInputSampleSize);

  return Sample();
}


DataAnalysisResult DesignOfExperimentEvaluation::getResult() const
{
  return result_;
}


bool DesignOfExperimentEvaluation::canBeLaunched(String &errorMessage) const
{
  // pm must have inputs
  if (!getPhysicalModel().getInputDimension())
    errorMessage = "The physical model must have inputs.";
  return errorMessage.empty();
}


Parameters DesignOfExperimentEvaluation::getParameters() const
{
  Parameters param;

  param.add("Outputs of interest", getInterestVariables().__str__());
  param.add("Sample size", getOriginalInputSample().getSize());

  OSS values;
  const Point minValues(getOriginalInputSample().getMin());
  const Point maxValues(getOriginalInputSample().getMax());
  for (UnsignedInteger i = 0; i < getOriginalInputSample().getDimension(); ++i)
  {
    if (minValues[i] != maxValues[i])
      values << getOriginalInputSample().getDescription()[i] << " : [" << minValues[i] << ", " << maxValues[i] << "]";
    else
      values << getOriginalInputSample().getDescription()[i] << " : " << minValues[i];
    if (i < getOriginalInputSample().getDimension() - 1)
      values << "\n";
  }
  param.add("Values", values);

  param.add("Block size", getBlockSize());

  return param;
}


bool DesignOfExperimentEvaluation::hasValidResult() const
{
  return result_.getDesignOfExperiment().getSample().getSize() != 0;
}


/* Method save() stores the object through the StorageManager */
void DesignOfExperimentEvaluation::save(Advocate& adv) const
{
  SimulationAnalysis::save(adv);
  adv.saveAttribute("originalInputSample_", originalInputSample_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void DesignOfExperimentEvaluation::load(Advocate& adv)
{
  SimulationAnalysis::load(adv);
  adv.loadAttribute("originalInputSample_", originalInputSample_);
  adv.loadAttribute("result_", result_);
  // can open older xml files
  if (!result_.getDesignOfExperiment().getSample().getSize())
  {
    DesignOfExperiment experiment;
    adv.loadAttribute("designOfExperiment_", experiment);
    if (experiment.getSample().getSize())
      result_.designOfExperiment_ = experiment;
  }
}
}
