//                                               -*- C++ -*-
/**
 *  @brief Output file metadata for coupling
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
#include "persalys/CouplingOutputFile.hxx"

#include <openturns/PersistentObjectFactory.hxx>

#include <boost/filesystem.hpp>

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
  if (boost::filesystem::path(path).is_absolute())
    throw InvalidArgumentException(HERE) << "Path must be relative";
  path_ = path;
}

String CouplingOutputFile::getPath() const
{
  return path_;
}

/* Variables accessor */
void CouplingOutputFile::setVariables(const Description & variableNames, const Description & tokens,
                                      const OT::Point & skipLines, const OT::Point & skipColumns)
{
  if (variableNames.getSize() != tokens.getSize())
    throw InvalidArgumentException(HERE) << "Variable names size must match tokens size";
  if (skipLines.getSize() != tokens.getSize())
    throw InvalidArgumentException(HERE) << "Skip lines size must match tokens size";
  if (skipColumns.getSize() != tokens.getSize())
    throw InvalidArgumentException(HERE) << "Skip columns size must match tokens size";
  variableNames_= variableNames;
  tokens_ = tokens;
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

Point CouplingOutputFile::getSkipLines() const
{
  return skipLines_;
}

Point CouplingOutputFile::getSkipColumns() const
{
  return skipColumns_;
}

/* String converter */
String CouplingOutputFile::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " path=" << getPath()
      << " variableNames=" << getVariableNames()
      << " tokens=" << getTokens()
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
  adv.loadAttribute("skipLines_", skipLines_);
  adv.loadAttribute("skipColumns_", skipColumns_);
}

}
