//                                               -*- C++ -*-
/**
 *  @brief Delegate for checkable linkable item
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


#include "persalys/LinkableItemDelegate.hxx"

#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QIcon>
#include <QSize>

namespace PERSALYS
{
void LinkableItemDelegate::paint(QPainter *painter,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
  if (index.row() < firstRowWithButton_)
  {
    QStyledItemDelegate::paint(painter, option, index);
    return;
  }

  QStyleOptionViewItem options = option;
  initStyleOption(&options, index);

  painter->save();

  QTextDocument doc;
  doc.setHtml(options.text);

  options.text = "";
  options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

  painter->translate(options.rect.left(), options.rect.top());
  QRect clip(0, 0, options.rect.width(), options.rect.height());
  doc.drawContents(painter, clip);

  painter->restore();
}


QSize LinkableItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QStyleOptionViewItem options = option;
  initStyleOption(&options, index);

  QTextDocument doc;
  doc.setHtml(options.text);
  doc.setTextWidth(options.rect.width());
  return QSize(doc.idealWidth(), doc.size().height());
}
}
