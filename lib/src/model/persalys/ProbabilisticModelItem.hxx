//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a probabilistic model
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#ifndef PERSALYS_PROBABILISTICMODELITEM_HXX
#define PERSALYS_PROBABILISTICMODELITEM_HXX

#include "persalys/PhysicalModelItem.hxx"

namespace PERSALYS
{
class PhysicalModelDiagramItem;
class PERSALYS_MODEL_API ProbabilisticModelItem : public PhysicalModelItem
{
  Q_OBJECT

public:
  ProbabilisticModelItem(const PhysicalModel &physicalModel, const PhysicalModelDiagramItem *diagramItem = 0);

  virtual void update(Observable * source, const OT::String & message);
};
}
#endif
