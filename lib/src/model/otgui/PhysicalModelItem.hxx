//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a physical model
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#ifndef OTGUI_PHYSICALMODELITEM_HXX
#define OTGUI_PHYSICALMODELITEM_HXX

#include "otgui/InputTableModel.hxx"
#include "otgui/OutputTableModel.hxx"
#include "otgui/PhysicalModel.hxx"

#include <QStandardItem>

namespace OTGUI {
class OTGUI_API PhysicalModelItem : public QObject, public QStandardItem, public Observer
{
  Q_OBJECT

public:
  PhysicalModelItem(const PhysicalModel & physicalModel);

  PhysicalModel getPhysicalModel() const;

  void setData(const QVariant & value, int role);

  virtual void update(Observable * source, const OT::String & message);

public slots:
signals:
  void inputChanged();
  void modelInputsChanged();
  void outputChanged();
  void codeChanged();
  void physicalModelRemoved(QStandardItem*);
  void parallelizeStatusChanged();
  void wantedMachineChanged();

private:
  PhysicalModel physicalModel_;
};
}
#endif