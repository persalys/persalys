//                                               -*- C++ -*-
/**
 *  @brief QWizard for inference
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_INFERENCEWIZARD_HXX
#define PERSALYS_INFERENCEWIZARD_HXX

#include "persalys/AnalysisWizard.hxx"
#include "persalys/InferenceAnalysis.hxx"
#include "persalys/VariablesSelectionTableModel.hxx"
#include "persalys/TemporaryLabel.hxx"
#include "persalys/DoubleSpinBox.hxx"
#include "persalys/LogSpinBox.hxx"
#include "persalys/CopyableTableView.hxx"
#include "persalys/ResizableStackedWidget.hxx"

#include <openturns/FittingTest.hxx>

#include <QMenu>
#include <QCheckBox>
#include <QComboBox>


namespace PERSALYS
{
class PERSALYS_VIEW_API InferenceWizard : public AnalysisWizard
{
  Q_OBJECT

public :
  explicit InferenceWizard(const Analysis& analysis, QWidget* parent = nullptr);

  Analysis getAnalysis() const override;
  bool validateCurrentPage() override;

protected:
  void initialize();
  void buildInterface();

public slots:
  void selectedVarChanged(QModelIndex, QModelIndex);
  void updateDistListForVar(QStringList);
  void applyCurrentDistToAll();
  void updateInterestVar(const OT::Description&, const OT::String&);
  void addAllDistributionsToAllVariables();
signals:
  void currentVarChanged(int);
  void currentVarChecked(bool);

private:
  OT::String currentVarName_;
  InferenceAnalysis inference_;
  OT::Description interestVar_;
  std::map<OT::String, OT::FittingTest::DistributionFactoryCollection> distFactoriesForEachInterestVar_;
  ResizableStackedWidget * stackWidget_ = nullptr;
  QComboBox * testTypeComboBox_ = nullptr;
  DoubleSpinBox * levelSpinbox_ = nullptr;
  QCheckBox * paramICCheckBox_ = nullptr;
  DoubleSpinBox * icLevelSpinbox_ = nullptr;
  DoubleSpinBox * lillieforsPrecisionSpinbox_ = nullptr;
  LogSpinBox * lillieforsMinimumSamplingSizeSpinbox_ = nullptr;
  LogSpinBox * lillieforsMaximumSamplingSizeSpinbox_ = nullptr;
  TemporaryLabel * errorMessageLabel_ = nullptr;
  bool pageValidity_ = true;
  VariablesSelectionTableModel * varTableModel_ = nullptr;
};

class VariablesTableView : public CopyableTableView
{
  Q_OBJECT

public:
  explicit VariablesTableView(QWidget* parent = nullptr)
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
