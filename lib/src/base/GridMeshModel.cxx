//                                               -*- C++ -*-
/**
 *  @brief Class to build a regular mesh
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
#include "persalys/GridMeshModel.hxx"

#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/IntervalMesher.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(GridMeshModel)

static Factory<GridMeshModel> Factory_GridMeshModel;

GridMeshModel::GridMeshModel()
  : MeshModelImplementation()
  , bounds_(Interval(0, 1))
  , nbNodes_(Indices(1, 10))
{
  setParameters(bounds_, nbNodes_);
}


/* Constructor with parameters */
GridMeshModel::GridMeshModel(const Interval& bounds, const Indices& nbNodes)
  : MeshModelImplementation()
  , bounds_()
  , nbNodes_()
{
  setParameters(bounds, nbNodes);
}


/* Constructor with parameters */
GridMeshModel::GridMeshModel(const VariableCollection& parameters, const Interval& bounds, const Indices& nbNodes)
  : MeshModelImplementation()
  , bounds_()
  , nbNodes_()
{
  setIndexParameters(parameters);
  setParameters(bounds, nbNodes);
}


/* Virtual constructor */
GridMeshModel* GridMeshModel::clone() const
{
  return new GridMeshModel(*this);
}


void GridMeshModel::setParameters(const Interval& bounds, const Indices& nbNodes)
{
  // check
  if (bounds.getDimension() != getIndexParameters().getSize())
    throw InvalidArgumentException(HERE) << "Bounds dimension must be equal to the dimension of the mesh " << getIndexParameters().getSize();
  if (nbNodes.getSize() != getIndexParameters().getSize())
    throw InvalidArgumentException(HERE) << "The list of number of nodes dimension must be equal to the dimension of the mesh " << getIndexParameters().getSize();
  if (bounds.isNumericallyEmpty())
    throw InvalidArgumentException(HERE) << "The lower bounds must be less than the upper bounds";
  if (nbNodes[0] < 2)
    throw InvalidArgumentException(HERE) << "The mesh must contain at least two nodes";

  bounds_ = bounds;
  nbNodes_ = nbNodes;

  // build mesh
  Indices nbMesh(nbNodes_.getSize());
  for (UnsignedInteger i =0; i < nbNodes_.getSize(); ++i)
    nbMesh[i] = nbNodes_[i] - 1;
  Mesh mesh(IntervalMesher(nbMesh).build(bounds_));
  Sample sample(mesh.getVertices());
  sample.setDescription(Description(1, getIndexParameters()[0].getName()));
  mesh_ = Mesh(sample, mesh.getSimplices());
}


Interval GridMeshModel::getBounds() const
{
  return bounds_;
}


Indices GridMeshModel::getNumberOfNodes() const
{
  return nbNodes_;
}


String GridMeshModel::getPythonScript() const
{
  OSS oss;

  // variables
  Description paramNames;
  for (UnsignedInteger i = 0; i < getIndexParameters().getSize(); ++i)
  {
    oss << getIndexParameters()[i].getPythonScript();
    paramNames.add(getIndexParameters()[i].getName());
  }

  oss << getName() << " = persalys.GridMeshModel(" << Parameters::GetOTDescriptionStr(paramNames, false) << ", ot.Interval(" << Parameters::GetOTPointStr(getBounds().getLowerBound()) << ", " << Parameters::GetOTPointStr(getBounds().getUpperBound()) << "), " << getNumberOfNodes().__str__() << ")\n";

  return oss;
}


/* String converter */
String GridMeshModel::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " bounds=" << getBounds()
      << " nb nodes=" << getNumberOfNodes()
      << " index parameters=" << getIndexParameters()
      << " mesh=" << getMesh();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void GridMeshModel::save(Advocate & adv) const
{
  MeshModelImplementation::save(adv);
  adv.saveAttribute("bounds_", bounds_);
  adv.saveAttribute("nbNodes_", nbNodes_);
}


/* Method load() reloads the object from the StorageManager */
void GridMeshModel::load(Advocate & adv)
{
  MeshModelImplementation::load(adv);
  adv.loadAttribute("bounds_", bounds_);
  adv.loadAttribute("nbNodes_", nbNodes_);
}
}
