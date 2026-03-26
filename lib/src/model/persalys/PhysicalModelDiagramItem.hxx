//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a physical model
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#ifndef PERSALYS_PHYSICALMODELDIAGRAMITEM_HXX
#define PERSALYS_PHYSICALMODELDIAGRAMITEM_HXX

#include "persalys/PhysicalModelDefinitionItem.hxx"

namespace PERSALYS
{
class PERSALYS_MODEL_API PhysicalModelDiagramItem : public PhysicalModelItem
{
  Q_OBJECT

  friend class PhysicalModelDefinitionItem;
  friend class ProbabilisticModelItem;
  friend class FieldModelDiagramWindow;
  friend class PhysicalModelDiagramWindow;

public:
  PhysicalModelDiagramItem(const PhysicalModel & physicalModel);

  void setData(const QVariant & value, int role) override;

  void update(Observable * source, const OT::String & message) override;

  void fill();
  void appendItem(const Analysis& analysis) override;
  void appendItem(const LimitState& limitState) override;
  void appendItem(const DesignOfExperiment& designOfExp) override;
  void updateDiagramBoxesValidity();

public slots:
  void appendPhysicalModelItem();
  void appendProbabilisticModelItem();
  void requestDesignOfExperimentEvaluation();
  void requestReliabilityCreation();
  void requestLimitStateRemoval();
  void requestDesignOfExperimentRemoval(bool);
  void requestObservationsRemoval();
  void requestCalibrationCreation();
  void updateDesignEvaluationCounter(bool);
  void duplicatePhysicalModel();
  void removePhysicalModel();
  void newLimitState();
  void newObservations();
  void requestOpenProperties();
  void requestDesignOfExperimentExport();

signals:
  // signal for diagram
  void inputNumberValidityChanged(bool, QString);
  void twoInputsValidityChanged(bool, QString);
  void outputNumberValidityChanged(bool, QString);
  void physicalModelValidityChanged(bool, QString);
  void probabilisticModelValidityChanged(bool, QString);
  void dependenceValidityChanged(bool, QString);
  void limitStateNumberValidityChanged(bool, QString);
  void doeNumberValidityChanged(bool, QString);
  void doeEvaluationNumberValidityChanged(bool, QString);
  void observationsNumberValidityChanged(bool, QString);

protected:
  void buildActions();

private:
  QAction   * defineAction_         = nullptr;
  QAction   * duplicateAction_      = nullptr;
  QAction   * removeAction_         = nullptr;
  QAction   * propertiesAction_     = nullptr;
  int         limitStateCounter_    = 0;
  int         observationsCounter_  = 0;
  OT::Indices doeCounter_;
};
}
#endif
