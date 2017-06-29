//                                               -*- C++ -*-
/**
 *  @brief LaunchParametersVisitor defines launching parameters.
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LAUNCHPARAMETERSVISITOR_HXX
#define LAUNCHPARAMETERSVISITOR_HXX

namespace OTGUI {
#ifdef OTGUI_HAVE_YACS
class YACSPhysicalModel;
#endif

class LaunchParametersVisitor
{
  public:
    LaunchParametersVisitor(){}
    ~LaunchParametersVisitor(){}
#ifdef OTGUI_HAVE_YACS
    virtual void visitYACS(YACSPhysicalModel* model)=0;
#endif

};
}
#endif // LAUNCHPARAMETERSVISITOR_HXX
