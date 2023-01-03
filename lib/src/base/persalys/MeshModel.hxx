//                                               -*- C++ -*-
/**
 *  @brief Class to build mesh
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_MESHMODEL_HXX
#define PERSALYS_MESHMODEL_HXX

#include "MeshModelImplementation.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API MeshModel : public OT::TypedInterfaceObject<MeshModelImplementation>
{
  CLASSNAME

public:
  typedef OT::Pointer<MeshModelImplementation> Implementation;

  /** Default constructor */
  MeshModel();
  /** Default constructor */
  MeshModel(const MeshModelImplementation & implementation);
  /** Constructor from implementation */
  MeshModel(const Implementation & p_implementation);
  /** Constructor from implementation pointer */
  MeshModel(MeshModelImplementation * p_implementation);

  OT::Mesh getMesh() const;

  VariableCollection getIndexParameters() const;
  void setIndexParameters(const VariableCollection& parameters);

  OT::Interval getBounds() const;
  OT::Indices getNumberOfNodes() const;

  virtual OT::String getHtmlDescription() const;
  virtual OT::String getPythonScript() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);
};
}
#endif
