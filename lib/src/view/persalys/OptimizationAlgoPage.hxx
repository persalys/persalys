//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the method of optimization
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_ALGOOPTIMIZATIONPAGE_HXX
#define PERSALYS_ALGOOPTIMIZATIONPAGE_HXX

#include "persalys/Analysis.hxx"
#include "persalys/OptimizationAnalysis.hxx"
#include "persalys/OutputsSelectionGroupBox.hxx"
#include "persalys/TemporaryLabel.hxx"
#include "persalys/CustomStandardItemModel.hxx"

#include <QWizardPage>
#include <QComboBox>
#include <QSortFilterProxyModel>
#include <QTableView>

namespace PERSALYS
{

class AlgoFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  AlgoFilterProxyModel(QObject *parent = nullptr);

  void setDerivativeFilter(const QList<int> & derivativeFilter);
  void setLocalityFilter(const QList<int> & localityFilter);

protected:
  virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
  QList<int> derivativeFilter_;
  QList<int> localityFilter_;
};


class PERSALYS_VIEW_API OptimizationAlgoPage : public QWizardPage
{
  Q_OBJECT

  friend class TestOptimizationWizard;

public:
  OptimizationAlgoPage(QWidget* parent = 0);
  OT::Description getInterestVariables() const;

  OT::String getSolverName() const;

  virtual bool validatePage();
  void buildInterface();

signals:
  void outputSelected();

public slots:
  void initialize(OptimizationAnalysis&);
protected slots:
  void updateRadioButtonsAlgoTable(QModelIndex);
  void updateFilters();
  void openDoc(QModelIndex);

protected:
  OT::Description solverNames_;
  QTableView * algoTableView_;
  CustomStandardItemModel * algoTableModel_;
  OutputsSelectionGroupBox * outputsSelectionGroupBox_;
  TemporaryLabel * errorMessageLabel_;

private:
  AlgoFilterProxyModel * proxyModel_;
  QComboBox * derivativeCombobox_;
  QComboBox * localityCombobox_;
};


}
#endif
