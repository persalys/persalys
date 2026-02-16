//                                               -*- C++ -*-
/**
 *  @brief Output file metadata for coupling
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
#include "persalys/CouplingOutputFile.hxx"
#include "persalys/BaseTools.hxx"
#include "persalys/PythonEnvironment.hxx"
#include "persalys/InterpreterUnlocker.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/PythonWrappingFunctions.hxx>

#include <filesystem>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(CouplingOutputFile)

static Factory<CouplingOutputFile> Factory_CouplingOutputFile;

/* Default constructor */
CouplingOutputFile::CouplingOutputFile(const String & path)
  : PersistentObject()
{
  setPath(path);
}


/* Virtual constructor */
CouplingOutputFile* CouplingOutputFile::clone() const
{
  return new CouplingOutputFile(*this);
}


/* Path accessor */
void CouplingOutputFile::setPath(const String & path)
{
  if (std::filesystem::path(path).is_absolute())
    throw InvalidArgumentException(HERE) << "Path must be relative";
  path_ = path;
}

String CouplingOutputFile::getPath() const
{
  return path_;
}

/* Variables accessor */
void CouplingOutputFile::setVariables(const Description & variableNames,
                                      const Description & tokens,
                                      const OT::Point & skipTokens,
                                      const OT::Point & skipLines,
                                      const OT::Point & skipColumns)
{
  if (variableNames.getSize() != tokens.getSize())
    throw InvalidArgumentException(HERE) << "Variable names size must match tokens size";
  if (skipLines.getSize() != tokens.getSize())
    throw InvalidArgumentException(HERE) << "Skip lines size must match tokens size";
  if (skipColumns.getSize() != tokens.getSize())
    throw InvalidArgumentException(HERE) << "Skip columns size must match tokens size";
  if (skipTokens.getSize() != tokens.getSize())
    throw InvalidArgumentException(HERE) << "Skip tokens size must match tokens size";
  ValidateVariables(variableNames);
  variableNames_ = variableNames;
  tokens_ = tokens;
  skipTokens_ = skipTokens;
  skipLines_ = skipLines;
  skipColumns_ = skipColumns;
}

Description CouplingOutputFile::getVariableNames() const
{
  return variableNames_;
}

Description CouplingOutputFile::getTokens() const
{
  return tokens_;
}

Point CouplingOutputFile::getSkipTokens() const
{
  return skipTokens_;
}

Point CouplingOutputFile::getSkipLines() const
{
  return skipLines_;
}

Point CouplingOutputFile::getSkipColumns() const
{
  return skipColumns_;
}

String CouplingOutputFile::checkOutputFile(const String &fname, const String &encoding) const
{
  if (encoding.empty())
    LOGWARN("Warning: no encoding passed to CouplingOutputFile::checkOutputFile. Using openTURNS default value (sys.getdefaultencoding())");
  
  OSS code;
  code << "import os\n";
  code << "import re\n";
  code << "import persalys\n";
  code << "import openturns.coupling_tools as otct\n";
  code << "def _check_file():\n";
  code << "    outfile = os.path.basename('" << fname << "')\n";
  code << "    output_file = persalys.CouplingOutputFile(outfile)\n";
  code << "    output_file.setVariables("
       << Parameters::GetOTDescriptionStr(getVariableNames())
       << ", " << Parameters::GetOTDescriptionStr(getTokens())
       << ", " << Parameters::GetOTPointStr(getSkipTokens())
       << ", " << Parameters::GetOTPointStr(getSkipLines())
       << ", " << Parameters::GetOTPointStr(getSkipColumns()) << ")\n";
  code << "    varnames = []\n";
  code << "    values = []\n";
  code << "    for varname, token, skip_tok, skip_line, skip_col in zip(output_file.getVariableNames(), output_file.getTokens(), output_file.getSkipTokens(), output_file.getSkipLines(), output_file.getSkipColumns()):\n";
  code << "        try:\n";
  code << "            token_esc = re.escape(token)\n";
  code << "            value = otct.get_value('" << fname << "', token=token_esc, skip_token=int(skip_tok), skip_line=int(skip_line), skip_col=int(skip_col)";
  if (!encoding.empty())
    code << ", encoding='" << encoding << "'";
  code << ")\n";
  code << "        except:\n";
  code << "            value = None\n";
  code << "        varnames.append(varname)\n";
  code << "        values.append(value)\n";
  code << "    return varnames, values\n";

  InterpreterUnlocker iul;
  PyObject * const module = PyImport_AddModule("__main__"); // Borrowed reference
  PyObject * const dict = PyModule_GetDict(module); // Borrowed reference

  ScopedPyObjectPointer retValue(PyRun_String(code.str().c_str(), Py_file_input, dict, dict));
  handleException();

  PyObject * const script = PyDict_GetItemString(dict, "_check_file");
  if (script == NULL)
    throw InternalException(HERE) << "no _check_file function";

  ScopedPyObjectPointer result(PyObject_CallObject(script, NULL));
  handleException();

  // result est un tuple (varnames, values)
  if (!PyTuple_Check(result.get()) || PyTuple_Size(result.get()) != 2)
    throw InternalException(HERE) << "Python script did not return a tuple of size 2";

  PyObject * const pyVarNames = PyTuple_GetItem(result.get(), 0); // borrowed
  PyObject * const pyValues = PyTuple_GetItem(result.get(), 1);   // borrowed

  const Description varNames = convert<_PySequence_, Description>(pyVarNames);

  Description missingVars;
  OSS output;

  for (UnsignedInteger i = 0; i < varNames.getSize(); ++i)
  {
    ScopedPyObjectPointer pyValue(PySequence_GetItem(pyValues, i)); // New reference
    const String & varName = varNames[i];

    if (pyValue.get() == Py_None)
    {
      missingVars.add(varName);
    }
    else
    {
      const Scalar value = convert<_PyFloat_, Scalar>(pyValue.get());
      output << varName << "=" << value << "\n";
    }
  }

  OSS resultMessage;
  if (!missingVars.isEmpty())
  {
    resultMessage << "The following variables could not be found: ";
    resultMessage << Parameters::GetOTDescriptionStr(missingVars, false, false)  << "\n";
  }
  resultMessage << output.str();
  return resultMessage.str();
}

/* String converter */
String CouplingOutputFile::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " path=" << getPath()
      << " variableNames=" << getVariableNames()
      << " tokens=" << getTokens()
      << " skipTokens=" << getSkipTokens()
      << " skipLines=" << getSkipLines()
      << " skipColumns=" << getSkipColumns();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CouplingOutputFile::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("path_", path_);
  adv.saveAttribute("variableNames_", variableNames_);
  adv.saveAttribute("tokens_", tokens_);
  adv.saveAttribute("skipTokens_", skipTokens_);
  adv.saveAttribute("skipLines_", skipLines_);
  adv.saveAttribute("skipColumns_", skipColumns_);
}


/* Method load() reloads the object from the StorageManager */
void CouplingOutputFile::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("path_", path_);
  adv.loadAttribute("variableNames_", variableNames_);
  adv.loadAttribute("tokens_", tokens_);
  if(adv.hasAttribute("skipTokens_"))
    adv.loadAttribute("skipTokens_", skipTokens_);
  else
    skipTokens_ = OT::Point(variableNames_.getSize(), 0.);
  adv.loadAttribute("skipLines_", skipLines_);
  adv.loadAttribute("skipColumns_", skipColumns_);
}

}
