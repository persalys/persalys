//                                               -*- C++ -*-
/**
 *  @brief QWizard to define an evaluation of the model
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
#include "persalys/OptimizationWizard.hxx"

#include "persalys/ComboBoxDelegate.hxx"
#include "persalys/SpinBoxDelegate.hxx"
#include "persalys/OptimizationAnalysis.hxx"
#include "persalys/QtTools.hxx"

#include <QHeaderView>
#include <QScrollBar>

#include <limits>

using namespace OT;

namespace PERSALYS
{

// - OptimizationBoundsPage ------------------------------------------------------------

OptimizationBoundsPage::OptimizationBoundsPage(const QString& subTitle, QWidget* parent)
  : QWizardPage(parent)
  , tableView_(0)
  , tableModel_(0)
  , errorMessageLabel_(0)
{
  setSubTitle(subTitle);

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  QGroupBox * inputsBox = new QGroupBox(tr("Inputs"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(inputsBox);
  tableView_ = new ResizableHeaderlessTableView;
  tableView_->setEditTriggers(QTableView::AllEditTriggers);
  groupBoxLayout->addWidget(tableView_);
  groupBoxLayout->setSizeConstraint(QLayout::SetMaximumSize);
  pageLayout->addWidget(inputsBox);

  errorMessageLabel_ = new TemporaryLabel;
  pageLayout->addWidget(errorMessageLabel_, 0, Qt::AlignBottom);
}


void OptimizationBoundsPage::initialize(const Analysis& analysis)
{
  const OptimizationAnalysis * analysis_ptr = dynamic_cast<const OptimizationAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  // fill table
  tableModel_ = new OptimizationTableModel(*analysis_ptr, this);
  connect(tableModel_, SIGNAL(errorMessageChanged(QString)), errorMessageLabel_, SLOT(setTemporaryErrorMessage(QString)));
  tableView_->setModel(tableModel_);

  // combobox delegate column 2
  ComboBoxDelegate * delegate = new ComboBoxDelegate(tableView_);
  tableView_->setItemDelegateForColumn(2, delegate);
  // spinbox delegate column 3
  SpinBoxDelegate * spinBoxDelegate = new SpinBoxDelegate(tableView_);
  spinBoxDelegate->setSpinBoxType(SpinBoxDelegate::doubleValue);
  tableView_->setItemDelegateForColumn(3, spinBoxDelegate);
  // spinbox delegate column 4 - 5
  spinBoxDelegate = new SpinBoxDelegate(true, tableView_);
  spinBoxDelegate->setSpinBoxType(SpinBoxDelegate::doubleValue);
  tableView_->setItemDelegateForColumn(4, spinBoxDelegate);
  tableView_->setItemDelegateForColumn(5, spinBoxDelegate);

  updateTable();

  // resize table
  tableView_->resizeRowToContents(0);
  tableView_->resizeWithOptimalWidth();
  if (tableView_->model()->rowCount() < RowNumberToScrollTable) // if too many variables: no fixed height + use scrollbar
  {
    tableView_->resizeWithOptimalHeight();
  }

  connect(tableModel_, SIGNAL(variablesChanged()), this, SLOT(updateTable()));
}

void OptimizationBoundsPage::updateTable()
{
  for (int row=1; row<tableModel_->rowCount(); ++row) {
    if(tableModel_->getAnalysis().getVariableInputs().contains(tableModel_->getAnalysis().getPhysicalModel().getInputNames()[row - 1]))
      tableView_->openPersistentEditor(tableModel_->index(row, 2));
    else
      tableView_->closePersistentEditor(tableModel_->index(row, 2));
  }
}

bool OptimizationBoundsPage::validatePage()
{
  Indices variablesIndices;
  Interval bounds(tableModel_->getAnalysis().getBounds());
  const Description inputNames(tableModel_->getAnalysis().getPhysicalModel().getInputNames());
  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++i)
  {
    if (tableModel_->getAnalysis().getVariableInputs().contains(inputNames[i]))
    {
      if (bounds.getMarginal(i).isEmpty())
      {
        errorMessageLabel_->setErrorMessage(tr("The lower bounds must be less than the upper bounds"));
        return false;
      }
      if (!bounds.getMarginal(i).contains(Point(1, tableModel_->getAnalysis().getStartingPoint()[i])))
      {
        errorMessageLabel_->setErrorMessage(tr("The interval must contain the starting point"));
        return false;
      }
      variablesIndices.add(i);
    }
  }
  // check
  if (!variablesIndices.getSize())
  {
    errorMessageLabel_->setErrorMessage(tr("At least one variable must vary"));
    return false;
  }


  OptimizationAnalysis dummyAnalysis = tableModel_->getAnalysis();
  dummyAnalysis.setBounds(getTableModel()->getAnalysis().getBounds());
  dummyAnalysis.setVariableInputs(getTableModel()->getAnalysis().getVariableInputs());
  dummyAnalysis.setStartingPoint(getTableModel()->getAnalysis().getStartingPoint());
  dummyAnalysis.setVariablesType(getTableModel()->getAnalysis().getVariablesType());
  //analysis_.updateParameters();
  emit currentAnalysisChanged(dummyAnalysis);
  return QWizardPage::validatePage();
}


// - OptimizationStoppingCriteria ------------------------------------------

OptimizationStoppingCriteria::OptimizationStoppingCriteria(QWidget* parent)
  : QGridLayout(parent)
{
  // max number evaluations
  QLabel * label = new QLabel(tr("Number of evaluations"));
  addWidget(label, 0, 0);
  evaluationsSpinBox_ = new UIntSpinBox;
  label->setBuddy(evaluationsSpinBox_);
  evaluationsSpinBox_->setRange(1, 2e9);
  evaluationsSpinBox_->setSingleStep(5);
  addWidget(evaluationsSpinBox_, 0, 1);

  // Absolute error
  label = new QLabel(tr("Absolute error"));
  addWidget(label, 1, 0);
  absoluteErrSpinBox_ = new LogDoubleSpinBox;
  label->setBuddy(absoluteErrSpinBox_);
  absoluteErrSpinBox_->setRange(std::numeric_limits<double>::min(), 1.0);
  addWidget(absoluteErrSpinBox_, 1, 1);

  // Relative error
  label = new QLabel(tr("Relative error"));
  addWidget(label, 2, 0);
  relativeErrSpinBox_ = new LogDoubleSpinBox;
  label->setBuddy(relativeErrSpinBox_);
  relativeErrSpinBox_->setRange(std::numeric_limits<double>::min(), 1.0);
  addWidget(relativeErrSpinBox_, 2, 1);

  // Residual error
  label = new QLabel(tr("Residual error"));
  addWidget(label, 1, 2);
  residualErrSpinBox_ = new LogDoubleSpinBox;
  label->setBuddy(residualErrSpinBox_);
  residualErrSpinBox_->setRange(std::numeric_limits<double>::min(), 1.0);
  addWidget(residualErrSpinBox_, 1, 3);

  // Constraint error
  label = new QLabel(tr("Constraint error"));
  addWidget(label, 2, 2);
  constraintErrSpinBox_ = new LogDoubleSpinBox;
  label->setBuddy(constraintErrSpinBox_);
  constraintErrSpinBox_->setRange(std::numeric_limits<double>::min(), 1.0);
  addWidget(constraintErrSpinBox_, 2, 3);
}


// - OptimizationWizard ----------------------------------------------------------

OptimizationWizard::OptimizationWizard(const Analysis& analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
{
  buildInterface();
}


void OptimizationWizard::buildInterface()
{
  setWindowTitle(tr("Optimization"));
  docLink_ = "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#optimizationwizard";

  // -- 1st page: bounds page

  // starting point - inputs bounds
  boundsPage_ = new OptimizationBoundsPage(tr("Choose the input variables to optimize. Define the starting point and the bounds of the optimization algorithm."), this);
  setPage(0, boundsPage_);

  // -- 2nd page: algo page
  algoPage_ = new OptimizationAlgoPage(this);
  algoPage_->buildInterface();
  setPage(1, algoPage_);


  // -- third page: Constraints
  cstrPage_ = new ConstraintsPage(this);
  setPage(2, cstrPage_);


  // algo parameter
  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * pageLayout = new QVBoxLayout(page);

  QGroupBox * paramGroupBox = new QGroupBox(tr("Parameter"));
  QGridLayout * paramGroupBoxLayout = new QGridLayout(paramGroupBox);

  QLabel * param = new QLabel(tr("Problem type"));
  paramGroupBoxLayout->addWidget(param, 0, 0);
  pbTypeComboBox_ = new QComboBox;
  pbTypeComboBox_->addItems(QStringList() << tr("Minimization") << tr("Maximization"));
  paramGroupBoxLayout->addWidget(pbTypeComboBox_, 0, 1);

  pageLayout->addWidget(paramGroupBox);

  // Stopping criteria
  QGroupBox * stopCriteriaGroupBox = new QGroupBox(tr("Stopping criteria"));
  stoppingCriteriaLayout_ = new OptimizationStoppingCriteria(stopCriteriaGroupBox);

  pageLayout->addWidget(stopCriteriaGroupBox);

  setPage(3, page);

  initialize(analysis_);
  //
  setStartId(0);
  connect(boundsPage_, SIGNAL(currentAnalysisChanged(OptimizationAnalysis&)), algoPage_, SLOT(initialize(OptimizationAnalysis&)));
}


void OptimizationWizard::initialize(const Analysis& analysis)
{
  OptimizationAnalysis * analysis_ptr = dynamic_cast<OptimizationAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
    return;

  boundsPage_->initialize(analysis_);
  //algoPage_->initialize(analysis_);
  pbTypeComboBox_->setCurrentIndex(analysis_ptr->getMinimization() ? 0 : 1);
  stoppingCriteriaLayout_->initialize(*analysis_ptr);
  connect(algoPage_, &OptimizationAlgoPage::outputSelected, [=](){
      cstrPage_->initialize(*analysis_ptr);});

}


int OptimizationWizard::nextId() const
{
  switch (currentId())
  {
    case 0: // starting point - type - bounds
      return 1;
    case 1: // method
      return 2;
    case 2: //constraints
      return 3;
    default: // optim parameters
      return -1;
  }
}


Analysis OptimizationWizard::getAnalysis() const
{
  OptimizationAnalysis optim(analysis_.getName(), boundsPage_->getTableModel()->getAnalysis().getPhysicalModel());
  optim.setBounds(boundsPage_->getTableModel()->getAnalysis().getBounds());
  optim.setVariableInputs(boundsPage_->getTableModel()->getAnalysis().getVariableInputs());
  optim.setStartingPoint(boundsPage_->getTableModel()->getAnalysis().getStartingPoint());
  optim.setVariablesType(boundsPage_->getTableModel()->getAnalysis().getVariablesType());
  optim.setSolverName(algoPage_->getSolverName());
  optim.setMinimization(pbTypeComboBox_->currentIndex() == 0 ? true : false);
  stoppingCriteriaLayout_->updateAlgorithm(optim);

  // Constraints
  optim.resetConstraints();
  foreach( QString str, cstrPage_->getTableModel()->getConstraints() ) {
    optim.addConstraint(str.toStdString());
  }


  optim.setInterestVariables(algoPage_->getInterestVariables());

  return optim;
}


void OptimizationWizard::resizeEvent(QResizeEvent* event)
{
  QWizard::resizeEvent(event);

  if (event->oldSize().width() > 0 && boundsPage_->getTableView() && boundsPage_->getTableModel())
  {
    // boundsPage_->getTableView()->resizeWithOptimalWidth() : fixes the width -> the table is not resizable
    // when resizing the wizard, we want to resize the table too
    boundsPage_->getTableView()->horizontalHeader()->setStretchLastSection(true);
    boundsPage_->getTableView()->setFixedWidth(QWIDGETSIZE_MAX); // remove constraints
  }
}
}
