//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#include "otgui/DesignOfExperimentImplementation.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/SpecFunc.hxx>

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(DesignOfExperimentImplementation);

static Factory<DesignOfExperimentImplementation> Factory_DesignOfExperimentImplementation;

/* Default constructor */
DesignOfExperimentImplementation::DesignOfExperimentImplementation()
  : DataSample()
  , Observable()
  , hasPhysicalModel_(true)
  , physicalModel_()
  , interestVariables_()
  , errorMessage_("")
  , stopRequested_(false)
  , progressValue_(0)
  , originalInputSample_()
  , failedInputSample_()
  , blockSize_(1)
{
}


/* Constructor with parameters */
DesignOfExperimentImplementation::DesignOfExperimentImplementation(const String& name, const PhysicalModel& physicalModel)
  : DataSample()
  , Observable()
  , hasPhysicalModel_(true)
  , physicalModel_(physicalModel)
  , interestVariables_(physicalModel.getSelectedOutputsNames())
  , errorMessage_("")
  , stopRequested_(false)
  , progressValue_(0)
  , originalInputSample_()
  , failedInputSample_()
  , blockSize_(1)
{
  setName(name);
}


/* Virtual constructor */
DesignOfExperimentImplementation* DesignOfExperimentImplementation::clone() const
{
  return new DesignOfExperimentImplementation(*this);
}


Bool DesignOfExperimentImplementation::operator==(const DesignOfExperimentImplementation& other) const
{
  if (this == &other)
    return true;
  return false;
}


Bool DesignOfExperimentImplementation::operator!=(const DesignOfExperimentImplementation& other) const
{
  return !operator==(other);
}


bool DesignOfExperimentImplementation::hasPhysicalModel() const
{
  return hasPhysicalModel_;
}


PhysicalModel DesignOfExperimentImplementation::getPhysicalModel() const
{
  return physicalModel_;
}


Description DesignOfExperimentImplementation::getVariableInputNames() const
{
  return physicalModel_.getInputNames();
}


Sample DesignOfExperimentImplementation::getFailedInputSample() const
{
  return failedInputSample_;
}


Sample DesignOfExperimentImplementation::getNotEvaluatedInputSample() const
{
  if ((getInputSample().getSize() + failedInputSample_.getSize()) < originalInputSample_.getSize())
    return Sample(originalInputSample_, getInputSample().getSize() + failedInputSample_.getSize(), originalInputSample_.getSize());

  return Sample();
}


Sample DesignOfExperimentImplementation::getOriginalInputSample() const
{
  return originalInputSample_;
}


void DesignOfExperimentImplementation::setOriginalInputSample(const Sample& sample)
{
  Sample newsample(sample);
  if (newsample.getSize() && hasPhysicalModel())
  {
    if (physicalModel_.getInputs().getSize() != sample.getDimension())
      throw InvalidArgumentException(HERE) << "The sample dimension must be equal to the number of inputs in the physical model " << physicalModel_.getInputs().getSize();

    newsample.setDescription(physicalModel_.getInputNames());
  }
  originalInputSample_ = newsample;
}


void DesignOfExperimentImplementation::setInputSample(const Sample& sample)
{
  Sample newsample(sample);
  if (newsample.getSize() && hasPhysicalModel())
  {
    if (physicalModel_.getInputs().getSize() != sample.getDimension())
      throw InvalidArgumentException(HERE) << "The sample dimension must be equal to the number of inputs in the physical model " << physicalModel_.getInputs().getSize();

    newsample.setDescription(physicalModel_.getInputNames());
  }
  DataSample::setInputSample(newsample);
}


String DesignOfExperimentImplementation::getErrorMessage() const
{
  return errorMessage_;
}


int DesignOfExperimentImplementation::getProgressValue() const
{
  return progressValue_;
}


UnsignedInteger DesignOfExperimentImplementation::getBlockSize() const
{
  return blockSize_;
}


void DesignOfExperimentImplementation::setBlockSize(const UnsignedInteger size)
{
  if (size < 1)
    throw InvalidValueException(HERE) << "The block size must be superior to 0";
  if (size > getOriginalInputSample().getSize())
    throw InvalidValueException(HERE) << "The block size can not be superior to the input sample size " << getOriginalInputSample().getSize();
  blockSize_ = size;
}


Description DesignOfExperimentImplementation::getInterestVariables() const
{
  return interestVariables_;
}


void DesignOfExperimentImplementation::setInterestVariables(const Description& variablesNames)
{
  if (!variablesNames.getSize())
    throw InvalidDimensionException(HERE) << "The number of outputs to analyse must be superior to 0";

  if (hasPhysicalModel())
  {
    const Description modelVariablesNames(getPhysicalModel().getSelectedOutputsNames());
    for (UnsignedInteger i = 0; i < variablesNames.getSize(); ++i)
      if (!modelVariablesNames.contains(variablesNames[i]))
        throw InvalidArgumentException(HERE) << "The name " << variablesNames[i] << " does not match an ouput variable name of the model";
  }

  interestVariables_ = variablesNames;
}


void DesignOfExperimentImplementation::initialize()
{
  errorMessage_ = "";
  stopRequested_ = false;
  progressValue_ = 0;
  failedInputSample_ = Sample();
  setInputSample(Sample());
  setOutputSample(Sample());
}


void DesignOfExperimentImplementation::run()
{
  try
  {
    // check
    if (!hasPhysicalModel())
      throw InvalidArgumentException(HERE) << "The design of experiment must be built from a physical model";
    if (!interestVariables_.getSize())
      throw InvalidDimensionException(HERE) << "You have not defined output variable to be analysed. Set interest variables.";

    const UnsignedInteger inputSampleSize = getOriginalInputSample().getSize();
    if (!inputSampleSize)
      throw InvalidArgumentException(HERE) << "The design of experiment input sample is empty";

    // clear result
    initialize();

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
    Sample outputSample(0, interestVariables_.getSize());
    outputSample.setDescription(interestVariables_);

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
        blockOutputSample = getPhysicalModel().getFunction(interestVariables_)(blockInputSample);
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
          for (UnsignedInteger k = 0; k < interestVariables_.getSize(); ++k)
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

    // set samples
    setInputSample(inputSample);
    setOutputSample(outputSample);

    notify("analysisFinished");
  }
  catch (std::exception & ex)
  {
    errorMessage_ = ex.what();
    notify("analysisBadlyFinished");
  }
}


String DesignOfExperimentImplementation::getPythonScript() const
{
  OSS oss;

  oss << getName() << " = otguibase.DesignOfExperimentImplementation('" << getName() << "', " << getPhysicalModel().getName() << ")\n";

  oss << "inputSample = [\n";
  for (UnsignedInteger i = 0; i < getInputSample().getSize(); ++i)
  {
    oss << "[";
    for (UnsignedInteger j = 0; j < getInputSample().getDimension(); ++j)
    {
      oss << getInputSample()[i][j];
      if (j < (getInputSample().getDimension() - 1))
        oss << ", ";
    }
    oss << "]";
    if (i < (getInputSample().getSize() - 1))
       oss << ",\n";
  }
  oss << "]\n";

  oss << getName() << ".setOriginalInputSample(inputSample)\n";
  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << "interestVariables = [";
  for (UnsignedInteger i = 0; i < getInterestVariables().getSize(); ++i)
  {
    oss << "'" << getInterestVariables()[i] << "'";
    if (i < getInterestVariables().getSize()-1)
      oss << ", ";
  }
  oss << "]\n";
  oss << getName() << ".setInterestVariables(interestVariables)\n";

  return oss;
}


void DesignOfExperimentImplementation::stop()
{
  stopRequested_ = true;
}


/* Method save() stores the object through the StorageManager */
void DesignOfExperimentImplementation::save(Advocate& adv) const
{
  DataSample::save(adv);
  adv.saveAttribute("hasPhysicalModel_", hasPhysicalModel_);
  adv.saveAttribute("physicalModel_", physicalModel_);
  adv.saveAttribute("errorMessage_", errorMessage_);
  adv.saveAttribute("originalInputSample_", originalInputSample_);
  adv.saveAttribute("failedInputSample_", failedInputSample_);
  adv.saveAttribute("blockSize_", blockSize_);
  adv.saveAttribute("interestVariables_", interestVariables_);
}


/* Method load() reloads the object from the StorageManager */
void DesignOfExperimentImplementation::load(Advocate& adv)
{
  DataSample::load(adv);
  adv.loadAttribute("hasPhysicalModel_", hasPhysicalModel_);
  adv.loadAttribute("physicalModel_", physicalModel_);
  adv.loadAttribute("errorMessage_", errorMessage_);
  adv.loadAttribute("originalInputSample_", originalInputSample_);
  adv.loadAttribute("failedInputSample_", failedInputSample_);
  adv.loadAttribute("blockSize_", blockSize_);
  adv.loadAttribute("interestVariables_", interestVariables_);
}
}
