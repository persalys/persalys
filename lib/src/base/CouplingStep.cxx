//                                               -*- C++ -*-
/**
 *  @brief Intermediate coupling step
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
#include "persalys/CouplingStep.hxx"

#include <openturns/PersistentObjectFactory.hxx>

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


/* String converter */
String CouplingStep::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " command=" << getCommand()
      << " isShell=" << getIsShell()
      << " inputFiles=" << getInputFiles()
      << " resourceFiles=" << getResourceFiles()
      << " outputFiles=" << getOutputFiles();
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
}

}
