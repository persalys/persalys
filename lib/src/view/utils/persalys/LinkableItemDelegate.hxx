//                                               -*- C++ -*-
/**
 *  @brief Delegate for checkable linkable item
 *
 *  Copyright 2015-2021 EDF-Phimeca
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


#ifndef LINKABLEITEMDELEGATE_HXX
#define LINKABLEITEMDELEGATE_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QStyledItemDelegate>

namespace PERSALYS {
  class PERSALYS_UTILS_API LinkableItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

  public:
    LinkableItemDelegate(const int firstRowWithButton, QObject *parent)
      : QStyledItemDelegate(parent)
      , firstRowWithButton_(firstRowWithButton) { }

  protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
  private:
    int firstRowWithButton_;};
}
#endif
