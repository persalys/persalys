//                                               -*- C++ -*-
/**
 *  @brief Class to build mesh with a file
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

ImportedMeshModel::ImportedMeshModel(const String &filename, 
  const VariableCollection &parameters, const Indices &columns, 
  const Tools::DataOrder order)
  : MeshModelImplementation()
  , DataImport()
  , order_ (order)
{
  setFileName(filename, order);
  setIndexParameters(parameters);
  setParameterColumns(columns);
}

/* Virtual constructor */
ImportedMeshModel* ImportedMeshModel::clone() const
{
  return new ImportedMeshModel(*this);
}

Sample ImportedMeshModel::importSample(const String& fileName,
                                       const Tools::DataOrder order)
{
  Sample sampleFromFile{DataImport::importSample(fileName, order)};

  // check sampleFromFile size
  if (sampleFromFile.getSize() < 2)
    throw InvalidArgumentException(HERE) << "The mesh must contain at least two nodes";

  return sampleFromFile;
}


void ImportedMeshModel::setDefaultColumns()
{
  setColumns(Indices(1, 0), Indices());
}

void ImportedMeshModel::setParameterColumns(const Indices& inputColumns)
{
  VariableCollection indexParameters{getIndexParameters()};
  // check columns
  if (inputColumns.getSize() != indexParameters.getSize())
    throw InvalidArgumentException(HERE) 
    << "The dimension of the list of the column numbers has to be equal to the dimension of the mesh " 
    << indexParameters.getSize();

  DataImport::setColumns(inputColumns, Indices());

  // build mesh
  Sample sample(getSampleFromFile().getMarginal(inputColumns_));
  sample.setDescription(Description(1, indexParameters[0].getName()));

  Collection simplices(sample.getSize() - 1, Indices(2));
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


String ImportedMeshModel::getHTMLDescription() const
{
  OSS oss;
  oss << MeshModelImplementation::getHTMLDescription();
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

  oss << getName() << " = persalys.ImportedMeshModel('" << fileName_ << "', " << Parameters::GetOTDescriptionStr(paramNames, false) << ", " << inputColumns_.__str__();

  switch (order_)
  {
    case Tools::DataOrder::Rows:
      oss << ", persalys.Tools.Rows";
      break;
    
    case Tools::DataOrder::Columns:
      oss << ", persalys.Tools.Columns";
      break;
    
    default:
      break;
  }

  oss << ")\n";

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
  adv.saveAttribute("order_", static_cast<UnsignedInteger>(order_));
}


/* Method load() reloads the object from the StorageManager */
void ImportedMeshModel::load(Advocate & adv)
{
  MeshModelImplementation::load(adv);
  DataImport::load(adv);
  UnsignedInteger order;
  adv.loadAttribute("order_", order);
  order_ = static_cast<Tools::DataOrder>(order);
}
}
