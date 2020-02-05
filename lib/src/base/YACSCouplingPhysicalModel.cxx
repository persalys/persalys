//                                               -*- C++ -*-
/**
 *  @brief Coupling model through YACS
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#include "persalys/YACSCouplingPhysicalModel.hxx"
#include "persalys/BaseTools.hxx"
#include "persalys/YACSPhysicalModel.hxx"
#include "persalys/FileMemoizeFunction.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <boost/filesystem.hpp>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(YACSCouplingPhysicalModel)

static Factory<YACSCouplingPhysicalModel> Factory_YACSCouplingPhysicalModel;

/* Default constructor */
YACSCouplingPhysicalModel::YACSCouplingPhysicalModel(const String & name,
                                                     const CouplingStepCollection & steps)
  : CouplingPhysicalModel(name, steps)
  , evaluation_()
{
  setCode(getCode());
}

/* Default constructor */
YACSCouplingPhysicalModel::YACSCouplingPhysicalModel(const String & name,
                                                     const InputCollection & inputs,
                                                     const OutputCollection & outputs,
                                                     const CouplingStepCollection & steps)
  : CouplingPhysicalModel(name, inputs, outputs, steps)
  , evaluation_()
{
  setCode(getCode());
}

/* Virtual constructor */
YACSCouplingPhysicalModel* YACSCouplingPhysicalModel::clone() const
{
  return new YACSCouplingPhysicalModel(*this);
}

ydefx::JobParametersProxy& YACSCouplingPhysicalModel::jobParameters()
{
  return evaluation_.jobParameters();
}


const ydefx::JobParametersProxy& YACSCouplingPhysicalModel::jobParameters() const
{
  return evaluation_.jobParameters();
}


void YACSCouplingPhysicalModel::setCode(const String & script)
{
  CouplingPhysicalModel::setCode(script);
  try
  {
    evaluation_.setContent(script);
  }
  catch (std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << "Error in the script:\n" << ex.what();
  }

  // add data files to job parameters
  const Collection<CouplingStep> steps(getSteps());
  std::list<std::string> inFiles;
  for (UnsignedInteger i = 0; i < steps.getSize(); ++ i)
  {
    const CouplingStep step(steps[i]);
    const CouplingInputFileCollection inputFiles(step.getInputFiles());
    for (UnsignedInteger j = 0; j < inputFiles.getSize(); ++ j)
    {
      const CouplingInputFile inputFile(inputFiles[j]);
      // yacs wants absolute paths
      inFiles.push_back(boost::filesystem::absolute(inputFile.getPath()).string());
    }
    const CouplingResourceFileCollection resourceFiles(step.getResourceFiles());
    for (UnsignedInteger j = 0; j < resourceFiles.getSize(); ++ j)
    {
      const CouplingResourceFile resourceFile(resourceFiles[j]);
      // yacs wants absolute paths
      inFiles.push_back(boost::filesystem::absolute(resourceFile.getPath()).string());
    }
  }
  jobParameters().in_files(inFiles);
}

Function YACSCouplingPhysicalModel::getFunction() const
{
  return generateFunction(getOutputNames());
}

Function YACSCouplingPhysicalModel::generateFunction(const Description & outputNames) const
{
  YACSEvaluation evaluation(evaluation_);
  evaluation.setOutputVariablesNames(outputNames);
  return FileMemoizeFunction(evaluation, getCacheInputFile(), getCacheOutputFile());
}


String YACSCouplingPhysicalModel::getPythonScript() const
{
  OSS oss;
  oss << CouplingPhysicalModel::getPythonScript();
  oss << getJobParamsPythonScript();
  return oss;
}


OT::String YACSCouplingPhysicalModel::getJobParamsPythonScript() const
{
  OSS oss;
  std::list<std::string> inFiles = jobParameters().in_files();
  if(!inFiles.empty())
  {
    String filesListName = getName() + "_infiles";
    oss << filesListName << " = [";
    for(const std::string& fileName : inFiles)
      oss << "'" << fileName << "',\n";
    oss << "]\n";
    oss << getName() << ".jobParameters().salome_parameters.in_files = "
        << filesListName << "\n";
  }
  std::string root_string = getName() + ".jobParameters().salome_parameters.";
  std::string value;
  value = jobParameters().job_name();
  if(!value.empty())
    oss << root_string << "job_name = '" << value << "'\n";

  value = jobParameters().pre_command();
  if(!value.empty())
    oss << root_string << "pre_command = '" << value << "'\n";

  value = jobParameters().env_file();
  if(!value.empty())
    oss << root_string << "env_file = '" << value << "'\n";

  value = jobParameters().work_directory();
  if(!value.empty())
    oss << root_string << "work_directory = '" << value << "'\n";

  value = jobParameters().result_directory();
  if(!value.empty())
    oss << root_string << "result_directory = '" << value << "'\n";

  value = jobParameters().maximum_duration();
  if(!value.empty())
    oss << root_string << "maximum_duration = '" << value << "'\n";

  value = jobParameters().resource_name();
  if(!value.empty())
    oss << root_string << "resource_required.name = '" << value << "'\n";

  if(jobParameters().nb_proc() > 0)
    oss << root_string << "resource_required.nb_proc = "
        << jobParameters().nb_proc() << "\n";

  if(jobParameters().mem_mb() > 0)
    oss << root_string << "resource_required.mem_mb = "
        << jobParameters().mem_mb() << "\n";

  if(jobParameters().nb_node() > 0)
    oss << root_string << "resource_required.nb_node = "
        << jobParameters().nb_node() << "\n";

  if(jobParameters().nb_proc_per_node() > 0)
    oss << root_string << "resource_required.nb_proc_per_node = "
        << jobParameters().nb_proc_per_node() << "\n";

  value = jobParameters().queue();
  if(!value.empty())
    oss << root_string << "queue = '" << value << "'\n";

  value = jobParameters().partition();
  if(!value.empty())
    oss << root_string << "partition = '" << value << "'\n";

  if(jobParameters().exclusive())
    oss << root_string << "exclusive = True\n";

  if(jobParameters().mem_per_cpu() > 0)
    oss << root_string << "mem_per_cpu = "
        << jobParameters().mem_per_cpu() << "\n";

  value = jobParameters().wckey();
  if(!value.empty())
    oss << root_string << "wckey = '" << value << "'\n";

  value = jobParameters().extra_params();
  if(!value.empty())
    oss << root_string << "extra_params = '''" << value << "'''\n";

  if(jobParameters().nb_branches() > 0)
    oss << getName() << ".jobParameters().nb_branches = "
        << jobParameters().nb_branches() << "\n";
  return oss;
}


/** String converter */
String YACSCouplingPhysicalModel::__repr__() const
{
  OSS oss;
  oss << CouplingPhysicalModel::__repr__()
      << " evaluation=" << evaluation_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void YACSCouplingPhysicalModel::save(Advocate & adv) const
{
  CouplingPhysicalModel::save(adv);
  adv.saveAttribute("evaluation_", evaluation_);
}


/* Method load() reloads the object from the StorageManager */
void YACSCouplingPhysicalModel::load(Advocate & adv)
{
  CouplingPhysicalModel::load(adv);
  adv.loadAttribute("evaluation_", evaluation_);
}


void YACSCouplingPhysicalModel::acceptLaunchParameters(LaunchParametersVisitor* visitor)
{
  visitor->visitYACS(this);
}
}
