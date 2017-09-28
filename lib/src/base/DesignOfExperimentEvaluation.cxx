//                                               -*- C++ -*-
/**
 *  @brief Evaluates the design of experiments
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
#include "otgui/DesignOfExperimentEvaluation.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/SpecFunc.hxx>

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(DesignOfExperimentEvaluation);

static Factory<DesignOfExperimentEvaluation> Factory_DesignOfExperimentEvaluation;

/* Default constructor */
DesignOfExperimentEvaluation::DesignOfExperimentEvaluation()
  : SimulationAnalysis()
  , originalInputSample_()
  , failedInputSample_()
{
}


/* Constructor with parameters */
DesignOfExperimentEvaluation::DesignOfExperimentEvaluation(const String& name, const PhysicalModel& physicalModel)
  : SimulationAnalysis(name, physicalModel)
  , originalInputSample_()
  , failedInputSample_()
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
  designOfExperiment_.getImplementation()->setName(name);
}


void DesignOfExperimentEvaluation::setDesignOfExperiment(const DesignOfExperiment& designOfExperiment)
{
  designOfExperiment_ = designOfExperiment;
  initialize();
}


Sample DesignOfExperimentEvaluation::getOriginalInputSample() const
{
  if (!originalInputSample_.getSize())
    originalInputSample_ = generateInputSample(0);
  return originalInputSample_;
}


void DesignOfExperimentEvaluation::run()
{
  isRunning_ = true;
  try
  {
    // clear result
    initialize();

    // check
    if (!getInterestVariables().getSize())
      throw InvalidDimensionException(HERE) << "You have not defined output variable to be analysed. Set interest variables.";

    const UnsignedInteger inputSampleSize = getOriginalInputSample().getSize();
    if (!inputSampleSize)
      throw InvalidArgumentException(HERE) << "The design of experiments input sample is empty";

    if (getBlockSize() > inputSampleSize)
      throw InvalidValueException(HERE) << "The block size (" << getBlockSize()
                                        << ") can not be superior to the input sample size (" << inputSampleSize << ")";

    // input sample
    Sample inputSample = Sample(0, getOriginalInputSample().getDimension());
    inputSample.setDescription(getOriginalInputSample().getDescription());

    // failed input sample
    failedInputSample_ = Sample(0, getOriginalInputSample().getDimension());
    failedInputSample_.setDescription(getOriginalInputSample().getDescription());

    // number of iterations
    const UnsignedInteger nbIter = static_cast<UnsignedInteger>(ceil(1.0 * inputSampleSize / getBlockSize()));
    // last block size
    const UnsignedInteger modulo = inputSampleSize % getBlockSize();
    const UnsignedInteger lastBlockSize = modulo == 0 ? getBlockSize() : modulo;

    // output = f(input)
    Sample outputSample(0, getInterestVariables().getSize());
    outputSample.setDescription(getInterestVariables());

    // iterations
    for (UnsignedInteger i = 0; i < nbIter; ++i)
    {
      if (stopRequested_)
        break;

      progressValue_ = (int) (i * 100 / nbIter);
      notify("progressValueChanged");

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
        blockOutputSample = getPhysicalModel().getFunction(getInterestVariables())(blockInputSample);
      }
      catch (InternalException & ex)
      {
        failedSample = blockInputSample;
      }

      // if SymbolicPhysicalModel find NaN and inf
      // for ex: in case of zero division the Symbolic models do not raise error
      if (!failedSample.getSize() && getPhysicalModel().getImplementation()->getClassName() == "SymbolicPhysicalModel")
      {
        bool nanFound = false;
        for (UnsignedInteger j = 0; j < blockInputSample.getSize(); ++j)
        {
          for (UnsignedInteger k = 0; k < getInterestVariables().getSize(); ++k)
          {
            if (!SpecFunc::IsNormal(blockOutputSample[j][k]))
            {
              failedSample = blockInputSample;
              nanFound = true;
              break;
            }
          }
          if (nanFound)
            break;
        }
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
    }

    if (!outputSample.getSize())
      throw InvalidRangeException(HERE) << "All the evaluations have failed. Check the model.";

    // set design of experiments
    designOfExperiment_.setInputSample(inputSample);
    designOfExperiment_.setOutputSample(outputSample);

    notify("analysisFinished");
  }
  catch (std::exception & ex)
  {
    errorMessage_ = ex.what();
    notify("analysisBadlyFinished");
  }
  isRunning_ = false;
}


Sample DesignOfExperimentEvaluation::getFailedInputSample() const
{
  return failedInputSample_;
}


Sample DesignOfExperimentEvaluation::getNotEvaluatedInputSample() const
{
  const UnsignedInteger originalInputSampleSize = originalInputSample_.getSize();
  const UnsignedInteger inputSampleSize = designOfExperiment_.getInputSample().getSize();
  const UnsignedInteger failedInputSampleSize = failedInputSample_.getSize();
  if ((inputSampleSize + failedInputSampleSize) < originalInputSampleSize)
    return Sample(originalInputSample_, inputSampleSize + failedInputSampleSize, originalInputSampleSize);

  return Sample();
}


/* Method save() stores the object through the StorageManager */
void DesignOfExperimentEvaluation::save(Advocate& adv) const
{
  SimulationAnalysis::save(adv);
  adv.saveAttribute("originalInputSample_", originalInputSample_);
  adv.saveAttribute("failedInputSample_", failedInputSample_);
}


/* Method load() reloads the object from the StorageManager */
void DesignOfExperimentEvaluation::load(Advocate& adv)
{
  SimulationAnalysis::load(adv);
  adv.loadAttribute("originalInputSample_", originalInputSample_);
  adv.loadAttribute("failedInputSample_", failedInputSample_);
}
}
