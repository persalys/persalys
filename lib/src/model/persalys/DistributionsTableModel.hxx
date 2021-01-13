//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the distributions to test
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_DISTRIBUTIONSTABLEMODEL_HXX
#define PERSALYS_DISTRIBUTIONSTABLEMODEL_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QAbstractTableModel>
#include <QWidget>

namespace PERSALYS
{
class PERSALYS_MODEL_API DistributionsTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  DistributionsTableModel(const QStringList &distributions, const QStringList &availableDistributions, const bool isCopulaList, QWidget *parent = 0);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  QStringList getDistributions() const;

public slots:
  void appendDistribution(const QString & distributionName);
  void updateData(const QStringList & distributions);
signals:
  void distributionsListChanged(QStringList);

private:
  bool isCopulaList_;
  QStringList distributions_;
  QStringList availableDistributions_;
};
}
#endif
