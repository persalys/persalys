//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a physical model
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
#ifndef PERSALYS_PHYSICALMODELITEM_HXX
#define PERSALYS_PHYSICALMODELITEM_HXX

#include "persalys/PhysicalModel.hxx"
#include "persalys/Item.hxx"

namespace PERSALYS
{
class PERSALYS_MODEL_API PhysicalModelItem : public Item, public Observer
{
  Q_OBJECT

public:
  PhysicalModelItem(const PhysicalModel & physicalModel, const OT::String observerType);

  PhysicalModel getPhysicalModel() const;

signals:
  void numberInputsChanged();
  void numberOutputsChanged();
  void inputListDefinitionChanged();
  void inputListDifferentiationChanged();
  void outputChanged();
  void codeChanged();
  void physicalModelRemoved(QStandardItem*);
  void parallelizationStatusChanged();
  void meshChanged();
  void stochasticInputListChanged();
  void inputListCorrelationChanged();
  void copulaChanged();

protected:
  PhysicalModel physicalModel_;
};
}
#endif
