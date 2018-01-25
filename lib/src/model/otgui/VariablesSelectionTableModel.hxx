//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list and select variables
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
#ifndef OTGUI_VARIABLESSELECTIONTABLEMODEL_HXX
#define OTGUI_VARIABLESSELECTIONTABLEMODEL_HXX

#include "otgui/OTGuiprivate.hxx"

#include <openturns/OTType.hxx>

#include <QAbstractTableModel>

namespace OTGUI
{
class OTGUI_API VariablesSelectionTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  VariablesSelectionTableModel(const OT::Description& variablesNames, const OT::Interval::BoolCollection& isVariablesChecked, QObject* parent = 0);

  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  Qt::ItemFlags flags(const QModelIndex& index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  QVariant data(const QModelIndex& index, int role) const;
  bool setData(const QModelIndex& index, const QVariant & value, int role);
  OT::Description getSelectedVariables() const;
  void updateData(const OT::Description& variablesNames, const OT::Interval::BoolCollection &isVariablesEnabled);

public slots:
signals:
  void selectionChanged(const OT::Description& variablesNames, const OT::String&);

private:
  OT::Description variablesNames_;
  OT::Interval::BoolCollection isVariableChecked_;
  OT::Interval::BoolCollection isVariableEnabled_;
};
}
#endif
