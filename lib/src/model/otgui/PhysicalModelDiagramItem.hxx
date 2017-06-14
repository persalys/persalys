//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a physical model
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
#ifndef OTGUI_PHYSICALMODELDIAGRAMITEM_HXX
#define OTGUI_PHYSICALMODELDIAGRAMITEM_HXX

#include "otgui/PhysicalModelDefinitionItem.hxx"
#include "otgui/ProbabilisticModelItem.hxx"
#include "otgui/LimitStateItem.hxx"
#include "otgui/DesignOfExperimentDefinitionItem.hxx"
#include <QMetaType> // mandatory to specify it to avoid windows compilation problem

Q_DECLARE_METATYPE(OTGUI::DesignOfExperimentDefinitionItem*)

namespace OTGUI {
class OTGUI_API PhysicalModelDiagramItem : public PhysicalModelItem
{
  Q_OBJECT

public:
  PhysicalModelDiagramItem(const PhysicalModel & physicalModel);

  void setData(const QVariant & value, int role);

  virtual void update(Observable * source, const OT::String & message);

  void fill();
  void appendAnalysisItem(Analysis& analysis);
  void appendLimitStateItem(const LimitState& limitState);
  void appendDesignOfExperimentItem(const DesignOfExperiment& designOfExperiment);

public slots:
  void appendPhysicalModelItem();
  void appendProbabilisticModelItem();
  void requestDesignOfExperimentEvaluation();
  void requestMetaModelCreation();
  void requestReliabilityCreation();
  void requestLimitStateRemoval();
  void requestDesignOfExperimentRemoval(bool);
  void incrementDesignEvaluationCounter();
  void removePhysicalModel();

signals:
  void inputNumberValidityChanged(bool);
  void outputNumberValidityChanged(bool);
  void physicalModelValidityChanged(bool);
  void probabilisticModelValidityChanged(bool);
  void dependencyValidityChanged(bool);
  void limitStateNumberValidityChanged(bool);
  void designOfExperimentNumberValidityChanged(bool);
  void designOfExperimentEvaluationNumberValidityChanged(bool);

  void evaluationModelRequested();
  void probabilisticModelRequested();
  void centralTendencyRequested();
  void sensitivityRequested();
  void limitStateRequested();
  void designOfExperimentRequested();
  void designOfExperimentEvaluationRequested(QList<QStandardItem*>);

  void modelDefinitionWindowRequested(PhysicalModelDefinitionItem*);
  void newProbabilisticModelItemCreated(ProbabilisticModelItem*);
  void newAnalysisItemCreated(AnalysisItem*);
  void newLimitStateCreated(LimitStateItem*);
  void newDesignOfExperimentCreated(DesignOfExperimentDefinitionItem*);

protected:
  void buildActions();

private:
  QAction * definePhysicalModel_;
  QAction * removePhysicalModel_;
  int limitStateCounter_;
  OT::Indices doeCounter_;
};
}
#endif