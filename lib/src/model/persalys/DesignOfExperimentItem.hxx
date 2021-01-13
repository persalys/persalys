//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of designs of experiments
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
#ifndef PERSALYS_DESIGNOFEXPERIMENTITEM_HXX
#define PERSALYS_DESIGNOFEXPERIMENTITEM_HXX

#include "persalys/Item.hxx"
#include "persalys/DesignOfExperiment.hxx"

namespace PERSALYS
{
class PERSALYS_MODEL_API DesignOfExperimentItem : public Item, public Observer
{
  Q_OBJECT

public:
  DesignOfExperimentItem(const DesignOfExperiment& designOfExperiment, const OT::String observerType);

  DesignOfExperiment getDesignOfExperiment() const;

  void setData(const QVariant & value, int role);

signals:
  void variablesChanged();

protected:
  DesignOfExperiment designOfExperiment_;
};
}
#endif
