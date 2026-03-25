//                                               -*- C++ -*-
/**
 *  @brief Coupling model through YACS
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
#include "persalys/YACSCouplingPhysicalModel.hxx"
#include "persalys/BaseTools.hxx"
#include "persalys/YACSPhysicalModel.hxx"
#include "persalys/FileMemoizeFunction.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <filesystem>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(YACSCouplingPhysicalModel)

static Factory<YACSCouplingPhysicalModel> Factory_YACSCouplingPhysicalModel;

/* Default constructor */
YACSCouplingPhysicalModel::YACSCouplingPhysicalModel(const String & name,
    const CouplingStepCollection & steps)
  : CouplingPhysicalModel(name, steps)
{
  // with YACS resources are copied to the salome workdir and cannot be resolved in the local FS
  // use this flag to fetch them relatively to the salome work dir
  resourcesCopiedToWorkdir_ = true;
  updateCode();
}

/* Default constructor */
YACSCouplingPhysicalModel::YACSCouplingPhysicalModel(const String & name,
    const InputCollection & inputs,
    const OutputCollection & outputs,
    const CouplingStepCollection & steps)
  : CouplingPhysicalModel(name, inputs, outputs, steps)
{
  resourcesCopiedToWorkdir_ = true;
  updateCode();
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


void YACSCouplingPhysicalModel::setParallel(const Bool flag)
{
  // just to trigger updateCode via PythonPhysicalModelPropertiesDialog for jobParameters
  CouplingPhysicalModel::setParallel(flag);
  updateCode();
}


void YACSCouplingPhysicalModel::setCode(const String & script)
{
  CouplingPhysicalModel::setCode(script);
  try
  {
    evaluation_.setCode(script);
  }
  catch (const std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << "Error in the script:\n" << ex.what();
  }
  updateCode();
}


void YACSCouplingPhysicalModel::updateCode()
{
  // coupling runs in a subdir of the salome work dir so that resource files can be found relatively to workdir
  const String workSubDir = "/persalys_" + std::to_string(std::hash<std::string> {}(getPythonScript())); // identifies the model
  evaluation_.setWorkSubDir(workSubDir);
  workDir_ = jobParameters().work_directory() + workSubDir; // must be set before updateCode

  CouplingPhysicalModel::updateCode();
  evaluation_.setCode(getCode());

  // add data files to job parameters
  Description extraInputFiles;
  for (const CouplingStep & step : getSteps())
  {
    for (const CouplingInputFile & inputFile : step.getInputFiles())
    {
      if (!inputFile.getPath().empty())
        // yacs wants absolute paths
        extraInputFiles.add(std::filesystem::absolute(inputFile.getPath()).string());
    }
    for (const CouplingResourceFile & resourceFile : step.getResourceFiles())
    {
      if (!resourceFile.getPath().empty())
        // yacs wants absolute paths
        extraInputFiles.add(std::filesystem::absolute(resourceFile.getPath()).string());
    }
  }
  evaluation_.setExtraInputFiles(extraInputFiles);
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
  oss << evaluation_.getJobParamsPythonScript(getName());
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


bool YACSCouplingPhysicalModel::canBeDetached() const
{
  return true;
}


}
