//                                               -*- C++ -*-
/**
 *  @brief Class to build mesh
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/MeshModel.hxx"

#include "persalys/GridMeshModel.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/IntervalMesher.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(MeshModel)

/* Default constructor */
MeshModel::MeshModel()
  : TypedInterfaceObject<MeshModelImplementation>(new GridMeshModel())
{
}


/* Default constructor */
MeshModel::MeshModel(const MeshModelImplementation & implementation)
  : TypedInterfaceObject<MeshModelImplementation>(implementation.clone())
{

}


/* Constructor from implementation */
MeshModel::MeshModel(const Implementation & p_implementation)
  : TypedInterfaceObject<MeshModelImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}

/* Constructor from implementation pointer */
MeshModel::MeshModel(MeshModelImplementation * p_implementation)
  : TypedInterfaceObject<MeshModelImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


Mesh MeshModel::getMesh() const
{
  return getImplementation()->getMesh();
}


VariableCollection MeshModel::getIndexParameters() const
{
  return getImplementation()->getIndexParameters();
}


void MeshModel::setIndexParameters(const VariableCollection& parameters)
{
  getImplementation()->setIndexParameters(parameters);
}


Interval MeshModel::getBounds() const
{
  return getImplementation()->getBounds();
}


Indices MeshModel::getNumberOfNodes() const
{
  return getImplementation()->getNumberOfNodes();
}


String MeshModel::getHtmlDescription() const
{
  return getImplementation()->getHtmlDescription();
}


String MeshModel::getPythonScript() const
{
  return getImplementation()->getPythonScript();
}


/* Method save() stores the object through the StorageManager */
void MeshModel::save(Advocate & adv) const
{
  getImplementation()->save(adv);
}


/* Method load() reloads the object from the StorageManager */
void MeshModel::load(Advocate & adv)
{
  getImplementation()->load(adv);
}
}
