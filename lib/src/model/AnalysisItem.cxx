//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of analysis
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
#include "otgui/AnalysisItem.hxx"

namespace OTGUI {

AnalysisItem::AnalysisItem(const Analysis & analysis, const QString & typeAnalysis)
  : ObserverItem(analysis.getName().c_str(), typeAnalysis)
  , analysis_(analysis)
{
}


AnalysisItem::~AnalysisItem()
{
}


void AnalysisItem::setData(const QVariant& value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      analysis_.setName(value.toString().toStdString());
      ObserverItem::setData(value, role);
      break;
  }
}


Analysis AnalysisItem::getAnalysis() const
{
  return analysis_;
}


void AnalysisItem::updateAnalysis(const Analysis & analysis)
{
  analysis_ = analysis;
  analysis_.addObserver(this);
  emit analysisChanged(analysis_);
}
}