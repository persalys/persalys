//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of an object
 *
 *  Copyright 2015-2016 EDF
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
#include "otgui/ObserverItem.hxx"

namespace OTGUI {

ObserverItem::ObserverItem(const QString & name, const QString & typeName)
  : QObject()
  , Item(name, typeName)
  , Observer()
{
}


ObserverItem::~ObserverItem()
{

}

//method to search good name for the item in accordance with its type


QVariant ObserverItem::data(int role) const
{
  switch (role)
  {
    case Qt::EditRole:
      return Item::data(Qt::DisplayRole);

    default:
      return Item::data(role);
  }
}


void ObserverItem::setData(const QVariant & value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      QStandardItem::setData(value.toString(), role);
      emitDataChanged();
      break;
  }
}
}