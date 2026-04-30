//                                               -*- C++ -*-
/**
 *  @brief YACSEvaluation implements the evaluation of the models defined by an xml file
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
  jobParams_ = ydefx::JobParametersProxy();
  jobParams_.configureResource("localhost");
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
  if (getCode().empty())
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
  if (nullptr == jobModel_.get() )
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
    // actually run in subdir
    ydefx::JobParametersProxy jobParams2(jobParams_);
    const String workSubDir = workSubDir_.empty() ? ("/persalys_" + std::to_string(std::hash<std::string> {}(getCode()))) : workSubDir_;
    jobParams2.work_directory(jobParams_.work_directory() + workSubDir);

    // add resource files
    std::list<std::string> inFiles;
    for (const String & extraInputFile : extraInputFiles_)
      inFiles.push_back(extraInputFile);
    jobParams2.add_in_files(inFiles);

    myJob.reset(l.submitPyStudyJob(modelToUse, studyFunction, jobSample, jobParams2));
    setIsRunning(true);
  }
  else
  {
    myJob.reset(l.connectJob(dump_, jobSample));
  }
  if (myJob)
  {
    double progress = myJob->progress();
    String state = myJob->state();
    while (progress < 1.0 && Description({"QUEUED", "IN_PROCESS", "RUNNING"}).contains(state))
    {
      // detach
      if (detachCallback_.first)
      {
        const Bool detach = detachCallback_.first(detachCallback_.second);
        if (detach)
        {
          setDump(myJob->dump());
          throw DetachedException(HERE) << "Job has been detached";
        }
      }

      // stop
      if (stopCallback_.first)
      {
        const Bool stop = stopCallback_.first(stopCallback_.second);
        if (stop)
        {
          myJob->cancel();
          // throw regardless of the return value of cancel to force error on next blocks
          throw InternalException(HERE) << "Job cancelled";
        }
      }

      std::this_thread::sleep_for(std::chrono::seconds(1));
      progress = myJob->progress();
      state = myJob->state();
    }
    if (progress >= 1.0 || state != "RUNNING")
      setIsRunning(false);

    if (!myJob->fetch())
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
        if (ydefx::ExecutionState::DONE == jobSample.pointState(j))
        {
          result(j, i) = jobSample.outputs<double>().get(name, j);
        }
        else// the point could not have been evaluated
        {
          nokIdx.add(j);
          const String message = OSS()
                                 << "The evaluation of the point number " << j << " is in error: " << jobSample.getError(j)
                                 << "\nFor further details, see " << jobParams_.work_directory()
                                 << "/logs directory on " << jobParams_.resource_name() << ".\n";
          errorDesc.add(message);
          break;
        }
      }
    }
    if (nokIdx.getSize())
    {
      throw BatchFailedException(HERE, nokIdx, errorDesc,
                                 nokIdx.complement(sampleSize), result.select(nokIdx.complement(sampleSize)))
        << (nokIdx.getSize() == 1 ? errorDesc[0] : "operator(Sample) partial fail");
    }
  }
  else
    throw NotDefinedException(HERE) << l.lastError();
  return result;
}

void YACSEvaluation::setExtraInputFiles(const Description & extraInputFiles)
{
  extraInputFiles_ = extraInputFiles;
}

void YACSEvaluation::setWorkSubDir(const OT::String & workSubDir)
{
  workSubDir_ = workSubDir;
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

void YACSEvaluation::setDump(const OT::String & dump) const
{
  dump_ = dump;
}

OT::String YACSEvaluation::getDump() const
{
  return dump_;
}

void YACSEvaluation::setIsRunning(const Bool isRunning) const
{
  isRunning_ = isRunning;
}

Bool YACSEvaluation::getIsRunning() const
{
  return isRunning_;
}


/* Method save() stores the object through the StorageManager */
void YACSEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("code_", code_);
  adv.saveAttribute("dump_", dump_);
  adv.saveAttribute("isRunning_", isRunning_);
  adv.saveAttribute("workSubDir_", workSubDir_);
  adv.saveAttribute("extraInputFiles_", extraInputFiles_);

  // jobParams_
  Description listInputFiles;
  std::list<std::string> inFiles = jobParams_.in_files();
  for(const std::string& f : inFiles)
    listInputFiles.add(f);
  adv.saveAttribute("inputFiles_", listInputFiles);
  adv.saveAttribute("job_name", jobParams_.job_name());
  adv.saveAttribute("job_type", jobParams_.job_type());
  adv.saveAttribute("job_file", jobParams_.job_file());
  adv.saveAttribute("pre_command", jobParams_.pre_command());
  adv.saveAttribute("env_file", jobParams_.env_file());
  adv.saveAttribute("work_directory", jobParams_.work_directory());
  adv.saveAttribute("local_directory", jobParams_.local_directory());
  adv.saveAttribute("result_directory", jobParams_.result_directory());
  adv.saveAttribute("maximum_duration", jobParams_.maximum_duration());
  adv.saveAttribute("resource_name", jobParams_.resource_name());
  const UnsignedInteger nb_proc = jobParams_.nb_proc();
  adv.saveAttribute("nb_proc", nb_proc);
  const UnsignedInteger mem_mb = jobParams_.mem_mb();
  adv.saveAttribute("mem_mb", mem_mb);
  const UnsignedInteger nb_node = jobParams_.nb_node();
  adv.saveAttribute("nb_node", nb_node);
  const UnsignedInteger nb_proc_per_node = jobParams_.nb_proc_per_node();
  adv.saveAttribute("nb_proc_per_node", nb_proc_per_node);
  adv.saveAttribute("queue", jobParams_.queue());
  adv.saveAttribute("partition", jobParams_.partition());
  adv.saveAttribute("exclusive", jobParams_.exclusive());
  adv.saveAttribute("mem_per_cpu", (UnsignedInteger)jobParams_.mem_per_cpu());
  adv.saveAttribute("wckey", jobParams_.wckey());
  adv.saveAttribute("extra_params", jobParams_.extra_params());
  const UnsignedInteger nb_branches = jobParams_.nb_branches();
  adv.saveAttribute("nb_branches", nb_branches);
}


/* Method load() reloads the object from the StorageManager */
void YACSEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("code_", code_);
  if (adv.hasAttribute("dump_"))
    adv.loadAttribute("dump_", dump_);
  if (adv.hasAttribute("isRunning_"))
    adv.loadAttribute("isRunning_", isRunning_);
  if (adv.hasAttribute("workSubDir_"))
    adv.loadAttribute("workSubDir_", workSubDir_);
  if (adv.hasAttribute("extraInputFiles_"))
    adv.loadAttribute("extraInputFiles_", extraInputFiles_);

  // jobParams_
  Description listInputFiles;
  adv.loadAttribute("inputFiles_", listInputFiles);
  std::list<std::string> inFiles;
  for(const std::string& f : listInputFiles)
    inFiles.push_back(f);
  jobParams_.in_files(inFiles);
  if (adv.hasAttribute("job_name"))
  {
    String job_name;
    String job_type;
    String job_file;
    String pre_command;
    String env_file;
    String work_directory;
    String local_directory;
    String result_directory;
    String maximum_duration;
    String resource_name;
    UnsignedInteger nb_proc = 0;
    UnsignedInteger mem_mb = 0;
    UnsignedInteger nb_node = 0;
    UnsignedInteger nb_proc_per_node = 0;
    String queue;
    String partition;
    Bool exclusive = false;
    UnsignedInteger mem_per_cpu = 0;
    String wckey;
    String extra_params;
    UnsignedInteger nb_branches = 0;
    adv.loadAttribute("job_name", job_name);
    adv.loadAttribute("job_type", job_type);
    adv.loadAttribute("job_file", job_file);
    adv.loadAttribute("pre_command", pre_command);
    adv.loadAttribute("env_file", env_file);
    adv.loadAttribute("work_directory", work_directory);
    adv.loadAttribute("local_directory", local_directory);
    adv.loadAttribute("result_directory", result_directory);
    adv.loadAttribute("maximum_duration", maximum_duration);
    adv.loadAttribute("resource_name", resource_name);
    adv.loadAttribute("nb_proc", nb_proc);
    adv.loadAttribute("mem_mb", mem_mb);
    adv.loadAttribute("nb_node", nb_node);
    adv.loadAttribute("nb_proc_per_node", nb_proc_per_node);
    adv.loadAttribute("queue", queue);
    adv.loadAttribute("partition", partition);
    adv.loadAttribute("exclusive", exclusive);
    adv.loadAttribute("mem_per_cpu", mem_per_cpu);
    adv.loadAttribute("wckey", wckey);
    adv.loadAttribute("extra_params", extra_params);
    adv.loadAttribute("nb_branches", nb_branches);
    jobParams_.job_name(job_name);
    jobParams_.job_type(job_type);
    jobParams_.job_file(job_file);
    jobParams_.pre_command(pre_command);
    jobParams_.env_file(env_file);
    jobParams_.work_directory(work_directory);
    jobParams_.local_directory(local_directory);
    jobParams_.result_directory(result_directory);
    jobParams_.maximum_duration(maximum_duration);
    jobParams_.resource_name(resource_name);
    jobParams_.nb_proc(nb_proc);
    jobParams_.mem_mb(mem_mb);
    jobParams_.nb_node(nb_node);
    jobParams_.nb_proc_per_node(nb_proc_per_node);
    jobParams_.queue(queue);
    jobParams_.partition(partition);
    jobParams_.exclusive(exclusive);
    jobParams_.mem_per_cpu(mem_per_cpu);
    jobParams_.wckey(wckey);
    jobParams_.extra_params(extra_params);
    jobParams_.nb_branches(nb_branches);
  }
  setCode(code_);
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

  jobParams_.createTmpResultDirectory();

  ydefx::PyStudyFunction studyFunction;
  studyFunction.loadString(code);
  if (!studyFunction.isValid())
    throw InvalidArgumentException(HERE) << studyFunction.errors();

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


OT::String YACSEvaluation::getJobParamsPythonScript(const String & name) const
{
  OSS oss;
  if (jobParameters().nb_branches() > 0)
    oss << name << ".jobParameters().nb_branches = " << jobParameters().nb_branches() << "\n";

  const std::string salomeParametersPrefix = name + ".jobParameters().salome_parameters";
  if (!jobParameters().in_files().empty())
  {
    oss << salomeParametersPrefix << ".in_files = [\n";
    for (const std::string & fileName : jobParameters().in_files())
      oss << "    r'" << fileName << "',\n";
    oss << "]\n";
  }

  if (!jobParameters().job_name().empty())
    oss << salomeParametersPrefix << ".job_name = r'" << jobParameters().job_name() << "'\n";

  if (!jobParameters().pre_command().empty())
    oss << salomeParametersPrefix << ".pre_command = r'" << jobParameters().pre_command() << "'\n";

  if (!jobParameters().env_file().empty())
    oss << salomeParametersPrefix << ".env_file = r'" << jobParameters().env_file() << "'\n";

  if (!jobParameters().work_directory().empty())
    oss << salomeParametersPrefix << ".work_directory = r'" << jobParameters().work_directory() << "'\n";

  if (!jobParameters().result_directory().empty())
    oss << salomeParametersPrefix << ".result_directory = r'" << jobParameters().result_directory() << "'\n";

  if (!jobParameters().maximum_duration().empty())
    oss << salomeParametersPrefix << ".maximum_duration = '" << jobParameters().maximum_duration() << "'\n";

  if (!jobParameters().resource_name().empty())
    oss << salomeParametersPrefix << ".resource_required.name = r'" << jobParameters().resource_name() << "'\n";

  if (jobParameters().nb_proc() > 0)
    oss << salomeParametersPrefix << ".resource_required.nb_proc = " << jobParameters().nb_proc() << "\n";

  if (jobParameters().mem_mb() > 0)
    oss << salomeParametersPrefix << ".resource_required.mem_mb = " << jobParameters().mem_mb() << "\n";

  if (jobParameters().nb_node() > 0)
    oss << salomeParametersPrefix << ".resource_required.nb_node = " << jobParameters().nb_node() << "\n";

  if (jobParameters().nb_proc_per_node() > 0)
    oss << salomeParametersPrefix << ".resource_required.nb_proc_per_node = " << jobParameters().nb_proc_per_node() << "\n";

  if (!jobParameters().queue().empty())
    oss << salomeParametersPrefix << ".queue = r'" << jobParameters().queue() << "'\n";

  if (!jobParameters().partition().empty())
    oss << salomeParametersPrefix << ".partition = r'" << jobParameters().partition() << "'\n";

  if (jobParameters().exclusive())
    oss << salomeParametersPrefix << ".exclusive = True\n";

  if (jobParameters().mem_per_cpu() > 0)
    oss << salomeParametersPrefix << ".mem_per_cpu = " << jobParameters().mem_per_cpu() << "\n";

  if (!jobParameters().wckey().empty())
    oss << salomeParametersPrefix << ".wckey = r'" << jobParameters().wckey() << "'\n";

  if (!jobParameters().extra_params().empty())
    oss << salomeParametersPrefix << ".extra_params = r'" << jobParameters().extra_params() << "'\n";
  return oss;
}

}
