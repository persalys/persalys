//                                               -*- C++ -*-
/**
 *  @brief Input file metadata for coupling
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
#include "persalys/CouplingInputFile.hxx"
#include "persalys/BaseTools.hxx"
#include "persalys/PythonEnvironment.hxx"
#include "persalys/InterpreterUnlocker.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/PythonWrappingFunctions.hxx>

#include <boost/filesystem.hpp>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(CouplingInputFile)

static Factory<CouplingInputFile> Factory_CouplingInputFile;

/* Default constructor */
CouplingInputFile::CouplingInputFile(const String & templatePath)
  : PersistentObject()
{
  setPath(templatePath);
}


/* Virtual constructor */
CouplingInputFile* CouplingInputFile::clone() const
{
  return new CouplingInputFile(*this);
}


/* Path accessor */
void CouplingInputFile::setPath(const String & path)
{
  path_ = path;

  if (getConfiguredPath().empty())
  {
    // by default the configured path is the basename of the template path, minus the ".in" extension
    String configuredPath(boost::filesystem::path(path).filename().string());
    std::size_t pos = configuredPath.find(".in");
    if (pos != std::string::npos)
      configuredPath = configuredPath.substr(0, pos);
    setConfiguredPath(configuredPath);
  }
}

String CouplingInputFile::getPath() const
{
  return path_;
}

/* Template path accessor */
void CouplingInputFile::setConfiguredPath(const String & configuredPath)
{
  if (boost::filesystem::path(configuredPath).is_absolute())
    throw InvalidArgumentException(HERE) << "Configured path must be relative";
  configuredPath_ = configuredPath;
}

String CouplingInputFile::getConfiguredPath() const
{
  return configuredPath_;
}

/* Variables accessor */
void CouplingInputFile::setVariables(const Description & variableNames,
                                     const Description & tokens,
                                     const Description & formats)
{
  if (variableNames.getSize() != tokens.getSize())
    throw InvalidArgumentException(HERE) << "Variable names size must match tokens size";
  variableNames_= variableNames;
  tokens_ = tokens;
  formats_ = formats;
}

Description CouplingInputFile::getVariableNames() const
{
  return variableNames_;
}

Description CouplingInputFile::getTokens() const
{
  return tokens_;
}

Description CouplingInputFile::getFormats() const
{
  return formats_;
}

/* Simulating input file creation */
void CouplingInputFile::simulateInput(InputCollection inColl) const
{
  const Description variableNames(getVariableNames());
  OSS code;
  code << "import tempfile\n";
  code << "import os\n";
  code << "import openturns.coupling_tools as otct\n";
  code << "import persalys\n";
  code << "input_file = persalys.CouplingInputFile('"<<EscapePath(getPath())<<"')\n";
  code << "input_file.setConfiguredPath('" << EscapePath(getConfiguredPath())<<"')\n";
  code << "input_file.setVariables("
       << Parameters::GetOTDescriptionStr(getVariableNames())<<", "
       << Parameters::GetOTDescriptionStr(getTokens())<<", "
       << Parameters::GetOTDescriptionStr(getFormats())<<")\n";
  code << "all_vars = dict(zip(" << Parameters::GetOTDescriptionStr(variableNames) << ", [";
  for (UnsignedInteger i = 0; i < inColl.getSize(); ++ i)
  {
    code << inColl[i].getValue();
    if (i < inColl.getSize() - 1)
      code << ", ";
  }
  code << "]))\n";
  code << "input_values = [all_vars[varname] for varname in input_file.getVariableNames()]\n";
  code << "formats = input_file.getFormats()\n";
  code << "if formats.isBlank():\n";
  code << "    formats=None\n";
  code << "otct.replace(input_file.getPath(), os.path.join(tempfile.gettempdir(), input_file.getConfiguredPath()), tokens=input_file.getTokens(), values=input_values, formats=formats)\n";

  InterpreterUnlocker iul;
  PyObject * module = PyImport_AddModule("__main__");// Borrowed reference.
  PyObject * dict = PyModule_GetDict(module);// Borrowed reference.
  ScopedPyObjectPointer retValue(PyRun_String(code.str().c_str(), Py_file_input, dict, dict));
  handleExceptionTraceback();
}

/* String converter */
String CouplingInputFile::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " path=" << getPath()
      << " configuredPath=" << getConfiguredPath()
      << " variableNames=" << getVariableNames()
      << " tokens=" << getTokens()
      << " formats=" << getFormats();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CouplingInputFile::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("path_", path_);
  adv.saveAttribute("configuredPath_", configuredPath_);
  adv.saveAttribute("variableNames_", variableNames_);
  adv.saveAttribute("tokens_", tokens_);
  adv.saveAttribute("formats_", formats_);
}


/* Method load() reloads the object from the StorageManager */
void CouplingInputFile::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("path_", path_);
  adv.loadAttribute("configuredPath_", configuredPath_);
  adv.loadAttribute("variableNames_", variableNames_);
  adv.loadAttribute("tokens_", tokens_);
  adv.loadAttribute("formats_", formats_);
}

}
