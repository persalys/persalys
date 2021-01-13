//                                               -*- C++ -*-
/**
 *  @brief Class to build regular mesh
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_GRIDMESHMODEL_HXX
#define PERSALYS_GRIDMESHMODEL_HXX

#include "MeshModelImplementation.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API GridMeshModel : public MeshModelImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  GridMeshModel();
  /** Constructor with parameters */
  GridMeshModel(const OT::Interval& bounds, const OT::Indices& nbNodes);
  GridMeshModel(const VariableCollection& parameters, const OT::Interval& bounds, const OT::Indices& nbNodes);

  /** Virtual constructor */
  GridMeshModel * clone() const override;

  void setParameters(const OT::Interval& bounds, const OT::Indices& nbNodes);
  OT::Interval getBounds() const override;
  OT::Indices getNumberOfNodes() const override;

  OT::String getPythonScript() const override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

private:
  OT::Interval bounds_;
  OT::Indices nbNodes_;
};
}
#endif
