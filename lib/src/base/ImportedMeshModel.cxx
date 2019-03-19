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
#include "otgui/ImportedMeshModel.hxx"

#include "otgui/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(ImportedMeshModel)

static Factory<ImportedMeshModel> Factory_ImportedMeshModel;

ImportedMeshModel::ImportedMeshModel()
  : MeshModelImplementation()
  , fileName_("")
  , columns_(Indices(1, 0))
{
}


/* Constructor with parameters */
ImportedMeshModel::ImportedMeshModel(const String& fileName, const Indices& columns)
  : MeshModelImplementation()
  , fileName_("")
  , columns_()
{
  setParameters(fileName, columns);
}


/* Constructor with parameters */
ImportedMeshModel::ImportedMeshModel(const VariableCollection& parameters, const String& fileName, const Indices& columns)
  : MeshModelImplementation()
  , fileName_("")
  , columns_()
{
  setIndexParameters(parameters);
  setParameters(fileName, columns);
}


/* Virtual constructor */
ImportedMeshModel* ImportedMeshModel::clone() const
{
  return new ImportedMeshModel(*this);
}


void ImportedMeshModel::setParameters(const String& fileName, const Indices& columns)
{
  // check
  if (fileName.empty())
    throw InvalidArgumentException(HERE) << "The file name cannot be empty";

  // import sample
  Sample sampleFromFile(Tools::ImportSample(fileName));

  // check columns
  if (columns.getSize() != getIndexParameters().getSize())
    throw InvalidArgumentException(HERE) << "The dimension of the list of the column numbers has to be equal to the dimension of the mesh " << getIndexParameters().getSize();
  if (!columns.check(sampleFromFile.getDimension()))
    throw InvalidArgumentException(HERE) << "Values in the columns list are not compatible with the sample dimension contained in the file.";

  Sample sample(sampleFromFile.getMarginal(columns));
  sample.setDescription(Description(1, getIndexParameters()[0].getName()));
  if (sample.getSize() < 2)
    throw InvalidArgumentException(HERE) << "The mesh must contain at least two nodes";

  fileName_ = fileName;
  columns_ = columns;

  // build mesh
  Collection<Indices> simplices(sampleFromFile.getSize() - 1, Indices(2));
  for (UnsignedInteger i = 0; i < simplices.getSize(); ++i)
  {
    simplices[i][0] = i;
    simplices[i][1] = i + 1;
  }
  mesh_ = Mesh(sample, IndicesCollection(simplices));
}


Interval ImportedMeshModel::getBounds() const
{
  return Interval(mesh_.getVertices().getMin()[0], mesh_.getVertices().getMax()[0]);
}


Indices ImportedMeshModel::getNumberOfNodes() const
{
  return  Indices(1, mesh_.getVertices().getSize());
}


String ImportedMeshModel::getFileName() const
{
  return fileName_;
}


Indices ImportedMeshModel::getColumns() const
{
  return columns_;
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

  oss << getName() << " = otguibase.ImportedMeshModel(" << Parameters::GetOTDescriptionStr(paramNames, false) << ", '" << fileName_ << "', " << columns_.__str__() << ")\n";

  return oss;
}


/* String converter */
String ImportedMeshModel::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " file name=" << getFileName()
      << " columns=" << getColumns()
      << " index parameters=" << getIndexParameters()
      << " mesh=" << getMesh();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void ImportedMeshModel::save(Advocate & adv) const
{
  MeshModelImplementation::save(adv);
  adv.saveAttribute("fileName_", fileName_);
  adv.saveAttribute("columns_", columns_);
}


/* Method load() reloads the object from the StorageManager */
void ImportedMeshModel::load(Advocate & adv)
{
  MeshModelImplementation::load(adv);
  adv.loadAttribute("fileName_", fileName_);
  adv.loadAttribute("columns_", columns_);
}
}
