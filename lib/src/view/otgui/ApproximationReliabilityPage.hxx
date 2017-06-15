//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define approximation reliability analysis
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
#ifndef OTGUI_APPROXIMATIONRELIABILITYPAGE_HXX
#define OTGUI_APPROXIMATIONRELIABILITYPAGE_HXX

#include "otgui/LimitState.hxx"
#include "otgui/Analysis.hxx"
#include "otgui/LogDoubleSpinBox.hxx"
#include "otgui/UIntSpinBox.hxx"

#include <openturns/OptimizationAlgorithm.hxx>

#include <QWizardPage>
#include <QLineEdit>
#include <QButtonGroup>

namespace OTGUI {
class OTGUI_API ApproximationReliabilityPage : public QWizardPage
{
  Q_OBJECT

public:
  enum Method {FORM = 3};
  enum OptimAlgo {AbdoRackwitzAlgo, CobylaAlgo, SQPAlgo};

  ApproximationReliabilityPage(QWidget* parent=0);

  void initialize(const Analysis& analysis);
  OT::OptimizationAlgorithm getOptimizationAlgorithm() const;
  Analysis getAnalysis(const OT::String& name, const LimitState& limitState) const;

  virtual int nextId() const;
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
  UIntSpinBox * iterationsSpinBox_;
  LogDoubleSpinBox * absoluteErrSpinBox_;
  LogDoubleSpinBox * relativeErrSpinBox_;
  LogDoubleSpinBox * residualErrSpinBox_;
  LogDoubleSpinBox * constraintErrSpinBox_;
};
}
#endif