//                                               -*- C++ -*-
/**
 *  @brief Class to build mesh with a file
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_IMPORTEDMESHMODEL_HXX
#define OTGUI_IMPORTEDMESHMODEL_HXX

#include "MeshModelImplementation.hxx"

namespace OTGUI
{
class OTGUI_API ImportedMeshModel : public MeshModelImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  ImportedMeshModel();
  /** Constructor with parameters */
  ImportedMeshModel(const OT::String& fileName, const OT::Indices& columns=OT::Indices(1, 0));
  ImportedMeshModel(const VariableCollection& parameters, const OT::String& fileName, const OT::Indices& columns=OT::Indices(1, 0));

  /** Virtual constructor */
  virtual ImportedMeshModel * clone() const;

  void setParameters(const OT::String& fileName, const OT::Indices& columns=OT::Indices(1, 0));
  OT::Interval getBounds() const;
  OT::Indices getNumberOfNodes() const;
  OT::String getFileName() const;
  OT::Indices getColumns() const;

  virtual OT::String getHtmlDescription() const;
  virtual OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::String fileName_;
  OT::Indices columns_;
};
}
#endif
