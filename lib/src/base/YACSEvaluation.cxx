//                                               -*- C++ -*-
/**
 *  @brief YACSEvaluation implements the evaluation of the models defined by an xml file
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include <openturns/BatchFailedException.hxx>
#include "persalys/InterpreterUnlocker.hxx"
#include "persalys/BaseTools.hxx"
#include "persalys/AppliException.hxx"
#include <thread> //std::this_thread
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
{
  if (!script.empty())
    setCode(script);
}


/* Virtual constructor */
YACSEvaluation* YACSEvaluation::clone() const
{
  return new YACSEvaluation(*this);
}


/* Comparison operator */
Bool YACSEvaluation::operator ==(const YACSEvaluation & other) const
{
  return (getCode() == other.getCode());
}


/* String converter */
String YACSEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << YACSEvaluation::GetClassName()
      << " name=" << getName()
      << " code=" << getCode();
  return oss;
}


/* String converter */
String YACSEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getInputDescription() << " code=" << getCode();
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
  ydefx::PyStudyFunction studyFunction;
  studyFunction.loadString(getCode());
  if(getCode().empty())
  {
    throw InvalidArgumentException(HERE) << "The script is empty.";
  }

  std::list<std::string> inputNames = studyFunction.inputNames();
  std::list<std::string> outputNames = studyFunction.outputNames();

  if (inputNames.size() != inS.getDimension())
    throw InvalidArgumentException(HERE) << "The dimension of the input sample "
                                         << inS.getDimension() << " is not valid";

  ydefx::Sample<double> jobSample;
  // set default value for not computed and failed points
  jobSample.outputs<double>().setDefault(std::nan(""));
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
  py2cpp::PyPtr modelToUse;
  if(nullptr == jobModel_.get() )
  {
    py2cpp::PyFunction objConstructor;
    objConstructor.loadExp("pydefx", "PyStudy");
    modelToUse = objConstructor();
  }
  else
  {
    modelToUse = jobModel_;
  }
  ydefx::Launcher l;
  std::unique_ptr<ydefx::Job> myJob;
  if (!isRunning_)
  {
    myJob.reset(l.submitPyStudyJob(modelToUse, studyFunction, jobSample, jobParams_));
    setIsRunning(true);
  }
  else
  {
    myJob.reset(l.connectJob(dump_, jobSample));
  }
  if(myJob)
  {
    double progress = myJob->progress();
    String state = myJob->state();
    while(progress < 1.0 && state != "FINISHED" && state != "ERROR" && state != "FAILED")
    {
      if (stopCallback_.first)
      {
        const Bool stop = stopCallback_.first(stopCallback_.second);
        if (stop)
        {
          setDump(myJob->dump());
          throw DetachedException(HERE) << "Job has been detached";
        }
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
      progress = myJob->progress();
      state = myJob->state();
    }
    if (progress >= 1.0 || state != "RUNNING")
      setIsRunning(false);

    if(!myJob->fetch())
    {
      setIsRunning(false);
      throw NotDefinedException(HERE) << myJob->lastError();
    }

    // get results
    UnsignedInteger sampleSize = jobSample.maxSize();
    Indices nokIdx;
    Description errorDesc;
    for(UnsignedInteger j = 0; j < sampleSize; ++j)
    {
      for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
      {
        std::string name = getOutputVariablesNames()[i];
        if(ydefx::ExecutionState::DONE == jobSample.pointState(j))
        {
          result(j, i) = jobSample.outputs<double>().get(name, j);
        }
        else// the point could not have been evaluated
        {
          nokIdx.add(j);
          const String message = OSS()
                                 << "\nThe evaluation of the point number " << j
                                 << " is in error:" << jobSample.getError(j)
                                 << "\nFor further details, see "
                                 << jobParams_.work_directory() << "/logs directory on "
                                 << jobParams_.resource_name() << ".";
          errorDesc.add(message);
          break;
        }
      }
    }
    if (nokIdx.getSize())
    {
      throw BatchFailedException(HERE, nokIdx, errorDesc,
                                 nokIdx.complement(sampleSize),
                                 result.select(nokIdx.complement(sampleSize)));
    }
  }
  else
    throw NotDefinedException(HERE) << l.lastError();
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
  setOutputDescription(outDescription);
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


const ydefx::JobParametersProxy& YACSEvaluation::jobParameters() const
{
  return jobParams_;
}


/** The job model is a python object which drives the execution of the job.*/
void YACSEvaluation::setJobModel(const py2cpp::PyPtr& model)
{
  jobModel_ = model;
}

void YACSEvaluation::setStopCallback(StopCallback callBack, void * state)
{
  stopCallback_ = std::pair<StopCallback, void *>(callBack, state);
}

/* Method save() stores the object through the StorageManager */
void YACSEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("code_", code_);
  Description listInputFiles;
  std::list<std::string> inFiles = jobParams_.in_files();
  for(const std::string& f : inFiles)
    listInputFiles.add(f);
  adv.saveAttribute("inputFiles_", listInputFiles);
  adv.saveAttribute("dump_", dump_);
  adv.saveAttribute("isRunning_", isRunning_);
}


/* Method load() reloads the object from the StorageManager */
void YACSEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("code_", code_);
  Description listInputFiles;
  adv.loadAttribute("inputFiles_", listInputFiles);
  std::list<std::string> inFiles;
  for(const std::string& f : listInputFiles)
    inFiles.push_back(f);
  jobParams_.in_files(inFiles);
  if (adv.hasAttribute("dump_"))
    adv.loadAttribute("dump_", dump_);
  if (adv.hasAttribute("isRunning_"))
    adv.loadAttribute("isRunning_", isRunning_);
}


/* Accessor to the formulas */
OT::String YACSEvaluation::getCode() const
{
  return code_;
}


void YACSEvaluation::setCode(const OT::String & code)
{
  inputValues_.clear();
  inDescription_.clear();
  outDescription_.clear();
  jobParams_ = ydefx::JobParametersProxy();
  jobParams_.configureResource("localhost");
  std::stringstream ss;
  ss << jobParams_.work_directory() << "/persalys_" << std::to_string(std::hash<std::string> {}(code));
  jobParams_.work_directory(ss.str());
  jobParams_.createTmpResultDirectory();

  ydefx::PyStudyFunction studyFunction;
  studyFunction.loadString(code);
  if(!studyFunction.isValid())
    throw InvalidArgumentException(HERE) << "Invalid YACS script code";

  std::list<std::string> inputNames = studyFunction.inputNames();
  std::list<std::string> outputNames = studyFunction.outputNames();
  code_ = code;

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
