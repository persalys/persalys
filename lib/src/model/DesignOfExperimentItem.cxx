//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of designs of experiments
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
#include "otgui/DesignOfExperimentItem.hxx"

using namespace OT;

namespace OTGUI {

DesignOfExperimentItem::DesignOfExperimentItem(const DesignOfExperiment & designOfExperiment)
  : QObject()
  , QStandardItem(QString::fromUtf8(designOfExperiment.getName().c_str()))
  , Observer(designOfExperiment.getImplementation()->getClassName())
  , designOfExperiment_(designOfExperiment)
{
  designOfExperiment_.addObserver(this);
  setData(designOfExperiment_.getImplementation()->getClassName().c_str(), Qt::UserRole);
}


void DesignOfExperimentItem::setData(const QVariant & value, int role)
{
  if (role == Qt::EditRole)
    designOfExperiment_.getImplementation()->setName(value.toString().toLocal8Bit().data());

  QStandardItem::setData(value, role);
}


DesignOfExperiment DesignOfExperimentItem::getDesignOfExperiment() const
{
  return designOfExperiment_;
}


void DesignOfExperimentItem::updateDesignOfExperiment(const DesignOfExperiment & designOfExperiment)
{
  designOfExperiment_ = designOfExperiment;
  designOfExperiment_.addObserver(this);
  emit designOfExperimentChanged(designOfExperiment_);
}


void DesignOfExperimentItem::update(Observable* source, const String & message)
{
  if (message == "analysisFinished")
  {
    emit analysisFinished();
  }
  if (message == "analysisBadlyFinished")
  {
    emit analysisBadlyFinished(designOfExperiment_.getErrorMessage().c_str());
  }
  else if (message == "designOfExperimentRemoved")
  {
    emit designOfExperimentRemoved(this);
  }
}
}