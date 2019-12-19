//                                               -*- C++ -*-
/**
 *  @brief Intermediate coupling step
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
#include "persalys/CouplingPhysicalModel.hxx"
#include "persalys/PythonScriptEvaluation.hxx"
#include "persalys/BaseTools.hxx"
#include "persalys/FileMemoizeFunction.hxx"

#include <openturns/PersistentObjectFactory.hxx>

#include <boost/regex.hpp>

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

inline String EscapePath(const FileName & filename)
{
  FileName escapedPath = boost::regex_replace(filename, boost::regex("\\\\"), "\\\\\\\\");
  return escapedPath;
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
      if (!inputFile.getTemplatePath().empty())
        oss << offset << "input_file" << j <<".setTemplatePath('" << EscapePath(inputFile.getTemplatePath())<<"')\n";
      if (inputFile.getVariableNames().getSize() > 0)
        oss << offset << "input_file" << j <<".setVariables(" << Parameters::GetOTDescriptionStr(inputFile.getVariableNames())<<", "<<Parameters::GetOTDescriptionStr(inputFile.getTokens())<<")\n";
      oss << offset << "input_files.append(input_file"<<j<<")\n";
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
        oss << offset << "output_file" << j <<".setVariables(" << Parameters::GetOTDescriptionStr(outputFile.getVariableNames())<<", "<<Parameters::GetOTDescriptionStr(outputFile.getTokens())<<", "<<outputFile.getSkipLines().__str__()<<", "<<outputFile.getSkipColumns().__str__()<<")\n";
      oss << offset << "output_files.append(output_file"<<j<<")\n";
    }
    // escape backslashes
    oss << offset << "step" << i << " = persalys.CouplingStep('"<<EscapePath(step.getCommand())<<"', input_files, output_files)\n";
    oss << offset << "step" << i << ".setIsShell(" << (step.getIsShell() ? "True": "False") << ")\n";
    oss << offset << "steps.append(step"<<i<<")\n";
  }
  return oss;
}

void CouplingPhysicalModel::setSteps(const CouplingStepCollection & steps)
{
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
  }
  steps_ = steps;

  OSS code;
  code << "import tempfile\n";
  code << "import openturns.coupling_tools as otct\n";
  code << "import persalys\n";
  code << "import shutil\n";
  code << "import os\n";
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

//   code << "    workdir = tempfile.mkdtemp()\n";
  code << "    checksum = hashlib.sha1()\n";
  code << "    [checksum.update(hex(struct.unpack('<Q', struct.pack('<d', x))[0]).encode()) for x in all_vars.values()]\n";
  code << "    workdir = os.path.join(tempfile.gettempdir(), 'persalys_' + checksum.hexdigest())\n";
  code << "    if not os.path.exists(workdir):\n";
  code << "        os.makedirs(workdir)\n";

  code << "    for step in steps:\n";
  code << "        for input_file in step.getInputFiles():\n";
  code << "            if not input_file.getPath():\n";
  code << "                continue\n";
  code << "            if not input_file.getTemplatePath():\n";
  code << "                if os.path.isfile(input_file.getPath()):\n";
  code << "                    shutil.copy(input_file.getPath(), os.path.join(workdir, input_file.getPath()))\n";
  code << "                elif os.path.isdir(input_file.getPath()):\n";
  code << "                    shutil.copytree(input_file.getPath(), os.path.join(workdir, input_file.getPath()))\n";
  code << "                else:\n";
  code << "                    raise ValueError('cannot handle file:', input_file.getPath())\n";
  code << "            else:\n";
  code << "                input_values = [all_vars[varname] for varname in input_file.getVariableNames()]\n";
  code << "                otct.replace(input_file.getTemplatePath(), os.path.join(workdir, input_file.getPath()), input_file.getTokens(), input_values)\n";
  code << "        if len(step.getCommand()) > 0:\n";
  code << "            otct.execute(step.getCommand(), workdir=workdir, is_shell=step.getIsShell())\n";
  code << "        for output_file in step.getOutputFiles():\n";
  code << "            if not output_file.getPath():\n";
  code << "                continue\n";
  code << "            outfile = os.path.join(workdir, output_file.getPath())\n";
  code << "            for varname, token, skip_line, skip_col in zip(output_file.getVariableNames(), output_file.getTokens(), output_file.getSkipLines(), output_file.getSkipColumns()):\n";
  code << "                all_vars[varname] = otct.get_value(outfile, token=token, skip_line=skip_line, skip_col=skip_col)\n";

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
    std::replace(inputName.begin(), inputName.end(), '.', '_');
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
  oss << getName() + " = persalys." << getClassName() << "('" << getName() << "', steps)\n";
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
  if (cleanupWorkDirectory != cleanupWorkDirectory_)
  {
    cleanupWorkDirectory_ = cleanupWorkDirectory;
    setSteps(getSteps());
  }
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
}


/* Method load() reloads the object from the StorageManager */
void CouplingPhysicalModel::load(Advocate & adv)
{
  PythonPhysicalModel::load(adv);
  adv.loadAttribute("steps_", steps_);
  adv.loadAttribute("cleanupWorkDirectory_", cleanupWorkDirectory_);
  adv.loadAttribute("cacheInputFile_", cacheInputFile_);
  adv.loadAttribute("cacheOutputFile_", cacheOutputFile_);
}


}
