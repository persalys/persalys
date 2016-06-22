//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a probabilistic model
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
#include "otgui/ProbabilisticModelItem.hxx"

using namespace OT;

namespace OTGUI {

ProbabilisticModelItem::ProbabilisticModelItem(const PhysicalModel & physicalModel)
  : QObject()
  , QStandardItem("Probabilistic Model")
  , Observer("ProbabilisticModel")
  , physicalModel_(physicalModel)
{
  setData("ProbabilisticModel", Qt::UserRole);
}


PhysicalModel ProbabilisticModelItem::getPhysicalModel() const
{
  return physicalModel_;
}


void ProbabilisticModelItem::update(Observable* source, const String & message)
{
  if (message == "inputDistributionChanged" || message == "modelInputsChanged")
  {
    emit inputChanged();
  }
  else if (message == "probabilisticModelRemoved")
  {
    emit probabilisticModelRemoved(this);
  }
}
}