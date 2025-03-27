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
#ifndef PERSALYS_DATAFIELDMODELITEM_HXX
#define PERSALYS_DATAFIELDMODELITEM_HXX

#include "persalys/DataFieldModel.hxx"
#include "persalys/Item.hxx"

namespace PERSALYS
{
class PERSALYS_MODEL_API DataFieldModelItem : public Item, public Observer
{
  Q_OBJECT

public:
  DataFieldModelItem(const DataFieldModel & dataModel, const OT::String observerType);

  DataFieldModel getDataFieldModel() const;

signals:
  void outputChanged();
  void meshChanged();
  void meshOverwritten();

protected:
  DataFieldModel dataModel_;
};
}
#endif
