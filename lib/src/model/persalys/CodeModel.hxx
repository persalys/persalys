//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to define Python script
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
#ifndef PERSALYS_CODEMODEL_HXX
#define PERSALYS_CODEMODEL_HXX

#include "persalys/PhysicalModel.hxx"

#include <QAbstractTableModel>

namespace PERSALYS
{
class PERSALYS_MODEL_API CodeModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  CodeModel(const PhysicalModel & physicalModel, QObject *parent = nullptr);

  int columnCount(const QModelIndex & parent = QModelIndex()) const override;
  int rowCount(const QModelIndex & parent = QModelIndex()) const override;
  QVariant data(const QModelIndex & index, int role) const override;
  bool setData(const QModelIndex & index, const QVariant & value, int role) override;
  Qt::ItemFlags flags(const QModelIndex & index) const override;

public slots:
  void updateData();
signals:
  void variablesChanged();
  void errorMessageChanged(const QString&);

private:
  PhysicalModel physicalModel_;
};
}
#endif
