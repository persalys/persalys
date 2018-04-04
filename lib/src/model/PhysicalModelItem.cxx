//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a physical model
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
#include "otgui/PhysicalModelItem.hxx"

using namespace OT;

namespace OTGUI
{

PhysicalModelItem::PhysicalModelItem(const PhysicalModel & physicalModel, const String observerType)
  : OTguiItem("Unnamed", "NoUserRole")
  , Observer(observerType)
  , physicalModel_(physicalModel)
{
  setData(observerType.c_str(), Qt::UserRole);
  physicalModel_.addObserver(this);
}


PhysicalModel PhysicalModelItem::getPhysicalModel() const
{
  return physicalModel_;
}
}