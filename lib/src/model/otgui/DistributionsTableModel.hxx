//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the distributions to test
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
#ifndef OTGUI_DISTRIBUTIONSTABLEMODEL_HXX
#define OTGUI_DISTRIBUTIONSTABLEMODEL_HXX

#include "otgui/OTGuiprivate.hxx"

#include <QAbstractTableModel>
#include <QWidget>

namespace OTGUI
{
class OTGUI_API DistributionsTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  DistributionsTableModel(const QStringList & distributions, QWidget* parent = 0);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

public slots:
  void appendDistribution(const QString & distributionName);

public slots:
signals:
  void distributionsListChanged(QStringList);

private:
  QStringList distributions_;
};
}
#endif
