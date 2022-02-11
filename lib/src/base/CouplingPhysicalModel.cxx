//                                               -*- C++ -*-
/**
 *  @brief Intermediate coupling step
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/CouplingPhysicalModel.hxx"
#include "persalys/PythonScriptEvaluation.hxx"
#include "persalys/BaseTools.hxx"
#include "persalys/FileMemoizeFunction.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(CouplingPhysicalModel)

static Factory<CouplingPhysicalModel> Factory_CouplingPhysicalModel;
static Factory<PersistentCollection<CouplingStep> > Factory_PersistentCollectionCouplingStep;

/* Default constructor */
CouplingPhysicalModel::CouplingPhysicalModel(const OT::String & name,
                                             const CouplingStepCollection & steps)
  : PythonPhysicalModel(name)
  , cleanupWorkDirectory_(true)
{
  setSteps(steps);
}

/* Default constructor */
CouplingPhysicalModel::CouplingPhysicalModel(const OT::String & name,
                                             const InputCollection & inputs,
                                             const OutputCollection & outputs,
                                             const CouplingStepCollection & steps)
  : PythonPhysicalModel(name)
  , cleanupWorkDirectory_(true)
{
  PhysicalModelImplementation::setInputs(inputs);
  PhysicalModelImplementation::setOutputs(outputs);
  setSteps(steps);
}

/* Virtual constructor */
CouplingPhysicalModel* CouplingPhysicalModel::clone() const
{
  return new CouplingPhysicalModel(*this);
}

/** Accessor to the steps */
CouplingStepCollection CouplingPhysicalModel::getSteps() const
{
  return steps_;
}

String CouplingPhysicalModel::getStepsMacro(const String & offset) const
{
  OSS oss;
  oss << offset << "steps = []\n";
  for (UnsignedInteger i = 0; i < steps_.getSize(); ++ i)
  {
    const CouplingStep step(steps_[i]);
    const CouplingInputFileCollection inputFiles(step.getInputFiles());
    oss << offset << "input_files = []\n";
    for (UnsignedInteger j = 0; j < inputFiles.getSize(); ++ j)
    {
      const CouplingInputFile inputFile(inputFiles[j]);
      if (inputFile.getPath().empty())
        continue;
      oss << offset << "input_file" << j << " = persalys.CouplingInputFile('"<<EscapePath(inputFile.getPath())<<"')\n";
      oss << offset << "input_file" << j <<".setConfiguredPath('" << EscapePath(inputFile.getConfiguredPath())<<"')\n";
      if (inputFile.getVariableNames().getSize() > 0)
        oss << offset << "input_file" << j <<".setVariables("
            << Parameters::GetOTDescriptionStr(inputFile.getVariableNames())<<", "
            << Parameters::GetOTDescriptionStr(inputFile.getTokens())<<", "
            << Parameters::GetOTDescriptionStr(inputFile.getFormats())<<")\n";
      oss << offset << "input_files.append(input_file"<<j<<")\n";
    }
    const CouplingResourceFileCollection resourceFiles(step.getResourceFiles());
    oss << offset << "resource_files = []\n";
    for (UnsignedInteger j = 0; j < resourceFiles.getSize(); ++ j)
    {
      const CouplingResourceFile resourceFile(resourceFiles[j]);
      if (resourceFile.getPath().empty())
        continue;
      oss << offset << "resource_file" << j << " = persalys.CouplingResourceFile('"<<EscapePath(resourceFile.getPath())<<"')\n";
      oss << offset << "resource_files.append(resource_file"<<j<<")\n";
    }
    const CouplingOutputFileCollection outputFiles(step.getOutputFiles());
    oss << offset << "output_files = []\n";
    for (UnsignedInteger j = 0; j < outputFiles.getSize(); ++ j)
    {
      const CouplingOutputFile outputFile(outputFiles[j]);
      if (outputFile.getPath().empty())
        continue;
      oss << offset << "output_file" << j << " = persalys.CouplingOutputFile('"<<EscapePath(outputFile.getPath())<<"')\n";
      if (outputFile.getVariableNames().getSize() > 0)
        oss << offset << "output_file" << j
	    <<".setVariables("
	    << Parameters::GetOTDescriptionStr(outputFile.getVariableNames())
	    <<", "<<Parameters::GetOTDescriptionStr(EscapeSpecialCharacters(outputFile.getTokens()))
	    <<", "<< Parameters::GetOTPointStr(outputFile.getSkipTokens())
	    <<", "<< Parameters::GetOTPointStr(outputFile.getSkipLines())
	    <<", "<< Parameters::GetOTPointStr(outputFile.getSkipColumns())<<")\n";
      oss << offset << "output_files.append(output_file"<<j<<")\n";
    }
    // escape backslashes and single quotes
    oss << offset << "step" << i << " = persalys.CouplingStep('"<<EscapeQuotes(EscapePath(step.getCommand()))<<"', input_files, resource_files, output_files)\n";
    oss << offset << "step" << i << ".setIsShell(" << (step.getIsShell() ? "True": "False") << ")\n";
    if(!step.getCode().empty())
      oss << offset << "step" << i << ".setCode('" << step.getEscapedCode() << "')\n";
    oss << offset << "steps.append(step"<<i<<")\n";
  }
  return oss;
}

void CouplingPhysicalModel::setSteps(const CouplingStepCollection & steps)
{
  steps_ = steps;
  updateCode();
}

void CouplingPhysicalModel::updateCode()
{
  CouplingStepCollection steps = getSteps();
  Description inputNames;
  Description outputNames;

  // retrieve output variables
  for (UnsignedInteger i = 0; i < steps.getSize(); ++ i)
  {
    const CouplingStep step(steps[i]);
    const CouplingOutputFileCollection outputFiles(step.getOutputFiles());
    for (UnsignedInteger j = 0; j < outputFiles.getSize(); ++ j)
    {
      const CouplingOutputFile outputFile(outputFiles[j]);
      if (outputFile.getPath().empty())
        continue;
      const Description variableNames(outputFile.getVariableNames());
      for (UnsignedInteger k = 0; k < variableNames.getSize(); ++ k)
      {
        if (!outputNames.contains(variableNames[k]))
          outputNames.add(variableNames[k]);
      }
    }
    if(!step.getCode().empty())
      for(UnsignedInteger i=0; i<step.getPPOutputs().getSize(); ++i)
        if(!outputNames.contains(step.getPPOutputs()[i]))
          outputNames.add(step.getPPOutputs()[i]);
  }

  // retrieve input variables
  for (UnsignedInteger i = 0; i < steps.getSize(); ++ i)
  {
    const CouplingStep step(steps[i]);
    const CouplingInputFileCollection inputFiles(step.getInputFiles());
    for (UnsignedInteger j = 0; j < inputFiles.getSize(); ++ j)
    {
      const CouplingInputFile inputFile(inputFiles[j]);
      if (inputFile.getPath().empty())
        continue;
      const Description variableNames(inputFile.getVariableNames());
      for (UnsignedInteger k = 0; k < variableNames.getSize(); ++ k)
      {
        if (!inputNames.contains(variableNames[k]) && !outputNames.contains(variableNames[k]))
          inputNames.add(variableNames[k]);
      }
    }
    if(!step.getCode().empty())
      for(UnsignedInteger i=0; i<step.getPPInputs().getSize(); ++i)
        if(!inputNames.contains(step.getPPInputs()[i]) &&
           !outputNames.contains(step.getPPInputs()[i]))
          inputNames.add(step.getPPInputs()[i]);
  }
  OSS code;
  code << "import tempfile\n";
  code << "import openturns.coupling_tools as otct\n";
  code << "import persalys\n";
  code << "import shutil\n";
  code << "import os\n";
  code << "import re\n";
  code << "import hashlib\n";
  code << "import struct\n\n";
  code << "def _exec(";
  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++ i)
  {
    code << inputNames[i];
    if (i < inputNames.getSize() - 1)
      code << ", ";
  }
  code << "):\n";

  code << getStepsMacro("    ");
  code << "    all_vars = dict(zip(" << Parameters::GetOTDescriptionStr(inputNames) << ", [";
  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++ i)
  {
    code << inputNames[i];
    if (i < inputNames.getSize() - 1)
      code << ", ";
  }
  code << "]))\n";

  code << "    checksum = hashlib.sha1()\n";
  code << "    [checksum.update(hex(struct.unpack('<Q', struct.pack('<d', x))[0]).encode()) for x in all_vars.values()]\n";
  code << "    global workdir\n";
  if(!workDir_.empty())
    code << "    workdir = os.path.join('"+EscapeQuotes(EscapePath(workDir_))+"', 'persalys_' + checksum.hexdigest())\n";
  else
    code << "    workdir = os.path.join(tempfile.gettempdir(), 'persalys_' + checksum.hexdigest())\n";
  code << "    if not os.path.exists(workdir):\n";
  code << "        os.makedirs(workdir)\n";
  code << "    for step in steps:\n";
  code << "        for input_file in step.getInputFiles():\n";
  code << "            if not input_file.getPath():\n";
  code << "                continue\n";
  code << "            input_values = [all_vars[varname] for varname in input_file.getVariableNames()]\n";
  code << "            formats = input_file.getFormats()\n";
  code << "            if formats.isBlank():\n";
  code << "                formats = None\n";
  code << "            otct.replace(input_file.getPath(), os.path.join(workdir, input_file.getConfiguredPath()), input_file.getTokens(), input_values, formats=formats, encoding=step.getEncoding())\n";
  code << "        for resource_file in step.getResourceFiles():\n";
  code << "            if not resource_file.getPath():\n";
  code << "                continue\n";
  code << "            if os.path.isfile(resource_file.getPath()):\n";
  code << "                shutil.copy(resource_file.getPath(), os.path.join(workdir, os.path.basename(resource_file.getPath())))\n";
  code << "            elif os.path.isdir(resource_file.getPath()):\n";
  code << "                dest = os.path.join(workdir, os.path.basename(resource_file.getPath()))\n";
  code << "                if os.path.exists(dest):\n";
  code << "                    shutil.rmtree(dest)\n";
  code << "                shutil.copytree(resource_file.getPath(), dest)\n";
  code << "            else:\n";
  code << "                raise ValueError('cannot handle file:', resource_file.getPath())\n";
  code << "        if len(step.getCommand()) > 0:\n";
  code << "            timeout = step.getTimeOut()\n";
  code << "            if timeout <= 0:\n";
  code << "                timeout = None\n";
  code << "            otct.execute(step.getCommand(), cwd=workdir, shell=step.getIsShell(), get_stderr=True, get_stdout=True, timeout=timeout)\n";
  code << "        for output_file in step.getOutputFiles():\n";
  code << "            if not output_file.getPath():\n";
  code << "                continue\n";
  code << "            outfile = os.path.join(workdir, output_file.getPath())\n";
  code << "            for varname, token, skip_tok, skip_line, skip_col in zip(output_file.getVariableNames(), output_file.getTokens(), output_file.getSkipTokens(), output_file.getSkipLines(), output_file.getSkipColumns()):\n";
  code << "                all_vars[varname] = otct.get_value(outfile, token=token, skip_token=int(skip_tok), skip_line=int(skip_line), skip_col=int(skip_col), encoding=step.getEncoding())\n";
  code << "        if step.getCode():\n";
  code << "            script = step.getCode()\n";

  code << "            regexsearch = re.search('def (\\w*)\\(.*\\):', script)\n";
  code << "            if regexsearch is not None:\n";
  code << "                script_funcname = regexsearch.group(1)\n";
  code << "            else:\n";
  code << "                raise RuntimeError('Could not find extra processing function name')\n";

  code << "            regexsearch = re.search('def \\w+\\(([\\w, ]+)\\):', script)\n";
  code << "            if regexsearch is not None:\n";
  code << "                script_invars = regexsearch.group(1).replace(' ', '').split(',')\n";
  code << "            else:\n";
  code << "                script_invars = []\n";
  code << "            regexsearch = re.search('return ([\\w, ]+)', script)\n";
  code << "            if regexsearch is not None:\n";
  code << "                script_outvars = regexsearch.group(1).replace(' ', '').split(',')\n";
  code << "            else:\n";
  code << "                script_outvars = []\n";
  code << "            exec_script = script+'\\nscript_output__ = '+script_funcname+'('+ ', '.join([str(all_vars[var]) for var in script_invars]) + ')\\n'\n";
  code << "            local_dict = globals()\n";
  code << "            exec(exec_script, globals(), local_dict)\n";
  code << "            script_output__ = local_dict['script_output__']\n";
  code << "            if len(script_outvars) == 1:\n";
  code << "                script_output__ = [script_output__]\n";
  code << "            for var, value in zip(script_outvars, script_output__):\n";
  code << "                all_vars[var] = value\n";

  if (cleanupWorkDirectory_)
    code << "    shutil.rmtree(workdir)\n";

  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++ i)
  {
    code << "    " << outputNames[i] <<" = all_vars['" << outputNames[i] << "']\n";
  }
  code << "    return ";
  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++ i)
  {
    code << outputNames[i];
    if (i < outputNames.getSize() - 1)
      code << ", ";
  }
  code << "\n";
  setCode(code);

  notify("stepsChanged");
}

Function CouplingPhysicalModel::generateFunction(const Description & outputNames) const
{
  return FileMemoizeFunction(PythonPhysicalModel::generateFunction(outputNames), getCacheInputFile(), getCacheOutputFile());
}


String CouplingPhysicalModel::getHtmlDescription(const bool deterministic) const
{
  OSS oss;
  oss << PhysicalModelImplementation::getHtmlDescription(deterministic);
  oss << "<h3>Outputs</h3><p>";
  oss << "<table style=\"width:100%\" border=\"1\" cellpadding=\"5\">";
  oss << "<tr>";
  oss << "  <th>Name</th>";
  oss << "  <th>Description</th>";
  oss << "</tr>";
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
  {
    oss << "<tr>";
    oss << "  <td>" << getOutputNames()[i] << "</td>";
    const String desc(getOutputs()[i].getDescription());
    oss << "  <td>" << (desc.empty() ? "-" : desc) << "</td>";
    oss << "</tr>";
  }
  oss << "</table></p>";
  for (UnsignedInteger i = 0; i < steps_.getSize(); ++i)
  {
    const CouplingStep step(steps_[i]);
    oss << "<h3>Step " << i << "</h3>";
    oss << "<section><p>";
    oss << step.getCommand();
    oss << "</p></section>";
  }
  return oss;
}


String CouplingPhysicalModel::getPythonScript() const
{
  OSS oss;

  for (UnsignedInteger i = 0; i < getInputDimension(); ++ i)
    oss << getInputs()[i].getPythonScript();

  for (UnsignedInteger i = 0; i < getOutputDimension(); ++ i)
    oss << getOutputs()[i].getPythonScript();

  oss << "inputs = [";
  for (UnsignedInteger i = 0; i < getInputDimension(); ++ i)
  {
    String inputName(getInputs()[i].getName());
    inputName = std::regex_replace(inputName, std::regex("[^0-9a-zA-Z_]"), "_");
    oss << inputName;
    if (i < getInputDimension() - 1)
      oss << ", ";
  }
  oss << "]\n";

  oss << "outputs = [";
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++ i)
  {
    String outputName(getOutputs()[i].getName());
    std::replace(outputName.begin(), outputName.end(), '.', '_');
    oss << outputName;
    if (i < getOutputDimension() - 1)
      oss << ", ";
  }
  oss << "]\n";

  oss << getStepsMacro();
  oss << getName() + " = persalys." << getClassName() << "('" << getName() << "', inputs, outputs, steps)\n";
  oss << getName() + ".setCleanupWorkDirectory(" << (getCleanupWorkDirectory() ? "True": "False") << ")\n";
  oss << getName() + ".setCacheFiles('" << EscapePath(getCacheInputFile()) << "', '"<<EscapePath(getCacheOutputFile()) <<"')\n";
  oss << PhysicalModelImplementation::getCopulaPythonScript();

  return oss;
}


/** String converter */
String CouplingPhysicalModel::__repr__() const
{
  OSS oss;
  oss << PhysicalModelImplementation::__repr__()
      << " steps=" << getSteps();
  return oss;
}

/** Whether the work dir is discarded */
void CouplingPhysicalModel::setCleanupWorkDirectory(const Bool cleanupWorkDirectory)
{
  cleanupWorkDirectory_ = cleanupWorkDirectory;
  updateCode();
}

Bool CouplingPhysicalModel::getCleanupWorkDirectory() const
{
  return cleanupWorkDirectory_;
}

void CouplingPhysicalModel::setCacheFiles(const OT::FileName & inputFile, const OT::FileName & outputFile)
{
  cacheInputFile_ = inputFile;
  cacheOutputFile_ = outputFile;
}

void CouplingPhysicalModel::setWorkDir(const OT::FileName & workDir)
{
  workDir_ = workDir;
  updateCode();
}


OT::FileName CouplingPhysicalModel::getWorkDir() const
{
  return workDir_;
}

OT::FileName CouplingPhysicalModel::getCacheInputFile() const
{
  return cacheInputFile_;
}

OT::FileName CouplingPhysicalModel::getCacheOutputFile() const
{
  return cacheOutputFile_;
}

/* Method save() stores the object through the StorageManager */
void CouplingPhysicalModel::save(Advocate & adv) const
{
  PythonPhysicalModel::save(adv);
  adv.saveAttribute("steps_", steps_);
  adv.saveAttribute("cleanupWorkDirectory_", cleanupWorkDirectory_);
  adv.saveAttribute("cacheInputFile_", cacheInputFile_);
  adv.saveAttribute("cacheOutputFile_", cacheOutputFile_);
  adv.saveAttribute("workDir_", workDir_);
}


/* Method load() reloads the object from the StorageManager */
void CouplingPhysicalModel::load(Advocate & adv)
{
  PythonPhysicalModel::load(adv);
  adv.loadAttribute("steps_", steps_);
  adv.loadAttribute("cleanupWorkDirectory_", cleanupWorkDirectory_);
  adv.loadAttribute("cacheInputFile_", cacheInputFile_);
  adv.loadAttribute("cacheOutputFile_", cacheOutputFile_);
  adv.loadAttribute("workDir_", workDir_);
}


}
