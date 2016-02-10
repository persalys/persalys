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
#ifndef OTGUI_ANALYSISITEM_HXX
#define OTGUI_ANALYSISITEM_HXX

#include "otgui/ObserverItem.hxx"
#include "otgui/Analysis.hxx"

namespace OTGUI {
class AnalysisItem : public ObserverItem
{
  Q_OBJECT

public:
  AnalysisItem(const Analysis & analysis, const QString & typeAnalysis=QString("Analysis"));

  virtual ~AnalysisItem();

  void setData(const QVariant & value, int role);

  Analysis getAnalysis() const;

public slots:
  void updateAnalysis(const Analysis & analysis);
signals:
  void analysisFinished(AnalysisItem*);
  void analysisChanged(const Analysis&);

private:
  Analysis analysis_;
};
}
#endif