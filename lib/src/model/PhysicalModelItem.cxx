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
#include "otgui/PhysicalModelItem.hxx"
#include "otgui/PythonPhysicalModel.hxx"

using namespace OT;

namespace OTGUI {

PhysicalModelItem::PhysicalModelItem(const PhysicalModel & physicalModel)
  : QObject()
  , QStandardItem(QString::fromUtf8(physicalModel.getName().c_str()))
  , Observer("PhysicalModel")
  , physicalModel_(physicalModel)
{
  physicalModel_.addObserver(this);
  setData("PhysicalModel", Qt::UserRole);
}


void PhysicalModelItem::setData(const QVariant & value, int role)
{
  if (role == Qt::EditRole)
    physicalModel_.getImplementation()->setName(value.toString().toLocal8Bit().data());

  QStandardItem::setData(value, role);
}


PhysicalModel PhysicalModelItem::getPhysicalModel() const
{
  return physicalModel_;
}


void PhysicalModelItem::update(Observable* source, const String & message)
{
  if (message == "inputNameChanged" || message == "inputDescriptionChanged" ||
      message == "inputAdded" || message == "inputRemoved" || message == "inputValueChanged")
  {
    emit inputChanged();
  }
  else if (message == "outputNameChanged" || message == "outputAdded" ||
           message == "outputRemoved" || message == "outputValueChanged" ||
           message == "outputDescriptionChanged" || message == "modelInputsChanged")
  {
    emit outputChanged();
  }
  else if (message == "codeChanged")
  {
    emit codeChanged();
  }
  else if (message == "parallelizeStatusChanged")
  {
    emit parallelizeStatusChanged();
  }
  else if (message == "wantedMachineChanged")
  {
    emit wantedMachineChanged();
  }
  else if (message == "physicalModelRemoved")
  {
    emit physicalModelRemoved(this);
  }
}
}