//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of analysis
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
#include "otgui/AnalysisItem.hxx"

#include "otgui/FunctionalChaosAnalysis.hxx"
#include "otgui/KrigingAnalysis.hxx"

#include <QDebug>

using namespace OT;

namespace OTGUI {

AnalysisItem::AnalysisItem(const Analysis & analysis, const String & typeAnalysis)
  : QObject()
  , QStandardItem(QString::fromUtf8(analysis.getName().c_str()))
  , Observer("Analysis")
  , analysis_(analysis)
{
  analysis_.addObserver(this);
  setData(QString::fromUtf8(typeAnalysis.c_str()), Qt::UserRole);
}


void AnalysisItem::setData(const QVariant & value, int role)
{
  if (role == Qt::EditRole)
    analysis_.getImplementation()->setName(value.toString().toLocal8Bit().data());

  QStandardItem::setData(value, role);
}


Analysis AnalysisItem::getAnalysis() const
{
  return analysis_;
}


void AnalysisItem::updateAnalysis(const Analysis & analysis)
{
  analysis_ = analysis;
  if (!analysis_.getImplementation().get()->getObservers().size())
    analysis_.addObserver(this);
  setData(QString::fromUtf8(analysis.getImplementation()->getClassName().c_str()), Qt::UserRole);
  emit analysisChanged(analysis_);
}


void AnalysisItem::setDesignOfExperiment(const DesignOfExperiment& designOfExperiment)
{
  if (dynamic_cast<DesignOfExperimentAnalysis*>(&*getAnalysis().getImplementation()))
    dynamic_cast<DesignOfExperimentAnalysis*>(&*getAnalysis().getImplementation())->setDesignOfExperiment(designOfExperiment);
}


void AnalysisItem::stopAnalysis()
{
  analysis_.stop();
}


void AnalysisItem::update(Observable* source, const String& message)
{
  if (message == "analysisFinished")
  {
    emit analysisFinished(this);
  }
  else if (message == "analysisBadlyFinished")
  {
    emit analysisBadlyFinished(this, analysis_.getErrorMessage().c_str());
  }
  else if (message == "analysisRemoved")
  {
    emit analysisRemoved(this);
  }
  else if (message == "metaModelCreated")
  {
    if (dynamic_cast<FunctionalChaosAnalysis*>(&*getAnalysis().getImplementation()))
    {
      PhysicalModel metaModel(dynamic_cast<FunctionalChaosAnalysis*>(&*getAnalysis().getImplementation())->getResult().getMetaModel());
      emit metaModelCreated(metaModel);
    }
    else if (dynamic_cast<KrigingAnalysis*>(&*getAnalysis().getImplementation()))
    {
      PhysicalModel metaModel(dynamic_cast<KrigingAnalysis*>(&*getAnalysis().getImplementation())->getResult().getMetaModel());
      emit metaModelCreated(metaModel);
    }
    else
      qDebug() << "The metamodel analysis is not recognized.";
  }
  else if (message == "informationMessageUpdated")
  {
    emit messageChanged(analysis_.getInformationMessage().c_str());
  }
  else if (message == "progressValueChanged")
  {
    emit progressValueChanged(analysis_.getProgressValue());
  }
}
}