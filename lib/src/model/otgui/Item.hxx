//                                               -*- C++ -*-
/**
 *  @brief QStandardItem
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
#ifndef OTGUI_ITEM_HXX
#define OTGUI_ITEM_HXX

#include <QStandardItem>

namespace OTGUI {
class Item : public QStandardItem
{
public:
  Item(const QString & name, const QString & typeName);

  virtual ~Item();

  QVariant data(int role) const;

private:
  QVariant type_;
};
}
#endif