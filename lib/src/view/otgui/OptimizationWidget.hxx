//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure optimization algorithm
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
#ifndef OTGUI_OPTIMIZATIONWIDGET_HXX
#define OTGUI_OPTIMIZATIONWIDGET_HXX

#include "otgui/Analysis.hxx"
#include "otgui/DoubleSpinBox.hxx"

#include <openturns/OptimizationSolver.hxx>

#include <QLineEdit>
#include <QButtonGroup>

namespace OTGUI {
class OTGUI_API OptimizationWidget : public QWidget
{
  Q_OBJECT

public:
  enum OptimAlgo {AbdoRackwitzAlgo, CobylaAlgo, SQPAlgo};

  OptimizationWidget(QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  OT::OptimizationSolver getOptimizationAlgorithm() const;

protected:
  void buildInterface();
  void updateWidgets();
  void updatePointLineEdit();

public slots:
  void openPointDefinitionWizard();
  void updateAlgorithmType(int);
  void updateNumberIterations(double);
  void updateAbsoluteError(double);
  void updateRelativeError(double);
  void updateResidualError(double);
  void updateConstraintError(double);

private:
  OT::OptimizationSolver optimAlgo_;
  OT::Description inputNames_;
  QLineEdit * pointLineEdit_;
  QButtonGroup * algoChoice_;
  QDoubleSpinBox * iterationsSpinBox_;
  QDoubleSpinBox * absoluteErrSpinBox_;
  QDoubleSpinBox * relativeErrSpinBox_;
  QDoubleSpinBox * residualErrSpinBox_;
  QDoubleSpinBox * constraintErrSpinBox_;
};
}
#endif