//                                               -*- C++ -*-
/**
 *  @brief QStandardItemModel to set not editable items and export data
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
#include "persalys/CustomStandardItemModel.hxx"

#include "persalys/AppliException.hxx"
#include "persalys/StudyTreeViewModel.hxx"

#include <QFile>

namespace PERSALYS
{

CustomStandardItemModel::CustomStandardItemModel(int nbRows, int nbColumns, QObject * parent)
  : QStandardItemModel(nbRows, nbColumns, parent)
{
}


void CustomStandardItemModel::setNotEditableItem(const int row, const int column, const QString text, const QColor textColor, const QColor bgColor)
{
  auto * item = new QStandardItem(text);
  if (textColor != QColor())
    item->setData(textColor, Qt::ForegroundRole);
  if (bgColor != QColor())
    item->setData(bgColor, Qt::BackgroundRole);
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


void CustomStandardItemModel::setIncludeHeadersInExport(const bool enable)
{
  includeHeadersInExport_ = enable;
}


QString CustomStandardItemModel::getFormattedText() const
{
  QString result;
  const int cols = columnCount();
  const int rows = rowCount();

  auto appendLine = [&result](const QStringList &cells)
  {
    result.append(cells.join(","));
    result.append('\n');
  };

  if (includeHeadersInExport_ && cols > 0)
  {
    QStringList headerCells;
    headerCells << QString();
    for (int j = 0; j < cols; ++j)
      headerCells << headerData(j, Qt::Horizontal).toString();
    appendLine(headerCells);
  }

  for (int i = 0; i < rows; ++i)
  {
    QStringList cells;
    if (includeHeadersInExport_)
      cells << headerData(i, Qt::Vertical).toString();

    for (int j = 0; j < cols; ++j)
    {
      const QVariant numericValue = data(index(i, j), Qt::UserRole + 10);
      if (numericValue.isNull())
        cells << data(index(i, j)).toString().simplified();
      else
        cells << QString::number(numericValue.toDouble(), 'g', StudyTreeViewModel::DefaultSignificantDigits);
    }
    appendLine(cells);
  }
  return result;
}
}
