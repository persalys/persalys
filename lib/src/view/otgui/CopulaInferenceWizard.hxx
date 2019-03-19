//                                               -*- C++ -*-
/**
 *  @brief QWizard for copula inference
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
#ifndef OTGUI_COPULAINFERENCEWIZARD_HXX
#define OTGUI_COPULAINFERENCEWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/CopulaInferenceAnalysis.hxx"
#include "otgui/VariablesSelectionTableModel.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/TemporaryLabel.hxx"

#include <QTableView>

namespace OTGUI
{
class OTGUI_API CopulaInferenceWizard : public AnalysisWizard
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
