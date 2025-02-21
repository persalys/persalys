//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a limit state
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
#ifndef PERSALYS_DATAMESHITEM_HXX
#define PERSALYS_DATAMESHITEM_HXX

#include "persalys/DataFieldModelItem.hxx"

namespace PERSALYS
{
class PERSALYS_MODEL_API DataMeshItem : public DataFieldModelItem
{
  Q_OBJECT

public:
  DataMeshItem(const DataFieldModel& dataModel);

  virtual void update(Observable * source, const OT::String & message);

protected:
  void buildActions();
};
}
#endif
