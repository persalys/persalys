//                                               -*- C++ -*-
/**
 *  @brief QWizard to define an evaluation of the model
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
#ifndef PERSALYS_OPTIMIZATIONWIZARD_HXX
#define PERSALYS_OPTIMIZATIONWIZARD_HXX

#include "persalys/AnalysisWizard.hxx"
#include "persalys/OptimizationTableModel.hxx"
#include "persalys/OptimizationAnalysis.hxx"
#include "persalys/LogDoubleSpinBox.hxx"
#include "persalys/UIntSpinBox.hxx"
#include "persalys/OptimizationAlgoPage.hxx"
#include "persalys/ConstraintsPage.hxx"
#include "persalys/ResizableHeaderlessTableView.hxx"

#include <QGridLayout>
#include <QHeaderView>

namespace PERSALYS
{

class PERSALYS_VIEW_API OptimizationBoundsPage : public QWizardPage
{
  Q_OBJECT

  friend class TestOptimizationWizard;

public:
  OptimizationBoundsPage(const QString& subTitle, QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  bool validatePage();
  ResizableHeaderlessTableView * getTableView() const
  {
    return tableView_;
  };
  OptimizationTableModel * getTableModel() const
  {
    return tableModel_;
  };
protected slots:
  void updateTable();

private:
  ResizableHeaderlessTableView * tableView_;
  OptimizationTableModel * tableModel_;
  TemporaryLabel * errorMessageLabel_;

};


class PERSALYS_VIEW_API OptimizationStoppingCriteria : public QGridLayout
{
  Q_OBJECT

public:
  OptimizationStoppingCriteria(QWidget* parent = 0);
  template <class T>
  void initialize(const T& optimAlgo)
  {
    evaluationsSpinBox_->setValue(optimAlgo.getMaximumCallsNumber());
    absoluteErrSpinBox_->setValue(optimAlgo.getMaximumAbsoluteError());
    relativeErrSpinBox_->setValue(optimAlgo.getMaximumRelativeError());
    residualErrSpinBox_->setValue(optimAlgo.getMaximumResidualError());
    constraintErrSpinBox_->setValue(optimAlgo.getMaximumConstraintError());
  }
  template <class T>
  void updateAlgorithm(T& optimAlgo)
  {
    optimAlgo.setMaximumCallsNumber(evaluationsSpinBox_->value());
    optimAlgo.setMaximumRelativeError(relativeErrSpinBox_->value());
    optimAlgo.setMaximumResidualError(residualErrSpinBox_->value());
    optimAlgo.setMaximumConstraintError(constraintErrSpinBox_->value());
    optimAlgo.setMaximumAbsoluteError(absoluteErrSpinBox_->value());
  }
  UIntSpinBox * getEvaluationSpinBox() const
  {
    return evaluationsSpinBox_;
  }

private:
  UIntSpinBox * evaluationsSpinBox_;
  LogDoubleSpinBox * absoluteErrSpinBox_;
  LogDoubleSpinBox * relativeErrSpinBox_;
  LogDoubleSpinBox * residualErrSpinBox_;
  LogDoubleSpinBox * constraintErrSpinBox_;
};


class PERSALYS_VIEW_API OptimizationWizard : public AnalysisWizard
{
  Q_OBJECT

  friend class TestOptimizationWizard;

public:
  OptimizationWizard(const Analysis& analysis, QWidget* parent = 0);

  virtual int nextId() const;
  virtual Analysis getAnalysis() const;

protected:
  void buildInterface();
  void initialize(const Analysis& analysis);
  virtual void resizeEvent(QResizeEvent * event);

private:
  OptimizationAlgoPage * algoPage_;
  OptimizationBoundsPage * boundsPage_;
  ConstraintsPage * cstrPage_;
  QComboBox * pbTypeComboBox_;
  OptimizationStoppingCriteria * stoppingCriteriaLayout_;
};
}
#endif
