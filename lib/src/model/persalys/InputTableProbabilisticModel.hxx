//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the inputs
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_INPUTTABLEPROBABILISTICMODEL_HXX
#define PERSALYS_INPUTTABLEPROBABILISTICMODEL_HXX

#include "persalys/PhysicalModel.hxx"

#include <QAbstractTableModel>

namespace PERSALYS
{
class PERSALYS_MODEL_API InputTableProbabilisticModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  InputTableProbabilisticModel(const PhysicalModel & physicalModel, const bool failSoftMode, QObject * parent = 0);

  int columnCount(const QModelIndex & parent = QModelIndex()) const override;
  int rowCount(const QModelIndex & parent = QModelIndex()) const override;
  Qt::ItemFlags flags(const QModelIndex & index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QVariant data(const QModelIndex & index, int role) const override;
  bool setData(const QModelIndex & index, const QVariant & value, int role) override;

public slots:
  void updateData();
signals:
  void distributionChanged(const QModelIndex&);
  void distributionsChanged();
  void inferenceResultRequested(const QModelIndex&);

private:
  bool failSoftMode_;
  PhysicalModel physicalModel_;
  QStringList allDistributions_;
};
}
#endif
