//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a physical model
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
#include "otgui/PhysicalModelItem.hxx"
#include "otgui/PythonPhysicalModel.hxx"

using namespace OT;

namespace OTGUI {

PhysicalModelItem::PhysicalModelItem(const PhysicalModel & physicalModel)
  : ObserverItem(physicalModel.getName().c_str(), "PhysicalModel")
  , physicalModel_(physicalModel)
{
}


PhysicalModelItem::~PhysicalModelItem()
{
// delete all children (analysis) + physicalModel_
}


void PhysicalModelItem::setData(const QVariant & value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      physicalModel_.setName(value.toString().toStdString());
      ObserverItem::setData(value, role);
      break;
  }
}


PhysicalModel PhysicalModelItem::getPhysicalModel() const
{
  return physicalModel_;
}


void PhysicalModelItem::updatePhysicalModel(const PhysicalModel & physicalModel)
{
  physicalModel_ = physicalModel;
  physicalModel_.addObserver(this);
  emit physicalModelChanged(physicalModel_);
}


void PhysicalModelItem::update(Observable* source, const String & message)
{
  if (message == "inputDescriptionChanged" || message == "inputAdded" ||
      message == "inputRemoved" || message == "inputValueChanged")
  {
    emit inputChanged();
  }
  else if (message == "outputAdded" || message == "outputRemoved" ||
           message == "outputValueChanged" || message == "outputDescriptionChanged" ||
           message == "modelInputsChanged")
  {
    emit outputChanged();
  }
  else if (message == "codeChanged")
  {
    emit codeChanged();
  }
}
}