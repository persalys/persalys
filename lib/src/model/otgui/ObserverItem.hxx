//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of an object
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
#ifndef OTGUI_OBSERVERITEM_HXX
#define OTGUI_OBSERVERITEM_HXX

#include "otgui/Observer.hxx"
#include "otgui/Item.hxx"

#include <QStandardItem>

namespace OTGUI {
class OTGUI_API ObserverItem : public QObject, public Item, public Observer
{
  Q_OBJECT

public:
  ObserverItem(const QString & name, const QString & typeName);

  virtual ~ObserverItem();

  QVariant data(int role = Qt::EditRole) const;
  void setData(const QVariant & value, int role);
};
}
#endif