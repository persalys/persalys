//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a multi-objective optimization analysis
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

#ifndef PERSALYS_MULTIOBJECTIVEOPTIMIZATIONWIZARD_HXX
#define PERSALYS_MULTIOBJECTIVEOPTIMIZATIONWIZARD_HXX

#include "persalys/OptimizationWizard.hxx"
#include "persalys/MultiObjectiveOptimizationAnalysis.hxx"
#include "persalys/MultiObjectiveOptimizationTableModel.hxx"
#include "persalys/ObjectivesTableModel.hxx"
#include "persalys/ValueLineEdit.hxx"

namespace PERSALYS
{

class PERSALYS_VIEW_API MultiObjectiveOptimizationAlgoPage : public OptimizationAlgoPage
{
  Q_OBJECT
public:
  MultiObjectiveOptimizationAlgoPage(QWidget* parent = 0);

  bool validatePage() override;
  void buildInterface();

public slots:
  void initialize(MultiObjectiveOptimizationAnalysis& analysis);
protected slots:
  void updateRadioButtonsAlgoTable(QModelIndex);
  void openDoc(QModelIndex);
};

class PERSALYS_VIEW_API MultiObjectiveDefinitionPage: public QWizardPage
{
  Q_OBJECT
public:
  MultiObjectiveDefinitionPage(QWidget* parent = 0);
  OT::Description getMinimization() const;
  bool validatePage() override;
  ObjectivesTableModel * getTableModel() const
  {
    return objTableModel_;
  };

public slots:
  void update(MultiObjectiveOptimizationAnalysis& analysis);

signals:
  void objectivesDefined();

private:
  QTableView * objTableView_;
  ObjectivesTableModel * objTableModel_;
  TemporaryLabel * errorMessageLabel_;
};

class PERSALYS_VIEW_API MultiObjectiveOptimizationBoundsPage : public QWizardPage
{
  Q_OBJECT

  friend class TestOptimizationWizard;

public:
  MultiObjectiveOptimizationBoundsPage(QWidget* parent = 0);

  void initialize(const Analysis& analysis);
  bool validatePage();
  ResizableHeaderlessTableView * getTableView() const
  {
    return tableView_;
  };
  MultiObjectiveOptimizationTableModel * getTableModel() const
  {
    return tableModel_;
  };
protected slots:
  void updateTable();
private:
  ResizableHeaderlessTableView * tableView_;
  MultiObjectiveOptimizationTableModel * tableModel_;
  TemporaryLabel * errorMessageLabel_;

};

class PERSALYS_VIEW_API MultiObjectiveOptimizationParameters : public QGridLayout
{
  Q_OBJECT

public:
  MultiObjectiveOptimizationParameters(QWidget* parent = 0);
  template <class T>
  void initialize(const T& optimAlgo)
  {
    generationSpinBox_->setValue(optimAlgo.getGenerationNumber());
    popSizeSpinBox_->setValue(optimAlgo.getPopulationSize());
    seedLineEdit_->setValue(optimAlgo.getSeed());
    constraintErrSpinBox_->setValue(optimAlgo.getMaximumConstraintError());
  }
  template <class T>
  void updateAlgorithm(T& optimAlgo)
  {
    optimAlgo.setGenerationNumber(generationSpinBox_->value());
    optimAlgo.setPopulationSize(popSizeSpinBox_->value());
    optimAlgo.setSeed(seedLineEdit_->value());
    optimAlgo.setMaximumConstraintError(constraintErrSpinBox_->value());
  }
  UIntSpinBox * getGenerationSpinBox() const
  {
    return generationSpinBox_;
  }
  UIntSpinBox * getPopSizeSpinBox() const
  {
    return popSizeSpinBox_;
  }
  ValueLineEdit * getSeedLineEdit_() const
  {
    return seedLineEdit_;
  }

private:
  UIntSpinBox * generationSpinBox_;
  UIntSpinBox * popSizeSpinBox_;
  ValueLineEdit * seedLineEdit_;
  LogDoubleSpinBox * constraintErrSpinBox_;

};

class PERSALYS_VIEW_API MultiObjectiveOptimizationWizard : public AnalysisWizard
{
  Q_OBJECT

public:
  MultiObjectiveOptimizationWizard(const Analysis& analysis, QWidget* parent = 0);
  void buildInterface();
  int nextId() const override;
  Analysis getAnalysis() const override;

protected:
  void initialize(const Analysis& analysis);

private:
  MultiObjectiveOptimizationAlgoPage * algoPage_;
  MultiObjectiveDefinitionPage * objPage_;
  ConstraintsPage * cstrPage_;
  MultiObjectiveOptimizationBoundsPage * boundsPage_;
  MultiObjectiveOptimizationParameters * parametersLayout_;
};

}
#endif
