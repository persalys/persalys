//                                               -*- C++ -*-
/**
 *  @brief QRadioButton delegate for checkable QTableview
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
#ifndef PERSALYS_RADIOBUTTONDELEGATE_HXX
#define PERSALYS_RADIOBUTTONDELEGATE_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QStyledItemDelegate>
#include <QApplication>
#include <QPainter>
#include <QRadioButton>

namespace PERSALYS
{
class PERSALYS_UTILS_API RadioButtonDelegate : public QStyledItemDelegate
{
public:
  RadioButtonDelegate(const int firstRowWithButton, QObject *parent)
    : QStyledItemDelegate(parent)
    , firstRowWithButton_(firstRowWithButton)
  {
  }


  void paint(QPainter* painter,
             const QStyleOptionViewItem& option,
             const QModelIndex& index) const;

  virtual QSize sizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& index) const
  {
    const QSize buttonSize = QRadioButton(index.data(Qt::DisplayRole).toString()).sizeHint();
    const int buttonMargin = 3;
    const int epsilon = 15; // correction for Windows
    return buttonSize + QSize(buttonMargin + epsilon, 0);
  }

private:
  int firstRowWithButton_;
};
}
#endif
