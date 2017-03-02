//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#include "otgui/DesignOfExperimentImplementation.hxx"

#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(DesignOfExperimentImplementation);

static Factory<DesignOfExperimentImplementation> RegisteredFactory;

/* Default constructor */
DesignOfExperimentImplementation::DesignOfExperimentImplementation()
  : DataSample()
  , Observable()
  , hasPhysicalModel_(true)
  , physicalModel_()
  , errorMessage_("")
  , stopRequested_(false)
  , progressValue_(0)
{
}


/* Constructor with parameters */
DesignOfExperimentImplementation::DesignOfExperimentImplementation(const String& name, const PhysicalModel& physicalModel)
  : DataSample()
  , Observable()
  , hasPhysicalModel_(true)
  , physicalModel_(physicalModel)
  , errorMessage_("")
  , stopRequested_(false)
  , progressValue_(0)
{
  setName(name);
}


/* Virtual constructor */
DesignOfExperimentImplementation* DesignOfExperimentImplementation::clone() const
{
  return new DesignOfExperimentImplementation(*this);
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


NumericalSample DesignOfExperimentImplementation::getFailedInputSample() const
{
  return failedInputSample_;
}


NumericalSample DesignOfExperimentImplementation::getNotEvaluatedInputSample() const
{
  return notEvaluatedInputSample_;
}


void DesignOfExperimentImplementation::setInputSample(const NumericalSample& sample)
{
  NumericalSample newsample(sample);
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


void DesignOfExperimentImplementation::run()
{
  if (!hasPhysicalModel())
    throw InvalidArgumentException(HERE) << "The design of experiment must be built from a physical model";

  try
  {
    // clear result
    errorMessage_ = "";
    stopRequested_ = false;
    progressValue_ = 0;

    // input sample
    NumericalSample inputSample(getInputSample());
    NumericalSample wellDoneInputSample(0, getInputSample().getDimension());
    NumericalSample failedInputSample(0, getInputSample().getDimension());
    failedInputSample.setDescription(getInputSample().getDescription());

    // output = f(input)
    NumericalSample outputSample(0, getPhysicalModel().getSelectedOutputsNames().getSize());
    for (UnsignedInteger i=0; i<inputSample.getSize(); ++i)
    {
      if (stopRequested_)
        break;

      progressValue_ = (int) (i * 100 / inputSample.getSize());
      notify("progressValueChanged");

      NumericalPoint outputPoint(getPhysicalModel().getFunction(getPhysicalModel().getSelectedOutputsNames())(inputSample[i]));
      NumericalPoint failedPoint;

      for (UnsignedInteger j=0; j<getPhysicalModel().getSelectedOutputsNames().getSize(); ++j)
      {
        if (std::isnan(outputPoint[j]) || std::isinf(outputPoint[j]))
        {
          failedPoint = inputSample[i];
          failedInputSample.add(failedPoint);
          break;
        }
      }
      if (!failedPoint.getSize())
      {
        outputSample.add(outputPoint);
        wellDoneInputSample.add(inputSample[i]);
      }
    }

    outputSample.setDescription(getPhysicalModel().getSelectedOutputsNames());

    if (!outputSample.getSize())
      throw InvalidRangeException(HERE) << "No succeeded evaluation";

    // set samples
    if (inputSample.getSize() > wellDoneInputSample.getSize())
      setInputSample(wellDoneInputSample);

    setOutputSample(outputSample);

    failedInputSample_ = failedInputSample;

    if ((wellDoneInputSample.getSize() + failedInputSample.getSize()) < inputSample.getSize())
      notEvaluatedInputSample_ = NumericalSample(inputSample, outputSample.getSize() + failedInputSample.getSize(), inputSample.getSize());

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

  oss << getName() + " = otguibase.DesignOfExperimentImplementation('" + getName() + "', " + getPhysicalModel().getName() + ")\n";

  oss << "inputSample = [\n";
  for (UnsignedInteger i=0; i<getInputSample().getSize(); ++i)
  {
    oss << "[";
    for (UnsignedInteger j=0; j<getInputSample().getDimension(); ++j)
    {
      oss << getInputSample()[i][j];
      if (j < (getInputSample().getDimension()-1))
        oss << ", ";
    }
    oss << "]";
    if (i < (getInputSample().getSize()-1))
       oss << ",\n";
  }
  oss << "]\n";

  oss << getName() << ".setInputSample(inputSample)\n";

  return oss.str();
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
  adv.saveAttribute("failedInputSample_", failedInputSample_);
  adv.saveAttribute("notEvaluatedInputSample_", notEvaluatedInputSample_);
}


/* Method load() reloads the object from the StorageManager */
void DesignOfExperimentImplementation::load(Advocate& adv)
{
  DataSample::load(adv);
  adv.loadAttribute("hasPhysicalModel_", hasPhysicalModel_);
  adv.loadAttribute("physicalModel_", physicalModel_);
  adv.loadAttribute("errorMessage_", errorMessage_);
  adv.loadAttribute("failedInputSample_", failedInputSample_);
  adv.loadAttribute("notEvaluatedInputSample_", notEvaluatedInputSample_);
}
}