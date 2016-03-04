//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of analysis
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
#include "otgui/AnalysisItem.hxx"

using namespace OT;

namespace OTGUI {

AnalysisItem::AnalysisItem(const Analysis & analysis, const String & typeAnalysis)
  : QObject()
  , QStandardItem(QString::fromLocal8Bit(analysis.getName().c_str()))
  , Observer()
  , analysis_(analysis)
{
  setData(QString::fromLocal8Bit(typeAnalysis.c_str()), Qt::UserRole);
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
  analysis_.addObserver(this);
  setData(QString::fromLocal8Bit(analysis.getImplementation()->getClassName().c_str()), Qt::UserRole);
  emit analysisChanged(analysis_);
}


void AnalysisItem::update(Observable* source, const String & message)
{
  if (message=="analysisFinished")
  {
    emit analysisFinished(this);
  }
  else if (message=="analysisRemoved")
  {
    emit analysisRemoved(this);
  }
}
}