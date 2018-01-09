//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list dependencies
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
#ifndef OTGUI_DEPENDENCIESTABLEMODEL_HXX
#define OTGUI_DEPENDENCIESTABLEMODEL_HXX

#include "otgui/PhysicalModel.hxx"

#include <QAbstractTableModel>
#include <QMetaType> // mandatory to specify it to avoid windows compilation problem

Q_DECLARE_METATYPE(OT::ComposedCopula)

namespace OTGUI
{
class DependenciesTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  DependenciesTableModel(const PhysicalModel &physicalModel, QObject *parent = 0);

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index, int role) const;

public slots:
  void updateData();
  void removeLine(const QModelIndex &index);
  void addLine(const OT::Copula& copula);
signals:
  void dataUpdated(const OT::ComposedCopula &copula);

protected:
  void updateCopula();

private:
  PhysicalModel physicalModel_;
  OT::ComposedCopula copula_;
};
}
#endif
