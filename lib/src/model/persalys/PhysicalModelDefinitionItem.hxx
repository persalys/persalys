//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a physical model
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
#ifndef PERSALYS_PHYSICALMODELDEFINITIONITEM_HXX
#define PERSALYS_PHYSICALMODELDEFINITIONITEM_HXX

#include "persalys/PhysicalModelItem.hxx"
#include "persalys/DesignOfExperiment.hxx"
#include "persalys/Analysis.hxx"

namespace PERSALYS
{
class PhysicalModelDiagramItem;
class PERSALYS_API PhysicalModelDefinitionItem : public PhysicalModelItem
{
  Q_OBJECT

public:
  PhysicalModelDefinitionItem(const PhysicalModel &physicalModel, const PhysicalModelDiagramItem *diagramItem = 0);

  virtual void update(Observable * source, const OT::String & message);
};
}
#endif
