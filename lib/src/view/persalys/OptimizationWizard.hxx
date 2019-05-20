//                                               -*- C++ -*-
/**
 *  @brief QWizard to define an evaluation of the model
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef PERSALYS_OPTIMIZATIONWIZARD_HXX
#define PERSALYS_OPTIMIZATIONWIZARD_HXX

#include "persalys/AnalysisWizard.hxx"
#include "persalys/OptimizationTableModel.hxx"
#include "persalys/LogDoubleSpinBox.hxx"
#include "persalys/UIntSpinBox.hxx"
#include "persalys/OptimizationIntroPage.hxx"
#include "persalys/ResizableHeaderlessTableView.hxx"

#include <QVBoxLayout>
#include <QHeaderView>

namespace PERSALYS
{
class PERSALYS_API OptimizationWizard : public AnalysisWizard
{
  Q_OBJECT

  friend class TestOptimizationWizard;

public:
  OptimizationWizard(const Analysis& analysis, QWidget* parent = 0);

  virtual int nextId() const;
  virtual Analysis getAnalysis() const;
  virtual bool validateCurrentPage();

protected:
  void buildInterface();
  void initialize(const Analysis& analysis);
  virtual void resizeEvent(QResizeEvent * event);

private:
  OptimizationIntroPage * introPage_;
  QVBoxLayout * groupBoxLayout_;
  ResizableHeaderlessTableView * tableView_;
  OptimizationTableModel * tableModel_;
  QComboBox * pbTypeComboBox_;
  UIntSpinBox * evaluationSpinBox_;
  LogDoubleSpinBox * absoluteErrSpinBox_;
  LogDoubleSpinBox * relativeErrSpinBox_;
  LogDoubleSpinBox * residualErrSpinBox_;
  LogDoubleSpinBox * constraintErrSpinBox_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
