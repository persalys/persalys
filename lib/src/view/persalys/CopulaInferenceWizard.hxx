//                                               -*- C++ -*-
/**
 *  @brief QWizard for copula inference
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_COPULAINFERENCEWIZARD_HXX
#define PERSALYS_COPULAINFERENCEWIZARD_HXX

#include "persalys/AnalysisWizard.hxx"
#include "persalys/CopulaInferenceAnalysis.hxx"
#include "persalys/VariablesSelectionTableModel.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/TemporaryLabel.hxx"

#include <QTableView>

namespace PERSALYS
{
class PERSALYS_VIEW_API CopulaInferenceWizard : public AnalysisWizard
{
  Q_OBJECT

public :
  CopulaInferenceWizard(const Analysis& analysis, QWidget* parent = 0);

  virtual Analysis getAnalysis() const;
  virtual bool validateCurrentPage();

protected:
  void buildInterface();

public slots:
  void selectedItemChanged(const QModelIndex &, const QModelIndex &);
  void updateDistForVars(const OT::Description&, const QStringList&);
  void removeGroup();
  void defineGroup();
signals:
  void removeTableLine(const QModelIndex &index);

private:
  std::map<OT::Description, CopulaInferenceAnalysis::DistributionFactoryCollection> distForVars_;
  VariablesSelectionTableModel * varTableModel_;
  QTableView * tableView_;
  QStandardItemModel * tableModel_;
  ResizableStackedWidget * stackedWidget_;
  TemporaryLabel * errorMessageLabel_;
};
}
#endif
