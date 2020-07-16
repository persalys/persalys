//                                               -*- C++ -*-
/**
 *  @brief Defines physical models thanks analytical formulas and mesh
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_PYTHONFIELDMODEL_HXX
#define PERSALYS_PYTHONFIELDMODEL_HXX

#include "PythonPhysicalModel.hxx"

#include <openturns/PointToFieldFunction.hxx>

namespace PERSALYS
{

class PERSALYS_BASE_API PythonFieldModel : public PythonPhysicalModel
{
  CLASSNAME

public:
  /** Default constructor */
  PythonFieldModel(const OT::String& name = "Unnamed");
  /** Constructor with parameters */
  PythonFieldModel(const OT::String& name,
                      const MeshModel& meshModel,
                      const InputCollection & inputs,
                      const OutputCollection & outputs,
                      const OT::String & code);

  /** Virtual constructor */
  virtual PythonFieldModel * clone() const;

  OT::String getPythonScript() const;

protected:
  virtual OT::PointToFieldFunction generatePointToFieldFunction(const OT::Description & outputNames) const;
};
}
#endif
