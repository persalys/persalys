//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of data models
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/DataModelItem.hxx"

using namespace OT;

namespace OTGUI {

DataModelItem::DataModelItem(const DataModel & dataModel)
  : QObject()
  , QStandardItem(QString::fromUtf8(dataModel.getName().c_str()))
  , Observer("DataModel")
  , dataModel_(dataModel)
{
  setData("DataModel", Qt::UserRole);
}


void DataModelItem::setData(const QVariant & value, int role)
{
  if (role == Qt::EditRole)
    dataModel_.setName(value.toString().toLocal8Bit().data());

  QStandardItem::setData(value, role);
}


DataModel DataModelItem::getDataModel() const
{
  return dataModel_;
}


void DataModelItem::updateDataModel(const DataModel & dataModel)
{
  dataModel_ = dataModel;
  dataModel_.addObserver(this);
  emit dataModelChanged(dataModel_);
}


void DataModelItem::update(Observable* source, const String & message)
{
  if (message=="dataModelRemoved")
  {
    emit dataModelRemoved(this);
  }
}
}