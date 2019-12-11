//                                               -*- C++ -*-
/**
 *  @brief Input file metadata for coupling
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
#include "persalys/CouplingInputFile.hxx"

#include <openturns/PersistentObjectFactory.hxx>

#include <boost/filesystem.hpp>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(CouplingInputFile)

static Factory<CouplingInputFile> Factory_CouplingInputFile;

/* Default constructor */
CouplingInputFile::CouplingInputFile(const String & path)
  : PersistentObject()
{
  setPath(path);
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
}

String CouplingInputFile::getPath() const
{
  return path_;
}

/* Template path accessor */
void CouplingInputFile::setTemplatePath(const String & templatePath)
{
  templatePath_ = templatePath;
}

String CouplingInputFile::getTemplatePath() const
{
  return templatePath_;
}

/* Variables accessor */
void CouplingInputFile::setVariables(const Description & variableNames, const Description & tokens)
{
  if (variableNames.getSize() != tokens.getSize())
    throw InvalidArgumentException(HERE) << "Variable names size must match tokens size";
  variableNames_= variableNames;
  tokens_ = tokens;
}

Description CouplingInputFile::getVariableNames() const
{
  return variableNames_;
}

Description CouplingInputFile::getTokens() const
{
  return tokens_;
}


/* String converter */
String CouplingInputFile::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " path=" << getPath()
      << " templatePath=" << getTemplatePath()
      << " variableNames=" << getVariableNames()
      << " tokens=" << getTokens();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CouplingInputFile::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("path_", path_);
  adv.saveAttribute("templatePath_", templatePath_);
  adv.saveAttribute("variableNames_", variableNames_);
  adv.saveAttribute("tokens_", tokens_);
}


/* Method load() reloads the object from the StorageManager */
void CouplingInputFile::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("path_", path_);
  adv.loadAttribute("templatePath_", templatePath_);
  adv.loadAttribute("variableNames_", variableNames_);
  adv.loadAttribute("tokens_", tokens_);
}

}
