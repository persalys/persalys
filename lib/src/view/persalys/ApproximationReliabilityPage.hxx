//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define approximation reliability analysis
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#ifndef PERSALYS_APPROXIMATIONRELIABILITYPAGE_HXX
#define PERSALYS_APPROXIMATIONRELIABILITYPAGE_HXX

#include "persalys/LimitState.hxx"
#include "persalys/Analysis.hxx"
#include "persalys/OptimizationWizard.hxx"

#include <openturns/OptimizationAlgorithm.hxx>

#include <QWizardPage>
#include <QLineEdit>
#include <QButtonGroup>

namespace PERSALYS
{
class PERSALYS_VIEW_API ApproximationReliabilityPage : public QWizardPage
{
  Q_OBJECT

public:
  enum OptimAlgo {AbdoRackwitzAlgo, CobylaAlgo, SQPAlgo};

  ApproximationReliabilityPage(QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  OT::OptimizationAlgorithm getOptimizationAlgorithm() const;
  void updateAnalysis(const Analysis& analysis);

  virtual bool validatePage();

protected:
  void buildInterface();
  void updatePointLineEdit();

public slots:
  void openPointDefinitionWizard();

private:
  OT::Description inputNames_;
  OT::Point startingPoint_;
  QLineEdit * pointLineEdit_;
  QButtonGroup * algoChoice_;
  OptimizationStoppingCriteria * stoppingCriteriaLayout_;
};
}
#endif
