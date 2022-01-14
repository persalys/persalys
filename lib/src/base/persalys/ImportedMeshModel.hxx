//                                               -*- C++ -*-
/**
 *  @brief Class to build mesh with a file
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#ifndef PERSALYS_IMPORTEDMESHMODEL_HXX
#define PERSALYS_IMPORTEDMESHMODEL_HXX

#include "MeshModelImplementation.hxx"
#include "DataImport.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API ImportedMeshModel : public MeshModelImplementation, public DataImport
{
  CLASSNAME

public:
  /** Default constructor */
  ImportedMeshModel();
  /** Constructor with parameters */
  ImportedMeshModel(const OT::String& fileName, const OT::Indices& columns=OT::Indices(1, 0));
  ImportedMeshModel(const VariableCollection& parameters, const OT::String& fileName, const OT::Indices& columns=OT::Indices(1, 0));

  /** Virtual constructor */
  ImportedMeshModel * clone() const override;

  void setParameterColumns(const OT::Indices &inputColumns);
  OT::Interval getBounds() const override;
  OT::Indices getNumberOfNodes() const override;

  OT::String getHtmlDescription() const override;
  OT::String getPythonScript() const override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  OT::Sample importSample(const OT::String& fileName) override;
  void setDefaultColumns() override;
};
}
#endif
