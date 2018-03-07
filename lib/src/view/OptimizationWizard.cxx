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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/OptimizationWizard.hxx"

#include "otgui/DoubleSpinBox.hxx"
#include "otgui/OptimizationAnalysis.hxx"

#include <QHeaderView>
#include <QScrollBar>

#include <limits>

using namespace OT;

namespace OTGUI
{

OptimizationWizard::OptimizationWizard(const Analysis& analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
{
  buildInterface();
}


void OptimizationWizard::buildInterface()
{
  setWindowTitle(tr("Optimization"));

  // -- intro page
  introPage_ = new IntroOptimizationPage(this);
  introPage_->initialize(analysis_);
  setPage(0, introPage_);

  // -- second page

  // starting point - inputs bounds
  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * pageLayout = new QVBoxLayout(page);

  QGroupBox * inputsBox = new QGroupBox(tr("Inputs"));
  groupBoxLayout_ = new QVBoxLayout(inputsBox);

  tableView_ = new ResizableHeaderlessTableView;
  tableView_->horizontalHeader()->hide();

  OptimizationAnalysis optim = *dynamic_cast<OptimizationAnalysis*>(analysis_.getImplementation().get());
  tableModel_ = new OptimizationTableModel(optim);
  tableView_->setModel(tableModel_);

  // resize table
  tableView_->resizeWithOptimalWidth();
  if (tableView_->model()->rowCount() < 15) // if too many variables: no fixed height + use scrollbar
  {
    tableView_->resizeWithOptimalHeight();
    groupBoxLayout_->addWidget(tableView_, 0, Qt::AlignTop);
  }
  else
  {
    groupBoxLayout_->addWidget(tableView_);
  }

  pageLayout->addWidget(inputsBox);
  pageLayout->addStretch();

  errorMessageLabel_ = new TemporaryLabel;
  pageLayout->addWidget(errorMessageLabel_);
  connect(tableModel_, SIGNAL(errorMessageChanged(QString)), errorMessageLabel_, SLOT(setTemporaryErrorMessage(QString)));

  setPage(1, page);

  // -- third page

  // algo parameter
  page = new QWizardPage(this);
  pageLayout = new QVBoxLayout(page);

  QGroupBox * paramGroupBox = new QGroupBox(tr("Parameter"));
  QGridLayout * paramGroupBoxLayout = new QGridLayout(paramGroupBox);

  QLabel * param = new QLabel(tr("Problem type"));
  paramGroupBoxLayout->addWidget(param, 0, 0);
  pbTypeComboBox_ = new QComboBox;
  pbTypeComboBox_->addItems(QStringList() << tr("Minimization") << tr("Maximization"));
  paramGroupBoxLayout->addWidget(pbTypeComboBox_, 0, 1);

  pageLayout->addWidget(paramGroupBox);

  // Stop criteria
  QGroupBox * stopCriteriaGroupBox = new QGroupBox(tr("Stop criteria"));
  QGridLayout * stopCriteriaGroupBoxLayout = new QGridLayout(stopCriteriaGroupBox);

  // max number iterations
  QLabel * label = new QLabel(tr("Number of function evaluations"));
  stopCriteriaGroupBoxLayout->addWidget(label, 0, 0);
  evaluationSpinBox_ = new UIntSpinBox;
  label->setBuddy(evaluationSpinBox_);
  evaluationSpinBox_->setRange(1, 2e9);
  evaluationSpinBox_->setSingleStep(5);
  stopCriteriaGroupBoxLayout->addWidget(evaluationSpinBox_, 0, 1);

  // Absolute error
  label = new QLabel(tr("Absolute error"));
  stopCriteriaGroupBoxLayout->addWidget(label, 1, 0);
  absoluteErrSpinBox_ = new LogDoubleSpinBox;
  label->setBuddy(absoluteErrSpinBox_);
  absoluteErrSpinBox_->setRange(std::numeric_limits<double>::min(), 1.0);
  stopCriteriaGroupBoxLayout->addWidget(absoluteErrSpinBox_, 1, 1);

  // Relative error
  label = new QLabel(tr("Relative error"));
  stopCriteriaGroupBoxLayout->addWidget(label, 2, 0);
  relativeErrSpinBox_ = new LogDoubleSpinBox;
  label->setBuddy(relativeErrSpinBox_);
  relativeErrSpinBox_->setRange(std::numeric_limits<double>::min(), 1.0);
  stopCriteriaGroupBoxLayout->addWidget(relativeErrSpinBox_, 2, 1);

  // Residual error
  label = new QLabel(tr("Residual error"));
  stopCriteriaGroupBoxLayout->addWidget(label, 1, 2);
  residualErrSpinBox_ = new LogDoubleSpinBox;
  label->setBuddy(residualErrSpinBox_);
  residualErrSpinBox_->setRange(std::numeric_limits<double>::min(), 1.0);
  stopCriteriaGroupBoxLayout->addWidget(residualErrSpinBox_, 1, 3);

  // Constraint error
  label = new QLabel(tr("Constraint error"));
  stopCriteriaGroupBoxLayout->addWidget(label, 2, 2);
  constraintErrSpinBox_ = new LogDoubleSpinBox;
  label->setBuddy(constraintErrSpinBox_);
  constraintErrSpinBox_->setRange(std::numeric_limits<double>::min(), 1.0);
  stopCriteriaGroupBoxLayout->addWidget(constraintErrSpinBox_, 2, 3);

  pageLayout->addWidget(stopCriteriaGroupBox);

  setPage(2, page);

  initialize(analysis_);
  //
  setStartId(0);
}


void OptimizationWizard::initialize(const Analysis& analysis)
{
  const OptimizationAnalysis * analysis_ptr = dynamic_cast<const OptimizationAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  pbTypeComboBox_->setCurrentIndex(analysis_ptr->getMinimization() ? 0 : 1);
  evaluationSpinBox_->setValue(analysis_ptr->getMaximumEvaluationNumber());
  absoluteErrSpinBox_->setValue(analysis_ptr->getMaximumAbsoluteError());
  relativeErrSpinBox_->setValue(analysis_ptr->getMaximumRelativeError());
  residualErrSpinBox_->setValue(analysis_ptr->getMaximumResidualError());
  constraintErrSpinBox_->setValue(analysis_ptr->getMaximumConstraintError());
}


int OptimizationWizard::nextId() const
{
  switch (currentId())
  {
    case 0: // method
      return 1;
    case 1: // starting point - bounds
      return 2;
    default: // optim parameters
      return -1;
  }
}


Analysis OptimizationWizard::getAnalysis() const
{
  OptimizationAnalysis optim(analysis_.getName(), tableModel_->getAnalysis().getPhysicalModel());
  optim.setBounds(tableModel_->getAnalysis().getBounds());
  optim.setVariableInputs(tableModel_->getAnalysis().getVariableInputs());
  optim.setStartingPoint(tableModel_->getAnalysis().getStartingPoint());

  optim.setSolverName(introPage_->getSolverName());
  optim.setMinimization(pbTypeComboBox_->currentIndex() == 0 ? true : false);
  optim.setMaximumEvaluationNumber(evaluationSpinBox_->value());
  optim.setMaximumAbsoluteError(absoluteErrSpinBox_->value());
  optim.setMaximumRelativeError(relativeErrSpinBox_->value());
  optim.setMaximumResidualError(residualErrSpinBox_->value());
  optim.setMaximumConstraintError(constraintErrSpinBox_->value());

  optim.setInterestVariables(introPage_->getInterestVariables());

  return optim;
}


bool OptimizationWizard::validateCurrentPage()
{
  if (currentId() == 1)
  {
    Point variableInputsValues;
    Point lowerB;
    Point upperB;
    Interval::BoolCollection finiteLowerB;
    Interval::BoolCollection finiteUpperB;
    Interval bounds(tableModel_->getAnalysis().getBounds());
    for (UnsignedInteger i = 0; i < tableModel_->getAnalysis().getPhysicalModel().getInputDimension(); ++i)
    {
      if (tableModel_->getAnalysis().getVariableInputs().contains(tableModel_->getAnalysis().getPhysicalModel().getInputNames()[i]))
      {
        variableInputsValues.add(tableModel_->getAnalysis().getStartingPoint()[i]);
        lowerB.add(bounds.getLowerBound()[i]);
        upperB.add(bounds.getUpperBound()[i]);
        finiteLowerB.add(bounds.getFiniteLowerBound()[i]);
        finiteUpperB.add(bounds.getFiniteUpperBound()[i]);
      }
    }
    Interval varBounds(lowerB, upperB, finiteLowerB, finiteUpperB);
    if (varBounds.isEmpty())
    {
      errorMessageLabel_->setTemporaryErrorMessage(tr("The lower bounds must be lesser than the upper bounds"));
      return false;
    }
    if (!varBounds.contains(variableInputsValues))
    {
      errorMessageLabel_->setTemporaryErrorMessage(tr("The interval must contain the starting point"));
      return false;
    }
  }
  return true;
}


void OptimizationWizard::resizeEvent(QResizeEvent* event)
{
  QWizard::resizeEvent(event);

  if (event->oldSize().width() > 0 && tableView_ && tableModel_)
  {
    // tableView_->resizeWithOptimalWidth() : fixes the width -> the table is not resizable
    // when resizing the wizard, we want to resize the table too
    tableView_->horizontalHeader()->setStretchLastSection(true);
    tableView_->setFixedWidth(QWIDGETSIZE_MAX); // remove constraints
  }
}
}
