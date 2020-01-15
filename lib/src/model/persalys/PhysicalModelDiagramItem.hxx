//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a physical model
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
class PERSALYS_API PhysicalModelDiagramItem : public PhysicalModelItem
{
  Q_OBJECT

  friend class PhysicalModelDefinitionItem;
  friend class ProbabilisticModelItem;
  friend class FieldModelDiagramWindow;
  friend class PhysicalModelDiagramWindow;

public:
  PhysicalModelDiagramItem(const PhysicalModel & physicalModel);

  void setData(const QVariant & value, int role);

  virtual void update(Observable * source, const OT::String & message);

  void fill();
  virtual void appendItem(const Analysis& analysis);
  virtual void appendItem(const LimitState& limitState);
  virtual void appendItem(const DesignOfExperiment& designOfExp);
  void updateDiagramBoxesValidity();

public slots:
  void appendPhysicalModelItem();
  void appendProbabilisticModelItem();
  void requestDesignOfExperimentEvaluation();
  void requestMetaModelCreation();
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
  void observationsNumberValidityChanged(bool);

protected:
  void buildActions();

private:
  QAction * defineAction_ = 0;
  QAction * duplicateAction_ = 0;
  QAction * removeAction_ = 0;
  int limitStateCounter_;
  int observationsCounter_;
  OT::Indices doeCounter_;
};
}
#endif
