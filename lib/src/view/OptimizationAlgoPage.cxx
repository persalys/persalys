//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to define the method of sensitivity analysis
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
#include "persalys/OptimizationAlgoPage.hxx"

#include "persalys/OptimizationAnalysis.hxx"
#include "persalys/RadioButtonDelegate.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/DocumentationToolButton.hxx"

#include <openturns/NLopt.hxx>

#include <QVBoxLayout>
#include <QRadioButton>
#include <QScrollArea>
#include <QHeaderView>

using namespace OT;

namespace PERSALYS
{

OptimizationAlgoPage::OptimizationAlgoPage(QWidget* parent)
  : QWizardPage(parent)
  , solverNames_()
  , algoTableView_(0)
  , algoTableModel_(0)
  , outputsSelectionGroupBox_(0)
  , errorMessageLabel_(0)
  , pageLayout_(0)
{
  setTitle(tr("Optimization methods"));
  pageLayout_ = new QVBoxLayout(this);
}


void OptimizationAlgoPage::initialize(OptimizationAnalysis& analysis)
{
  QLayoutItem *child;
  while ((child = pageLayout_->takeAt(0)) != 0) {
    delete child->widget();
    delete child;
  }

  // output selection
  outputsSelectionGroupBox_ = new OutputsSelectionGroupBox(false, this);
  pageLayout_->addWidget(outputsSelectionGroupBox_);

  // button to open OT documentation
  DocumentationToolButton * infoButton = new DocumentationToolButton("user_manual/optimization.html", FileTools::docOT);
  pageLayout_->addWidget(infoButton);

  solverNames_ = OptimizationAnalysis::GetSolverNames(analysis.getBounds());
  algoTableView_ = new ResizableHeaderlessTableView;
  RadioButtonDelegate * delegate = new RadioButtonDelegate(1, algoTableView_);
  algoTableView_->setItemDelegateForColumn(0, delegate);
  algoTableView_->setSelectionMode(QAbstractItemView::NoSelection);
  algoTableView_->verticalHeader()->hide();
  algoTableView_->horizontalHeader()->hide();

  algoTableModel_ = new CustomStandardItemModel(solverNames_.getSize()+1, 1, algoTableView_);
  algoTableView_->setModel(algoTableModel_);
  algoTableModel_->setNotEditableHeaderItem(0, 0, tr("Available algorithms"));
  for (UnsignedInteger i = 0; i < solverNames_.getSize(); ++i) {
    algoTableModel_->setNotEditableItem(i + 1, 0, solverNames_[i].c_str());
    algoTableModel_->setData(algoTableModel_->index(i+1, 0), (int)i, Qt::UserRole);
  }

  pageLayout_->addWidget(algoTableView_);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  connect(outputsSelectionGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), errorMessageLabel_, SLOT(reset()));

  pageLayout_->addStretch();
  pageLayout_->addWidget(errorMessageLabel_);

  // method
  const String methodName = analysis.getSolverName();
  const Description::const_iterator it = std::find(solverNames_.begin(), solverNames_.end(), methodName);
  UnsignedInteger index = 0;
  //if algo is no longer compatible default to Cobyla
  if(it!=solverNames_.end())
    index = it - solverNames_.begin();
  else {
    LOGWARN(OSS() << "Optimization problem has changed and " << methodName << " algorithm is no longer available... Using Cobyla as default.");
    index = std::find(solverNames_.begin(), solverNames_.end(), "Cobyla") - solverNames_.begin();
  }
  algoTableView_->selectRow(index+1);
  algoTableModel_->setData(algoTableModel_->index(index+1, 0), true, Qt::CheckStateRole);

  // update outputs list
  PhysicalModel model = analysis.getPhysicalModel();
  outputsSelectionGroupBox_->updateComboBoxModel(model.getSelectedOutputsNames(), analysis.getInterestVariables());

  connect(algoTableView_, SIGNAL(clicked(QModelIndex)), this, SLOT(updateRadioButtonsAlgoTable(QModelIndex)));
}


Description OptimizationAlgoPage::getInterestVariables() const
{
  return QtOT::StringListToDescription(outputsSelectionGroupBox_->getSelectedOutputsNames());
}


String OptimizationAlgoPage::getSolverName() const
{
  // check
  if (!(algoTableModel_ && algoTableView_))
    throw InternalException(HERE) << "Error in OptimizationAlgoPage::getSolverName";

  QModelIndex selectedAlgoIndex;
  for (int i = 1; i < algoTableModel_->rowCount(); ++i)
    if (algoTableModel_->data(algoTableModel_->index(i, 0), Qt::CheckStateRole).toBool())
      selectedAlgoIndex = algoTableModel_->index(i, 0);

  if(selectedAlgoIndex.isValid())
    return solverNames_[algoTableModel_->data(selectedAlgoIndex, Qt::UserRole).toInt()];
  else
    return solverNames_[0];
}


bool OptimizationAlgoPage::validatePage()
{
  if (outputsSelectionGroupBox_->getSelectedOutputsNames().size() != 1)
  {
    errorMessageLabel_->setErrorMessage(tr("Only one output must be selected"));
    return false;
  }
  return QWizardPage::validatePage();
}

void OptimizationAlgoPage::updateRadioButtonsAlgoTable(QModelIndex current)
{
  // check
  if (!algoTableModel_ || current.row() < 1)
    return;

  // set CheckStateRole of algoTableModel_
  for (int i = 1; i < algoTableModel_->rowCount(); ++i)
  {
    if (algoTableModel_->index(i, 0).row() == current.row())
      algoTableModel_->setData(algoTableModel_->index(i, 0), true, Qt::CheckStateRole);
    else
      algoTableModel_->setData(algoTableModel_->index(i, 0), false, Qt::CheckStateRole);
  }
}

}
