//                                               -*- C++ -*-
/**
 *  @brief Retrieve information from an FMU file
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/FMUInfo.hxx"
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/PythonWrappingFunctions.hxx>

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(FMUInfo);

static Factory<FMUInfo> Factory_FMUInfo;

/* Default constructor */
FMUInfo::FMUInfo()
  : PersistentObject()
{
}

/* Constructor with parameters */
FMUInfo::FMUInfo(const String & fileName)
  : PersistentObject()
  , fileName_(fileName)
{
  if (fileName.empty())
    throw InvalidArgumentException(HERE) << "Empty file name";

  initialize();
}


FMUInfo * FMUInfo::clone() const
{
  return new FMUInfo(*this);
}

class InterpreterUnlocker
{
public:
  InterpreterUnlocker() { gstate_ = PyGILState_Ensure(); }
  ~InterpreterUnlocker() { PyGILState_Release(gstate_); }
private:
  PyGILState_STATE gstate_;
};

void FMUInfo::initialize()
{
  InterpreterUnlocker uil;

  ScopedPyObjectPointer otfmi_fmiModule(PyImport_ImportModule("otfmi.fmi")); // new reference
  handleException();// if cannot import otfm

  assert(otfmi_fmiModule.get());
  PyObject * otfmi_fmiDict = PyModule_GetDict(otfmi_fmiModule.get());
  assert(otfmi_fmiDict);
  PyObject * load_fmuMethod = PyDict_GetItemString(otfmi_fmiDict, "load_fmu");
  assert(load_fmuMethod);
  ScopedPyObjectPointer fileName_py(convert< String, _PyString_>(fileName_));
  ScopedPyObjectPointer model(PyObject_CallFunctionObjArgs(load_fmuMethod, fileName_py.get(), NULL)); // new reference

  // loading can fail if no binary for the target platform
  handleException();

  PyObject * get_name_variableMethod = PyDict_GetItemString(otfmi_fmiDict, "get_name_variable");
  ScopedPyObjectPointer varList(PyObject_CallFunctionObjArgs(get_name_variableMethod, model.get(), NULL)); // new reference
  variableNames_ = convert<_PySequence_, Description>(varList.get());

  PyObject * get_causalityMethod = PyDict_GetItemString(otfmi_fmiDict, "get_causality");
  ScopedPyObjectPointer causalityList(PyObject_CallFunctionObjArgs(get_causalityMethod, model.get(), NULL)); // new reference
  causality_ = convert<_PySequence_, Indices>(causalityList.get());

  PyObject * get_variabilityMethod = PyDict_GetItemString(otfmi_fmiDict, "get_variability");
  ScopedPyObjectPointer variabilityList(PyObject_CallFunctionObjArgs(get_variabilityMethod, model.get(), NULL)); // new reference
  variability_ = convert<_PySequence_, Indices>(variabilityList.get());

  PyObject * get_start_valueMethod = PyDict_GetItemString(otfmi_fmiDict, "get_start_value");
  ScopedPyObjectPointer startDict(PyObject_CallFunctionObjArgs(get_start_valueMethod, model.get(), NULL)); // new reference
  ScopedPyObjectPointer startKeys(PyObject_CallMethod(startDict.get(), const_cast<char*>("keys"),  NULL)); // new reference
  ScopedPyObjectPointer startValues(PyObject_CallMethod(startDict.get(), const_cast<char*>("values"),  NULL)); // new reference
  startKeys_ = convert<_PySequence_, Description>(startKeys.get());
  startValues_ = convert<_PySequence_, Point>(startValues.get());

  ScopedPyObjectPointer authorStr(PyObject_CallMethod(model.get(), const_cast<char*>("get_author"), NULL)); // new reference
  author_ = convert<_PyString_, String>(authorStr.get());

  ScopedPyObjectPointer copyrightStr(PyObject_CallMethod(model.get(), const_cast<char*>("get_copyright"), NULL)); // new reference
  copyright_ = convert<_PyString_, String>(copyrightStr.get());

  ScopedPyObjectPointer toolStr(PyObject_CallMethod(model.get(), const_cast<char*>("get_generation_tool"), NULL)); // new reference
  tool_ = convert<_PyString_, String>(toolStr.get());

  ScopedPyObjectPointer dateTimeStr(PyObject_CallMethod(model.get(), const_cast<char*>("get_generation_date_and_time"), NULL)); // new reference
  dateTime_ = convert<_PyString_, String>(dateTimeStr.get());

  ScopedPyObjectPointer guidStr(PyObject_CallMethod(model.get(), const_cast<char*>("get_guid"), NULL)); // new reference
  guid_ = convert<_PyString_, String>(guidStr.get());

  ScopedPyObjectPointer identifierStr(PyObject_CallMethod(model.get(), const_cast<char*>("get_identifier"), NULL)); // new reference
  identifier_ = convert<_PyString_, String>(identifierStr.get());

  ScopedPyObjectPointer platformStr(PyObject_CallMethod(model.get(), const_cast<char*>("get_model_types_platform"), NULL)); // new reference
  platform_ = convert<_PyString_, String>(platformStr.get());

  ScopedPyObjectPointer versionStr(PyObject_CallMethod(model.get(), const_cast<char*>("get_model_version"), NULL)); // new reference
  version_ = convert<_PyString_, String>(versionStr.get());

  ScopedPyObjectPointer fmiVersionStr(PyObject_CallMethod(model.get(), const_cast<char*>("get_version"), NULL)); // new reference
  fmiVersion_ = convert<_PyString_, String>(fmiVersionStr.get());

  if (variableNames_.getSize() != causality_.getSize())
  {
    throw InternalException(HERE) << "variableNames_.getSize() != causality_.getSize()";
  }
  if (variableNames_.getSize() != variability_.getSize())
  {
    throw InternalException(HERE) << "variableNames_.getSize() != variability_.getSize()";
  }
  if (startKeys_.getSize() != startValues_.getSize())
  {
    throw InternalException(HERE) << "startKeys_.getSize() != startValues_.getSize()";
  }
}


String FMUInfo::__repr__() const
{
  return OSS() << "class=" << getClassName()
               << " variables=" << variableNames_
               << " causality=" << causality_
               << " variability_=" << variability_;
}


String FMUInfo::getFileName() const
{
  return fileName_;
}


String FMUInfo::getAuthor() const
{
  return author_;
}


String FMUInfo::getCopyright() const
{
  return copyright_;
}


String FMUInfo::getTool() const
{
  return tool_;
}


String FMUInfo::getDateTime() const
{
  return dateTime_;
}


String FMUInfo::getGUID() const
{
  return guid_;
}


String FMUInfo::getIdentifier() const
{
  return identifier_;
}


String FMUInfo::getPlatform() const
{
  return platform_;
}


String FMUInfo::getVersion() const
{
  return version_;
}


String FMUInfo::getFMIVersion() const
{
  return fmiVersion_;
}


Description FMUInfo::getVariableNames() const
{
  return variableNames_;
}


Indices FMUInfo::getCausality() const
{
  return causality_;
}


Indices FMUInfo::getVariability() const
{
  return variability_;
}


Description FMUInfo::getStartKeys() const
{
  return startKeys_;
}


Point FMUInfo::getStartValues() const
{
  return startValues_;
}


/* Method save() stores the object through the StorageManager */
void FMUInfo::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("fileName_", fileName_);
  adv.saveAttribute("author_", author_);
  adv.saveAttribute("copyright_", copyright_);
  adv.saveAttribute("tool_", tool_);
  adv.saveAttribute("dateTime_", dateTime_);
  adv.saveAttribute("guid_", guid_);
  adv.saveAttribute("identifier_", identifier_);
  adv.saveAttribute("platform_", platform_);
  adv.saveAttribute("version_", version_);
  adv.saveAttribute("fmiVersion_", fmiVersion_);
  adv.saveAttribute("variableNames_", variableNames_);
  adv.saveAttribute("causality_", causality_);
  adv.saveAttribute("variability_", variability_);
  adv.saveAttribute("startKeys_", startKeys_);
  adv.saveAttribute("startValues_", startValues_);
}


/* Method load() reloads the object from the StorageManager */
void FMUInfo::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("fileName_", fileName_);
  adv.loadAttribute("author_", author_);
  adv.loadAttribute("copyright_", copyright_);
  adv.loadAttribute("tool_", tool_);
  adv.loadAttribute("dateTime_", dateTime_);
  adv.loadAttribute("guid_", guid_);
  adv.loadAttribute("identifier_", identifier_);
  adv.loadAttribute("platform_", platform_);
  adv.loadAttribute("version_", version_);
  adv.loadAttribute("fmiVersion_", fmiVersion_);
  adv.loadAttribute("variableNames_", variableNames_);
  adv.loadAttribute("causality_", causality_);
  adv.loadAttribute("variability_", variability_);
  adv.loadAttribute("startKeys_", startKeys_);
  adv.loadAttribute("startValues_", startValues_);
}


}
