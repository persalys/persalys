//                                               -*- C++ -*-
/**
 *  @brief LaunchParametersVisitor defines launching parameters.
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

#ifndef PERSALYS_LAUNCHPARAMETERSVISITOR_HXX
#define PERSALYS_LAUNCHPARAMETERSVISITOR_HXX

namespace PERSALYS
{
#ifdef PERSALYS_HAVE_YACS
class YACSPhysicalModel;
class YACSCouplingPhysicalModel;
#endif

class LaunchParametersVisitor
{
public:
  LaunchParametersVisitor() {}
  ~LaunchParametersVisitor() {}
#ifdef PERSALYS_HAVE_YACS
  virtual void visitYACS(YACSPhysicalModel* model) = 0;
  virtual void visitYACS(YACSCouplingPhysicalModel* model) = 0;
#endif

};
}
#endif // PERSALYS_LAUNCHPARAMETERSVISITOR_HXX
