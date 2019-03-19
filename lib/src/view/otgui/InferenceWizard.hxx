//                                               -*- C++ -*-
/**
 *  @brief QWizard for inference
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
#ifndef OTGUI_INFERENCEWIZARD_HXX
#define OTGUI_INFERENCEWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/InferenceAnalysis.hxx"
#include "otgui/VariablesSelectionTableModel.hxx"
#include "otgui/TemporaryLabel.hxx"
#include "otgui/DoubleSpinBox.hxx"
#include "otgui/CopyableTableView.hxx"
#include "otgui/ResizableStackedWidget.hxx"

#include <openturns/FittingTest.hxx>

#include <QMenu>

namespace OTGUI
{
class OTGUI_API InferenceWizard : public AnalysisWizard
{
  Q_OBJECT

public :
  InferenceWizard(const Analysis& analysis, QWidget* parent = 0);

  virtual Analysis getAnalysis() const;
  virtual bool validateCurrentPage();

protected:
  void initialize();
  void buildInterface();

public slots:
  void selectedVarChanged(QModelIndex, QModelIndex);
  void updateDistListForVar(QStringList);
  void applyCurrentDistToAll();
  void updateInterestVar(OT::Description, OT::String);
signals:
  void currentVarChanged(int);
  void currentVarChecked(bool);

private:
  OT::String currentVarName_;
  InferenceAnalysis inference_;
  OT::Description interestVar_;
  std::map<OT::String, OT::FittingTest::DistributionFactoryCollection> distFactoriesForEachInterestVar_;
  ResizableStackedWidget * stackWidget_;
  DoubleSpinBox * levelSpinbox_;
  TemporaryLabel * errorMessageLabel_;
  bool pageValidity_;
  VariablesSelectionTableModel * varTableModel_;
};

class VariablesTableView : public CopyableTableView
{
  Q_OBJECT

public:
  VariablesTableView(QWidget* parent = 0)
    : CopyableTableView(parent)
  {
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
  }

public slots:
  // show the context menu when right clicking
  void contextMenu(const QPoint & pos)
  {
    QMenu * contextMenu(new QMenu(this));
    //Use the same list of distributions for all the variables
    QAction * applyToAllAction = new QAction(tr("Apply the list of distributions to all variables"), this);
    applyToAllAction->setStatusTip(tr("Export the data"));
    connect(applyToAllAction, SIGNAL(triggered()), this, SIGNAL(applyToAllRequested()));
    contextMenu->addAction(applyToAllAction);
    contextMenu->popup(this->mapToGlobal(pos));
  }

signals:
  void applyToAllRequested();
};
}
#endif
