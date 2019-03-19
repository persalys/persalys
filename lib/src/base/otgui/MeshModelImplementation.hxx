//                                               -*- C++ -*-
/**
 *  @brief Class to build mesh
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
#ifndef OTGUI_MESHMODELIMPLEMENTATION_HXX
#define OTGUI_MESHMODELIMPLEMENTATION_HXX

#include "Variable.hxx"

#include <openturns/Mesh.hxx>

namespace OTGUI
{
class OTGUI_API MeshModelImplementation : public OT::PersistentObject
{
  CLASSNAME

public:
  /** Default constructor */
  MeshModelImplementation();

  /** Virtual constructor */
  virtual MeshModelImplementation * clone() const;

  OT::Mesh getMesh() const;

  VariableCollection getIndexParameters() const;
  void setIndexParameters(const VariableCollection& parameters);

  virtual OT::Interval getBounds() const;
  virtual OT::Indices getNumberOfNodes() const;

  virtual OT::String getHtmlDescription() const;
  virtual OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  OT::Mesh mesh_;

private:
  OT::PersistentCollection<Variable> indexParameters_;
};
}
#endif
