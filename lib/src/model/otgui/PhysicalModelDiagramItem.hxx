//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a physical model
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_PHYSICALMODELDIAGRAMITEM_HXX
#define OTGUI_PHYSICALMODELDIAGRAMITEM_HXX

#include "otgui/PhysicalModelDefinitionItem.hxx"
#include "otgui/MeshItem.hxx"
#include "otgui/ProbabilisticModelItem.hxx"
#include "otgui/LimitStateItem.hxx"
#include "otgui/DesignOfExperimentDefinitionItem.hxx"

namespace OTGUI
{
class OTGUI_API PhysicalModelDiagramItem : public PhysicalModelItem
{
  Q_OBJECT

public:
  PhysicalModelDiagramItem(const PhysicalModel & physicalModel);

  void setData(const QVariant & value, int role);

  virtual void update(Observable * source, const OT::String & message);

  void fill();
  void appendItem(Analysis& analysis);
  void appendItem(const LimitState& limitState);

public slots:
  void appendPhysicalModelItem();
  void appendProbabilisticModelItem();
  void requestDesignOfExperimentEvaluation();
  void requestMetaModelCreation();
  void requestReliabilityCreation();
  void requestLimitStateRemoval();
  void requestDesignOfExperimentRemoval(bool);
  void updateDesignEvaluationCounter(bool);
  void duplicatePhysicalModel();
  void removePhysicalModel();

signals:
  // signal for diagram
  void inputNumberValidityChanged(bool);
  void twoInputsValidityChanged(bool);
  void outputNumberValidityChanged(bool);
  void physicalModelValidityChanged(bool);
  void probabilisticModelValidityChanged(bool);
  void dependenceValidityChanged(bool);
  void limitStateNumberValidityChanged(bool);
  void doeNumberValidityChanged(bool);
  void doeEvaluationNumberValidityChanged(bool);

  // signals for StudyManager
  void evaluationModelRequested();
  void screeningRequested();
  void optimizationRequested();
  void probabilisticModelRequested();
  void centralTendencyRequested();
  void sensitivityRequested();
  void limitStateRequested();
  void designOfExperimentRequested();
  void designOfExperimentEvaluationRequested(const PhysicalModel&);

  void modelDefinitionWindowRequested(PhysicalModelDefinitionItem*);
  void meshWindowRequested(MeshItem*);
  void probabilisticModelItemCreated(ProbabilisticModelItem*);
  void doeAnalysisItemCreated(DesignOfExperimentDefinitionItem*);
  void analysisItemCreated(AnalysisItem*);
  void limitStateCreated(LimitStateItem*);

  void changeCurrentItemRequested(QModelIndex);

protected:
  void buildActions();

private:
  QAction * defineAction_;
  QAction * duplicateAction_;
  QAction * removeAction_;
  int limitStateCounter_;
  OT::Indices doeCounter_;
};
}
#endif
