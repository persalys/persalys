//                                               -*- C++ -*-
/**
 *  @brief YACSPhysicalModel implements models defined by an xml file
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
#include "persalys/YACSPhysicalModel.hxx"
#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/MemoizeFunction.hxx>

#include <regex>

using namespace OT;


namespace PERSALYS
{

CLASSNAMEINIT(YACSPhysicalModel)

static Factory<YACSPhysicalModel> Factory_YACSPhysicalModel;

/* Default constructor */
YACSPhysicalModel::YACSPhysicalModel(const String & name)
  : PythonPhysicalModel(name)
{
  // Nothing to do
}


/* Constructor with parameters */
YACSPhysicalModel::YACSPhysicalModel(const String & name,
                                     const InputCollection & inputs,
                                     const OutputCollection & outputs,
                                     const String & code)
  : PythonPhysicalModel(name, inputs, outputs, "")
{
  setCode(code);
}


/* Virtual constructor */
YACSPhysicalModel* YACSPhysicalModel::clone() const
{
  return new YACSPhysicalModel(*this);
}


void YACSPhysicalModel::setInputs(const InputCollection & inputs)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a YACS model";
}


void YACSPhysicalModel::addInput(const Input & input)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a YACS model";
}


void YACSPhysicalModel::removeInput(const String & inputName)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a YACS model";
}


void YACSPhysicalModel::setOutputs(const OutputCollection & outputs)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a YACS model";
}


void YACSPhysicalModel::addOutput(const Output & output)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a YACS model";
}


void YACSPhysicalModel::removeOutput(const String & outputName)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a YACS model";
}


ydefx::JobParametersProxy& YACSPhysicalModel::jobParameters()
{
  return evaluation_.jobParameters();
}


const ydefx::JobParametersProxy& YACSPhysicalModel::jobParameters()const
{
  return evaluation_.jobParameters();
}


void YACSPhysicalModel::setCode(const String & code)
{
  PythonPhysicalModel::setCode(code);
  if (useYACS_)
  {
    try
    {
      evaluation_.setCode(code);
    }
    catch (std::exception & ex)
    {
      throw InvalidArgumentException(HERE) << "Error in the script.\n" << ex.what();
    }
  }
}


Function YACSPhysicalModel::generateFunction(const Description & outputNames) const
{
  if (!useYACS_)
    return PythonPhysicalModel::generateFunction(outputNames);
  if (!functionCache_.getEvaluation().getImplementation()->isActualImplementation())
  {
    YACSEvaluation anEvaluation(evaluation_);
    anEvaluation.setOutputVariablesNames(outputNames);
    functionCache_ = MemoizeFunction(anEvaluation);
  }
  return functionCache_;
}


String YACSPhysicalModel::getPythonScript() const
{
  OSS oss;

  for (UnsignedInteger i = 0; i < getInputDimension(); ++ i)
    oss << getInputs()[i].getPythonScript();

  for (UnsignedInteger i = 0; i < getOutputDimension(); ++ i)
    oss << getOutputs()[i].getPythonScript();

  oss << "inputs = " << Parameters::GetOTDescriptionStr(getInputNames(), false) << "\n";
  oss << "outputs = " << Parameters::GetOTDescriptionStr(getOutputNames(), false) << "\n";

  // replace ''' by """
  std::string code = getCode();
  std::regex_replace(code, std::regex("'''"), "\"\"\"");

  oss << "code = '''" << code << "'''\n";
  oss << getName()
      << " = persalys.YACSPhysicalModel('" << getName() << "'"
      << ", inputs, outputs, code)\n";
  oss << getName() << ".setUseYACS(" << (useYACS_ ? "True" : "False") << ")\n";
  if (useYACS_)
    oss << getJobParamsPythonScript();

  oss << getProbaModelPythonScript();
  oss << PhysicalModelImplementation::getCopulaPythonScript();

  return oss;
}


OT::String YACSPhysicalModel::getJobParamsPythonScript() const
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


/* Use YACS backend or Python backend */
void YACSPhysicalModel::setUseYACS(const Bool useYACS)
{
  if (useYACS != useYACS_)
  {
    useYACS_ = useYACS;
    functionCache_ = Function();

    if (useYACS)
    {
      try
      {
        evaluation_.setCode(getCode());
      }
      catch (std::exception & ex)
      {
        // just to keep in sync
      }
    }
  }
}


Bool YACSPhysicalModel::getUseYACS() const
{
  return useYACS_;
}


/** String converter */
String YACSPhysicalModel::__repr__() const
{
  OSS oss;
  oss << PythonPhysicalModel::__repr__()
      << " evaluation=" << evaluation_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void YACSPhysicalModel::save(Advocate & adv) const
{
  PythonPhysicalModel::save(adv);
  adv.saveAttribute("evaluation_", evaluation_);
  adv.saveAttribute("useYACS_", useYACS_);
}


/* Method load() reloads the object from the StorageManager */
void YACSPhysicalModel::load(Advocate & adv)
{
  PythonPhysicalModel::load(adv);
  adv.loadAttribute("evaluation_", evaluation_);
  if (adv.hasAttribute("useYACS_"))
    adv.loadAttribute("useYACS_", useYACS_);
  else
    useYACS_ = true;
}


bool YACSPhysicalModel::canBeDetached() const
{
  return useYACS_;
}


}
