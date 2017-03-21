//                                               -*- C++ -*-
/**
 *  @brief QStandardItemModel to set not editable items and export data
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
#include "otgui/CustomStandardItemModel.hxx"

#include "otgui/OTguiException.hxx"

#include <QFile>

namespace OTGUI {

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
  setNotEditableItem(row, column, QString::number(value, 'g', prec));
}


void CustomStandardItemModel::setNotEditableHeaderItem(const int row, const int column, const QString text)
{
  QStandardItem * item = new QStandardItem(text);
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  item->setBackground(QBrush("#f2f1f0"));
  item->setTextAlignment(Qt::AlignCenter);
  setItem(row, column, item);
}


void CustomStandardItemModel::exportData(const QString & fileName)
{
  QFile file(fileName);

  // check
  if (!file.open(QFile::WriteOnly))
    throw IOException(HERE) << tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()).toStdString();

  QString result;

  for(int i = 0; i < rowCount(); ++i)
  {
    for (int j = 0; j < columnCount()-1; ++j)
    {
      result.append(data(index(i, j)).toString());
      result.append('\t');
    }
    result.append(data(index(i, columnCount()-1)).toString());
    result.append('\n');
  }
  file.write(result.toStdString().c_str());
  file.close();
}
}