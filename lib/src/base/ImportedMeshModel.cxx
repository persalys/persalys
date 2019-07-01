//                                               -*- C++ -*-
/**
 *  @brief Class to build mesh with a file
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
#include "persalys/ImportedMeshModel.hxx"

#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(ImportedMeshModel)

static Factory<ImportedMeshModel> Factory_ImportedMeshModel;

ImportedMeshModel::ImportedMeshModel()
  : MeshModelImplementation()
  , DataImport()
{
}


/* Constructor with parameters */
ImportedMeshModel::ImportedMeshModel(const String& fileName, const Indices& columns)
  : MeshModelImplementation()
  , DataImport(fileName, columns)
{
  update();
}


/* Constructor with parameters */
ImportedMeshModel::ImportedMeshModel(const VariableCollection& parameters, const String& fileName, const Indices& columns)
  : MeshModelImplementation()
  , DataImport(fileName, columns)
{
  setIndexParameters(parameters);
  update();
}


/* Virtual constructor */
ImportedMeshModel* ImportedMeshModel::clone() const
{
  return new ImportedMeshModel(*this);
}


void ImportedMeshModel::setDefaultColumns()
{
  setColumns(Indices(1, 0));
}


void ImportedMeshModel::update()
{
  // build mesh
  Sample sample(getSampleFromFile().getMarginal(inputColumns_));
  sample.setDescription(Description(1, getIndexParameters()[0].getName()));

  Collection<Indices> simplices(sample.getSize() - 1, Indices(2));
  for (UnsignedInteger i = 0; i < simplices.getSize(); ++i)
  {
    simplices[i][0] = i;
    simplices[i][1] = i + 1;
  }
  mesh_ = Mesh(sample, IndicesCollection(simplices));
}


void ImportedMeshModel::setColumns(const Indices& inputColumns, const Indices& /*outputColumns*/)
{
  // check columns
  if (inputColumns.getSize() != getIndexParameters().getSize())
    throw InvalidArgumentException(HERE) << "The dimension of the list of the column numbers has to be equal to the dimension of the mesh " << getIndexParameters().getSize();
  if (getSampleFromFile().getSize() < 2)
    throw InvalidArgumentException(HERE) << "The mesh must contain at least two nodes";
  DataImport::setColumns(inputColumns);
}


Interval ImportedMeshModel::getBounds() const
{
  return Interval(mesh_.getVertices().getMin()[0], mesh_.getVertices().getMax()[0]);
}


Indices ImportedMeshModel::getNumberOfNodes() const
{
  return  Indices(1, mesh_.getVertices().getSize());
}


String ImportedMeshModel::getHtmlDescription() const
{
  OSS oss;
  oss << MeshModelImplementation::getHtmlDescription();
  oss << "<p>File path : " << fileName_ << "</p>";

  return oss;
}


String ImportedMeshModel::getPythonScript() const
{
  OSS oss;

  // variables
  Description paramNames;
  for (UnsignedInteger i = 0; i < getIndexParameters().getSize(); ++i)
  {
    oss << getIndexParameters()[i].getPythonScript();
    paramNames.add(getIndexParameters()[i].getName());
  }

  oss << getName() << " = persalys.ImportedMeshModel(" << Parameters::GetOTDescriptionStr(paramNames, false) << ", '" << fileName_ << "', " << inputColumns_.__str__() << ")\n";

  return oss;
}


/* String converter */
String ImportedMeshModel::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " file name=" << getFileName()
      << " columns=" << getInputColumns()
      << " index parameters=" << getIndexParameters()
      << " mesh=" << getMesh();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void ImportedMeshModel::save(Advocate & adv) const
{
  MeshModelImplementation::save(adv);
  DataImport::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void ImportedMeshModel::load(Advocate & adv)
{
  MeshModelImplementation::load(adv);
  DataImport::load(adv);
}
}
