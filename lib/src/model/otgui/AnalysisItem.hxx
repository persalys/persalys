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
#ifndef OTGUI_ANALYSISITEM_HXX
#define OTGUI_ANALYSISITEM_HXX

#include "otgui/Analysis.hxx"
#include "otgui/MetaModel.hxx"
#include "otgui/DesignOfExperiment.hxx"

#include <QStandardItem>

namespace OTGUI {
class OTGUI_API AnalysisItem : public QObject, public QStandardItem, public Observer
{
  Q_OBJECT

public:
  AnalysisItem(const Analysis & analysis, const OT::String & typeAnalysis="Analysis");

  void setData(const QVariant & value, int role);

  Analysis getAnalysis() const;

  virtual void update(Observable * source, const OT::String & message);

public slots:
  void updateAnalysis(const Analysis & analysis);
  void setDesignOfExperiment(const DesignOfExperiment & designOfExperiment);
signals:
  void analysisFinished(AnalysisItem*);
  void analysisRemoved(QStandardItem*);
  void analysisChanged(const Analysis&);
  void metaModelCreated(PhysicalModel&);

private:
  Analysis analysis_;
};
}
#endif