//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a probabilistic model
 *
 *  Copyright 2015-2016 EDF
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
#ifndef OTGUI_PROBABILISTICMODELITEM_HXX
#define OTGUI_PROBABILISTICMODELITEM_HXX

#include "otgui/InputTableModel.hxx"
#include "otgui/OutputTableModel.hxx"
#include "otgui/PhysicalModel.hxx"

#include <QStandardItem>

namespace OTGUI {
class ProbabilisticModelItem : public QObject, public QStandardItem, public Observer
{
  Q_OBJECT

public:
  ProbabilisticModelItem(const PhysicalModel & physicalModel);

  PhysicalModel getPhysicalModel() const;

  virtual void update(Observable * source, const OT::String & message);

public slots:
  void updatePhysicalModel(const PhysicalModel & physicalModel);
signals:
  void inputChanged();
  void physicalModelChanged(const PhysicalModel&);
  void physicalModelRemoved(QStandardItem*);

private:
  PhysicalModel physicalModel_;
};
}
#endif