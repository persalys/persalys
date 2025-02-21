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
#ifndef PERSALYS_OPTIMIZATIONTABLEMODEL_HXX
#define PERSALYS_OPTIMIZATIONTABLEMODEL_HXX

#include "persalys/OptimizationAnalysis.hxx"

#include <QAbstractTableModel>

namespace PERSALYS
{
class PERSALYS_MODEL_API OptimizationTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  OptimizationTableModel(QObject *parent = nullptr);
  OptimizationTableModel(const OptimizationAnalysis & analysis, QObject *parent = nullptr);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  virtual bool setData(const QModelIndex & index, const QVariant & value, int role);
  virtual Qt::ItemFlags flags(const QModelIndex & index) const;
  OptimizationAnalysis getAnalysis() const;

public slots:
signals:
  void errorMessageChanged(QString);
  void variablesChanged();

protected:
  QStringList types_;
private:
  OptimizationAnalysis analysis_;
  bool firstColumnChecked_;
};
}
#endif
