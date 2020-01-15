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

#include <openturns/PersistentObjectFactory.hxx>

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
      << " configuredPath=" << getConfiguredPath()
      << " variableNames=" << getVariableNames()
      << " tokens=" << getTokens();
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
}


/* Method load() reloads the object from the StorageManager */
void CouplingInputFile::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("path_", path_);
  adv.loadAttribute("configuredPath_", configuredPath_);
  adv.loadAttribute("variableNames_", variableNames_);
  adv.loadAttribute("tokens_", tokens_);
}

}
