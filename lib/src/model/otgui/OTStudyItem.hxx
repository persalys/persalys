//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a otStudy
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
#ifndef OTGUI_OTSTUDYITEM_HXX
#define OTGUI_OTSTUDYITEM_HXX

#include "otgui/PhysicalModelItem.hxx"
#include "otgui/ProbabilisticModelItem.hxx"
#include "otgui/DesignOfExperimentItem.hxx"
#include "otgui/LimitStateItem.hxx"
#include "otgui/AnalysisItem.hxx"
#include "otgui/OTStudy.hxx"

namespace OTGUI {
class OTGUI_API OTStudyItem : public QObject, public QStandardItem, public Observer
{
  Q_OBJECT

public:
  OTStudyItem(OTStudy * otStudy);

  void update(Observable * source, const OT::String & message);

  void addPhysicalModelItem(PhysicalModel & physicalModel);
  void addDesignOfExperimentItem(DesignOfExperiment & design);
  void addLimitStateItem(LimitState & limitState);
  void addAnalysisItem(Analysis & analysis);
  void addDeterministicAnalysisItem(Analysis & analysis, AnalysisItem * item);
  void addProbabilisticAnalysisItem(Analysis & analysis, AnalysisItem * item);
  void addReliabilityAnalysisItem(Analysis & analysis, AnalysisItem * item);

  void setData(const QVariant & value, int role);
  OTStudy * getOTStudy();

public slots:
  void updatePhysicalModel(const PhysicalModel & physicalModel);
  void updateAnalysis(const Analysis & analysis);
  void removeItem(QStandardItem*);
signals:
  void newPhysicalModelItemCreated(PhysicalModelItem*);
  void newProbabilisticModelItemCreated(ProbabilisticModelItem*);
  void newDesignOfExperimentItemCreated(DesignOfExperimentItem*);
  void newAnalysisItemCreated(AnalysisItem*);
  void newLimitStateItemCreated(LimitStateItem*);
  void itemRemoved(QStandardItem*);
  void otStudyRemoved(QStandardItem*);

private:
  OTStudy * otStudy_;
};
}
#endif