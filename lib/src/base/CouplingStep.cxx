//                                               -*- C++ -*-
/**
 *  @brief Intermediate coupling step
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/CouplingStep.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <regex>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(CouplingStep)

static Factory<CouplingStep> Factory_CouplingStep;
static Factory<PersistentCollection<CouplingInputFile> > Factory_PersistentCollectionCouplingInputFile;
static Factory<PersistentCollection<CouplingResourceFile> > Factory_PersistentCollectionCouplingResourceFile;
static Factory<PersistentCollection<CouplingOutputFile> > Factory_PersistentCollectionCouplingOutputFile;

/* Default constructor */
CouplingStep::CouplingStep(const String & command)
  : PersistentObject()
  , command_(command)
  , isShell_(false)
{
}

/* Default constructor */
CouplingStep::CouplingStep(const String & command,
                           const CouplingInputFileCollection & inputFiles,
                           const CouplingResourceFileCollection & resourceFiles,
                           const CouplingOutputFileCollection & outputFiles)
  : PersistentObject()
  , command_(command)
  , isShell_(false)
  , inputFiles_(inputFiles)
  , resourceFiles_(resourceFiles)
  , outputFiles_(outputFiles)
{
}

/* Virtual constructor */
CouplingStep* CouplingStep::clone() const
{
  return new CouplingStep(*this);
}


/* Command accessor */
void CouplingStep::setCommand(const String & command)
{
  command_ = command;
}

String CouplingStep::getCommand() const
{
  return command_;
}

/* Files accessor */
void CouplingStep::setInputFiles(const CouplingInputFileCollection & inputFiles)
{
  inputFiles_ = inputFiles;
}

CouplingInputFileCollection CouplingStep::getInputFiles() const
{
  return inputFiles_;
}

void CouplingStep::setResourceFiles(const CouplingResourceFileCollection & resourceFiles)
{
  resourceFiles_ = resourceFiles;
}

CouplingResourceFileCollection CouplingStep::getResourceFiles() const
{
  return resourceFiles_;
}

void CouplingStep::setOutputFiles(const CouplingOutputFileCollection & outputFiles)
{
  outputFiles_ = outputFiles;
}

CouplingOutputFileCollection CouplingStep::getOutputFiles() const
{
  return outputFiles_;
}

/* Whether the command is to be interpreted by the shell */
void CouplingStep::setIsShell(const Bool isShell)
{
  isShell_ = isShell;
}

Bool CouplingStep::getIsShell() const
{
  return isShell_;
}

void CouplingStep::setTimeOut(const double & timeOut)
{
  timeOut_ = timeOut;
}

double CouplingStep::getTimeOut() const
{
  return timeOut_;
}

void CouplingStep::setEncoding(const OT::String & encoding)
{
  encoding_ = encoding;
}

String CouplingStep::getEncoding() const
{
  return encoding_;
}

void CouplingStep::setCode(const OT::String & code)
{
  ppCode_ = code;
}

String CouplingStep::getCode() const
{
  return ppCode_;
}

String CouplingStep::getEscapedCode() const
{
  String code = std::regex_replace(getCode(), std::regex("\n", std::regex::extended), "\\n");
  return code;
}

Description CouplingStep::getPPOutputs() const
{
  String PPCode = getCode();
  Description vars;
  std::regex variable("([_a-zA-Z][_a-zA-Z0-9]*)");
  std::regex returnOutput("return[ ]+([_a-zA-Z0-9, ]+)", std::regex::extended);
  std::smatch what;
  if (std::regex_search(PPCode, what, returnOutput)) {
    String outputList = what[1];
    std::string::const_iterator start = outputList.begin(), end = outputList.end();
    while (std::regex_search(start, end, what, variable)) {
      start = what[0].second;
      vars.add(what[1]);
    }
  }
  return vars;
}

Description CouplingStep::getPPInputs() const
{
  String PPCode = getCode();
  Description vars;
  std::regex variable("([_a-zA-Z][_a-zA-Z0-9]*)");
  std::regex defFunc("def[ ]+[_a-zA-Z][_a-zA-Z0-9]*[ ]*\\(([_a-zA-Z0-9, ]*)\\)[ ]*:", std::regex::extended);
  std::smatch what;
  if (std::regex_search(PPCode, what, defFunc)) {
    String outputList = what[1];
    std::string::const_iterator start = outputList.begin(), end = outputList.end();
    while (std::regex_search(start, end, what, variable)) {
      start = what[0].second;
      vars.add(what[1]);
    }
  }
  return vars;
}

/* String converter */
String CouplingStep::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " command=" << getCommand()
      << " isShell=" << getIsShell()
      << " inputFiles=" << getInputFiles()
      << " resourceFiles=" << getResourceFiles()
      << " outputFiles=" << getOutputFiles()
      << " encoding=" << getEncoding()
      << " ppCode=" << getCode();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CouplingStep::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("command_", command_);
  adv.saveAttribute("inputFiles_", inputFiles_);
  adv.saveAttribute("resourceFiles_", resourceFiles_);
  adv.saveAttribute("outputFiles_", outputFiles_);
  adv.saveAttribute("isShell_", isShell_);
  adv.saveAttribute("timeOut_", timeOut_);
  adv.saveAttribute("encoding_", encoding_);
  adv.saveAttribute("ppCode_", ppCode_);
}


/* Method load() reloads the object from the StorageManager */
void CouplingStep::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("command_", command_);
  adv.loadAttribute("inputFiles_", inputFiles_);
  adv.loadAttribute("resourceFiles_", resourceFiles_);
  adv.loadAttribute("outputFiles_", outputFiles_);
  adv.loadAttribute("isShell_", isShell_);
  adv.loadAttribute("timeOut_", timeOut_);
  adv.loadAttribute("encoding_", encoding_);
  if(adv.hasAttribute("ppCode_"))
    adv.loadAttribute("ppCode_", ppCode_);
  else
    ppCode_ = "";
}

}
