//                                               -*- C++ -*-
/**
 *  @brief QStandardItemModel to set not editable items and export data
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/CustomStandardItemModel.hxx"

#include "otgui/AppliException.hxx"
#include "otgui/StudyTreeViewModel.hxx"

#include <QFile>

namespace OTGUI
{

CustomStandardItemModel::CustomStandardItemModel(int nbRows, int nbColumns, QObject * parent)
  : QStandardItemModel(nbRows, nbColumns, parent)
{
}


void CustomStandardItemModel::setNotEditableItem(const int row, const int column, const QString text, const QColor color)
{
  QStandardItem * item = new QStandardItem(text);
  if (color != QColor())
    item->setData(color, Qt::ForegroundRole);
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  setItem(row, column, item);
}


void CustomStandardItemModel::setNotEditableItem(const int row, const int column, const double value, const int prec)
{
  QStandardItem * item = new QStandardItem(QString::number(value, 'g', prec));
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  item->setData(value, Qt::UserRole + 10);
  setItem(row, column, item);
}


void CustomStandardItemModel::setNotEditableHeaderItem(const int row, const int column, const QString text)
{
  QStandardItem * item = new QStandardItem(text);
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  item->setBackground(QBrush("#f2f1f0"));
  item->setTextAlignment(Qt::AlignCenter);
  setItem(row, column, item);
}


QString CustomStandardItemModel::getFormattedText() const
{
  QString result;

  for (int i = 0; i < rowCount(); ++i)
  {
    for (int j = 0; j < columnCount(); ++j)
    {
      if (data(index(i, j), Qt::UserRole + 10).isNull())
        result.append(data(index(i, j)).toString().simplified()); // simplified() = rm '\t', '\n', '\v', '\f', '\r', ' '
      else
        result.append(QString::number(data(index(i, j), Qt::UserRole + 10).toDouble(), 'g', StudyTreeViewModel::DefaultSignificantDigits));
      result.append(j < columnCount() - 1 ? ',' : '\n');
    }
  }
  return result;
}
}
