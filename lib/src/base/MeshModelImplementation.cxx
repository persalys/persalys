//                                               -*- C++ -*-
/**
 *  @brief Class to build mesh
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/MeshModelImplementation.hxx"

#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(MeshModelImplementation)

static Factory<MeshModelImplementation> Factory_MeshModelImplementation;

MeshModelImplementation::MeshModelImplementation()
  : PersistentObject()
  , mesh_()
  , indexParameters_()
{
  PersistentObject::setName("meshModel");
  indexParameters_.add(Variable("t"));
}


/* Virtual constructor */
MeshModelImplementation* MeshModelImplementation::clone() const
{
  return new MeshModelImplementation(*this);
}


Mesh MeshModelImplementation::getMesh() const
{
  return mesh_;
}


VariableCollection MeshModelImplementation::getIndexParameters() const
{
  return indexParameters_;
}


void MeshModelImplementation::setIndexParameters(const VariableCollection& parameters)
{
  // check
  if (parameters.getSize() != 1)
    throw InvalidArgumentException(HERE) << "Parameters dimension must be 1";

  indexParameters_ = parameters;
}


Interval MeshModelImplementation::getBounds() const
{
  throw NotYetImplementedException(HERE) << "In MeshModelImplementation::getBounds()";
}


Indices MeshModelImplementation::getNumberOfNodes() const
{
  throw NotYetImplementedException(HERE) << "In MeshModelImplementation::getNumberOfNodes()";
}


String MeshModelImplementation::getHtmlDescription() const
{
  OSS oss;

  oss << "<h3>Index parameters</h3><p>";
  oss << "<table style=\"width:100%\" border=\"1\" cellpadding=\"5\">";
  oss << "<tr>";
  oss << "  <th>Name</th>";
  oss << "  <th>Bounds</th>";
  oss << "  <th>Nodes</th>";
  oss << "</tr>";
  for (UnsignedInteger i = 0; i < indexParameters_.getSize(); ++i)
  {
    oss << "<tr>";
    oss << "  <td>" << indexParameters_[i].getName() << "</td>";
    oss << "  <td>" << Parameters::GetOTIntervalDescription(getBounds().getMarginal(i))[0] << "</td>";
    oss << "  <td>" << getNumberOfNodes()[i] << "</td>";
    oss << "</tr>";
  }

  oss << "</table></p>";
  return oss;
}


String MeshModelImplementation::getPythonScript() const
{
  throw NotYetImplementedException(HERE) << "In MeshModelImplementation::getPythonScript()";
}


/* String converter */
String MeshModelImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " index parameters=" << getIndexParameters()
      << " mesh=" << getMesh();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void MeshModelImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("mesh_", mesh_);
  adv.saveAttribute("indexParameters_", indexParameters_);
}


/* Method load() reloads the object from the StorageManager */
void MeshModelImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("mesh_", mesh_);
  adv.loadAttribute("indexParameters_", indexParameters_);
}
}
