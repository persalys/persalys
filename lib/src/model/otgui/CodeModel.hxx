//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to define Python script
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
#ifndef OTGUI_CODEMODEL_HXX
#define OTGUI_CODEMODEL_HXX

#include "otgui/PhysicalModel.hxx"

#include <QAbstractTableModel>

namespace OTGUI
{
class OTGUI_API CodeModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  CodeModel(const PhysicalModel & physicalModel, QObject* parent = 0);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;

signals:
  void variablesChanged();

private:
  PhysicalModel physicalModel_;
};
}
#endif