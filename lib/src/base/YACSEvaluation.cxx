//                                               -*- C++ -*-
/**
 *  @brief YACSEvaluation implements the evaluation of the models defined by an xml file
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/YACSEvaluation.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include "persalys/InterpreterUnlocker.hxx"
#include <memory> //std::unique_ptr
#include <cmath> //std::nan

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(YACSEvaluation)

static Factory<YACSEvaluation> Factory_YACSEvaluation;

/* Default constructor */
YACSEvaluation::YACSEvaluation(const String & script)
  : EvaluationImplementation()
  , inputValues_()
  , inDescription_()
  , outDescription_()
  , jobParams_()
  , studyFunction_()
{
  if (!script.empty())
    setContent(script);
  jobParams_.configureResource("localhost");
  jobParams_.createTmpResultDirectory();
}


/* Virtual constructor */
YACSEvaluation* YACSEvaluation::clone() const
{
  return new YACSEvaluation(*this);
}


/* Comparison operator */
Bool YACSEvaluation::operator ==(const YACSEvaluation & other) const
{
  return (studyFunction_.content() == other.studyFunction_.content());
}


/* String converter */
String YACSEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << YACSEvaluation::GetClassName()
      << " name=" << getName()
      << " code=" << studyFunction_.content();
  return oss;
}


/* String converter */
String YACSEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getInputDescription() << " code=" << studyFunction_.content();
  return oss;
}


/* Operator () */
Point YACSEvaluation::operator() (const Point & inP) const
{
  return operator()(Sample(1, inP))[0];
}


/* Operator () */
Sample YACSEvaluation::operator() (const Sample & inS) const
{
  InterpreterUnlocker iul;
  if(!studyFunction_.isValid())
  {
    Log::Error(OSS() << "Invalid study function: " << studyFunction_.errors() 
                     << "\n");
    throw InvalidArgumentException(HERE) << "The study function is incorrect: "
                                         << studyFunction_.errors() ;
  }

  std::list<std::string> inputNames = studyFunction_.inputNames();
  std::list<std::string> outputNames = studyFunction_.outputNames();
  if (inputNames.size() != inS.getDimension())
  {
    Log::Error(OSS() << 
    "In YACSEvaluation::operator(): inputNames.size() != inS.getDimension()\n");
    throw InvalidArgumentException(HERE) << "The dimension of the input sample "
                                       << inS.getDimension() << " is not valid";
  }
  ydefx::Sample<double> jobSample;
  // set default value for not computed and failed points
  jobSample.outputs<double>().setDefault(std::nan("1"));
  UnsignedInteger nameIdx = 0;
  // ydefx identifies variables by their name, but ot does by index.
  for(const std::string& name : inputNames)
  {
    jobSample.inputs<double>().addName(name);
    for (UnsignedInteger valueIdx = 0; valueIdx < inS.getSize(); ++valueIdx)
      jobSample.inputs<double>().set(name, valueIdx, inS(valueIdx, nameIdx));
    ++ nameIdx;
  }

  // specify that all the outputs are of type double
  for(const std::string& name : outputNames)
    jobSample.outputs<double>().addName(name);

  Sample result(inS.getSize(), getOutputDimension());
  result.setDescription(getOutputVariablesNames());

  ydefx::Launcher l;
  std::unique_ptr<ydefx::Job> myJob;
  try
  {
    myJob.reset(l.submitMonoPyJob(studyFunction_, jobSample, jobParams_));
    if(myJob)
    {
      myJob->wait();
      if(!myJob->fetch())
        throw NotDefinedException(HERE) << myJob->lastError();

      // get results
      UnsignedInteger sampleSize = jobSample.maxSize();
      for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
      {
        std::string name = getOutputVariablesNames()[i];
        for(UnsignedInteger j = 0; j < sampleSize; ++j)
          if(ydefx::ExecutionState::DONE == jobSample.pointState(j))
            result(j, i) = jobSample.outputs<double>().get(name, j);
          else // the point could not have been evaluated
            throw InternalException(HERE)
                  << "\nThe evaluation of the point number " << j
                  << " is in error:" << jobSample.getError(j)
                  << "\nFor further details, see "
                  << jobParams_.work_directory() << "/logs directory on "
                  << jobParams_.resource_name() << ".";
      }
    }
    else
      throw NotDefinedException(HERE) << l.lastError();
  }
  catch (InternalException& e)
  {
    throw;
  }
  catch (std::exception& e)
  {
    throw NotDefinedException(HERE) << e.what();
  }
  return result;
}


/* Accessor for input values */
Point YACSEvaluation::getInputValues() const
{
  return inputValues_;
}


/* Accessor for input values */
void YACSEvaluation::setInputValues(const Point & inP)
{
  inputValues_ = inP;
}


Description YACSEvaluation::getInputVariablesNames() const
{
  return inDescription_;
}


/* Accessor for input point dimension */
UnsignedInteger YACSEvaluation::getInputDimension() const
{
  return getInputVariablesNames().getSize();
}


Description YACSEvaluation::getOutputVariablesNames() const
{
  return outDescription_;
}


void YACSEvaluation::setOutputVariablesNames(const Description & outDescription)
{
  outDescription_ = outDescription;
}


/* Accessor for output point dimension */
UnsignedInteger YACSEvaluation::getOutputDimension() const
{
  return getOutputVariablesNames().getSize();
}


ydefx::JobParametersProxy& YACSEvaluation::jobParameters()
{
  return jobParams_;
}


const ydefx::JobParametersProxy& YACSEvaluation::jobParameters()const
{
  return jobParams_;
}


/* Method save() stores the object through the StorageManager */
void YACSEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("code_", studyFunction_.content());
  Description listInputFiles;
  std::list<std::string> inFiles = jobParams_.in_files();
  for(const std::string& f : inFiles)
    listInputFiles.add(f);
  adv.saveAttribute("inputFiles_", listInputFiles);
}


/* Method load() reloads the object from the StorageManager */
void YACSEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  std::string value;
  adv.loadAttribute("code_", value);
  setContent(value);
  Description listInputFiles;
  adv.loadAttribute("inputFiles_", listInputFiles);
  std::list<std::string> inFiles;
  for(const std::string& f : listInputFiles)
    inFiles.push_back(f);
  jobParams_.in_files(inFiles);
}


/* Accessor to the formulas */
OT::String YACSEvaluation::getContent() const
{
  return studyFunction_.content();
}


void YACSEvaluation::setContent(const OT::String & pyScript)
{
  inputValues_.clear();
  inDescription_.clear();
  outDescription_.clear();

  studyFunction_.loadString(pyScript);
  if(!studyFunction_.isValid())
    return;

  std::list<std::string> inputNames = studyFunction_.inputNames();
  std::list<std::string> outputNames = studyFunction_.outputNames();

  for(const std::string& name : inputNames)
  {
    inputValues_.add(0.0);
    inDescription_.add(name);
  }

  for(const std::string& name : outputNames)
    outDescription_.add(name);

  setInputDescription(inDescription_);
  setOutputDescription(outDescription_);
}

}
