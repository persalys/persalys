//                                               -*- C++ -*-
/**
 *  @brief QWidget to define dependencies
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
#ifndef PERSALYS_DEPENDENCIESWIDGET_HXX
#define PERSALYS_DEPENDENCIESWIDGET_HXX

#include "persalys/ProbabilisticModelItem.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/VariablesSelectionTableModel.hxx"
#include "persalys/DependenciesTableModel.hxx"
#include "persalys/TemporaryLabel.hxx"
#include "persalys/Study.hxx"

#include <QTableView>

namespace PERSALYS
{
class PERSALYS_API DependenciesWidget : public QWidget
{
  Q_OBJECT

public:
  DependenciesWidget(ProbabilisticModelItem *item, QWidget *parent = 0);
  DependenciesWidget(const PhysicalModel& model, QWidget *parent = 0);

public slots:
  void updateWidgets();
  void updateCopulaWidget(const int index, const OT::Distribution &copula);
  void selectedItemChanged(const QModelIndex &current, const QModelIndex &previous);
  void removeCopula();
  void addCopula();
  void openWizardToChooseInferenceResult(const QModelIndex&);
signals:
  void removeTableLine(const QModelIndex &index);

protected:
  void buildInterface();
  void updateVariablesList();

private:
  bool failSoftMode_;
  Study study_;
  PhysicalModel physicalModel_;
  VariablesSelectionTableModel * varTableModel_;
  ResizableStackedWidget * rightSideOfSplitterStackedWidget_;
  ResizableStackedWidget * copulaStackedWidget_;
  QTableView * tableView_;
  DependenciesTableModel * tableModel_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
