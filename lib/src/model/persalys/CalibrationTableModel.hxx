//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list inputs
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
#ifndef PERSALYS_CALIBRATIONTABLEMODEL_HXX
#define PERSALYS_CALIBRATIONTABLEMODEL_HXX

#include "persalys/CalibrationAnalysis.hxx"

#include <QAbstractTableModel>
#include <QMetaType> // mandatory to specify it to avoid windows compilation problem

#include "persalys/QtTools.hxx" // for Q_DECLARE_METATYPE(PERSALYS::PhysicalModel)

namespace PERSALYS
{
class PERSALYS_MODEL_API CalibrationTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  CalibrationTableModel(QObject *parent = nullptr);

  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  OT::PointWithDescription getFixedValues() const;
  OT::PointWithDescription getCalibratedInputs() const;

public slots:
  void updateData(const PhysicalModel &model);
signals:
  void modelChanged(const PhysicalModel &model);

private:
  PhysicalModel fakeModel_;
};
}
#endif
