//                                               -*- C++ -*-
/**
 *  @brief Resource file metadata for coupling
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
#include "persalys/CouplingResourceFile.hxx"

#include <openturns/PersistentObjectFactory.hxx>

#include <boost/filesystem.hpp>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(CouplingResourceFile)

static Factory<CouplingResourceFile> Factory_CouplingResourceFile;

/* Default constructor */
CouplingResourceFile::CouplingResourceFile(const String & path)
  : PersistentObject()
{
  setPath(path);
}


/* Virtual constructor */
CouplingResourceFile* CouplingResourceFile::clone() const
{
  return new CouplingResourceFile(*this);
}


/* Path accessor */
void CouplingResourceFile::setPath(const String & path)
{
  path_ = path;
}

String CouplingResourceFile::getPath() const
{
  return path_;
}

/* String converter */
String CouplingResourceFile::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " path=" << getPath();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CouplingResourceFile::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("path_", path_);
}


/* Method load() reloads the object from the StorageManager */
void CouplingResourceFile::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("path_", path_);
}

}
