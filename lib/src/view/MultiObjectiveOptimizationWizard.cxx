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

#include "persalys/MultiObjectiveOptimizationWizard.hxx"
#include "persalys/RadioButtonDelegate.hxx"
#include "persalys/LinkableItemDelegate.hxx"
#include "persalys/ComboBoxDelegate.hxx"
#include "persalys/SpinBoxDelegate.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/FileTools.hxx"
#include "persalys/DocumentationToolButton.hxx"

using namespace OT;

namespace PERSALYS
{
  MultiObjectiveOptimizationAlgoPage::MultiObjectiveOptimizationAlgoPage(QWidget* parent)
    : OptimizationAlgoPage(parent)
  {
  }

  void MultiObjectiveOptimizationAlgoPage::buildInterface()
  {
    setTitle(tr("Multi-objective optimization methods"));
    QVBoxLayout * pageLayout = new QVBoxLayout(this);

    // output selection
    outputsSelectionGroupBox_ = new OutputsSelectionGroupBox(true, this);
    outputsSelectionGroupBox_->setTitle(tr("Variables to optimize: "));
    pageLayout->addWidget(outputsSelectionGroupBox_);

    QGroupBox * groupBox = new QGroupBox(tr("Available algorithms"));
    QGridLayout * groupBoxLayout = new QGridLayout(groupBox);

    algoTableView_ = new QTableView;
    RadioButtonDelegate * delegate = new RadioButtonDelegate(0, algoTableView_);
    algoTableView_->setItemDelegateForColumn(0, delegate);
    algoTableView_->setSelectionMode(QAbstractItemView::NoSelection);
    algoTableView_->verticalHeader()->hide();
    groupBoxLayout->addWidget(algoTableView_, 0, 0);

    algoTableModel_ = new CustomStandardItemModel(solverNames_.getSize(), 3, algoTableView_);
    algoTableView_->setModel(algoTableModel_);
    pageLayout->addWidget(groupBox);

    DocumentationToolButton * docButton = new DocumentationToolButton("user_manual/_generated/openturns.Pagmo.html", FileTools::docOT);
    pageLayout->addWidget(docButton);

    // error message
    errorMessageLabel_ = new TemporaryLabel;
    connect(outputsSelectionGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), errorMessageLabel_, SLOT(reset()));
    pageLayout->addWidget(errorMessageLabel_);
  }

  void MultiObjectiveOptimizationAlgoPage::initialize(MultiObjectiveOptimizationAnalysis& analysis)
  {
    algoTableModel_->clear();
    algoTableModel_->setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));

    errorMessageLabel_->reset();
    solverNames_ = MultiObjectiveOptimizationAnalysis::GetSolverNames(analysis.getBounds(),analysis.getVariablesType(), analysis.getEqualityConstraints(), analysis.getInequalityConstraints());

    for (UnsignedInteger i = 0; i < solverNames_.getSize(); ++i) {
      algoTableModel_->setNotEditableItem(i, 0, solverNames_[i].c_str());
      algoTableModel_->setData(algoTableModel_->index(i, 0), (int)i, Qt::UserRole);
    }

    const String methodName = analysis.getSolverName();
    const Description::const_iterator it = std::find(solverNames_.begin(), solverNames_.end(), methodName);
    const UnsignedInteger index = it - solverNames_.begin();
    algoTableView_->selectRow(index);
    algoTableModel_->setData(algoTableModel_->index(index, 0), true, Qt::CheckStateRole);

    // update outputs list
    PhysicalModel model = analysis.getPhysicalModel();
    outputsSelectionGroupBox_->updateComboBoxModel(model.getSelectedOutputsNames(), analysis.getInterestVariables());

    connect(algoTableView_, SIGNAL(clicked(QModelIndex)), this, SLOT(updateRadioButtonsAlgoTable(QModelIndex)));
    connect(algoTableView_, SIGNAL(clicked(QModelIndex)), this, SLOT(openDoc(QModelIndex)));

  }


  bool MultiObjectiveOptimizationAlgoPage::validatePage()
  {
    if (outputsSelectionGroupBox_->getSelectedOutputsNames().size() < 2)
    {
      errorMessageLabel_->setErrorMessage(tr("At least 2 outputs must be selected"));
      return false;
    }
    if (getSolverName().empty())
      return false;
    if (!errorMessageLabel_->text().isEmpty())
      return false;
    emit outputSelected();
    return QWizardPage::validatePage();
  }

  void MultiObjectiveOptimizationAlgoPage::updateRadioButtonsAlgoTable(QModelIndex current)
  {
    // check
    if (!algoTableModel_ || current.row() < 0 || current.column() > 0)
      return;

    // set CheckStateRole of algoTableModel_
    for (int i = 0; i < algoTableModel_->rowCount(); ++i)
    {
      if (algoTableModel_->index(i, 0).row() == current.row())
        algoTableModel_->setData(algoTableModel_->index(i, 0), true, Qt::CheckStateRole);
      else
        algoTableModel_->setData(algoTableModel_->index(i, 0), false, Qt::CheckStateRole);
    }
  }

  void MultiObjectiveOptimizationAlgoPage::openDoc(QModelIndex current)
{
  // check
  if (!algoTableModel_ || current.row() < 0 || current.column() != 1)
    return;

  const QString urlLink = algoTableModel_->data(algoTableModel_->index(current.row(), 1), Qt::UserRole).toString();
  FileTools::OpenUrl(urlLink);
}

  MultiObjectiveDefinitionPage::MultiObjectiveDefinitionPage(QWidget* parent)
    : QWizardPage(parent)
  {
    setTitle(tr("Optimization objectives"));

    QVBoxLayout * pageLayout = new QVBoxLayout(this);
    QGroupBox * groupBox = new QGroupBox(tr("Objectives definition"));
    QGridLayout * groupBoxLayout = new QGridLayout(groupBox);

    objTableView_ = new QTableView;
    objTableView_->setSelectionMode(QAbstractItemView::NoSelection);
    objTableView_->horizontalHeader()->setStretchLastSection(true);
    groupBoxLayout->addWidget(objTableView_, 0, 0);

    objTableModel_ = new ObjectivesTableModel(this);
    objTableView_->setModel(objTableModel_);

    ComboBoxDelegate * delegate = new ComboBoxDelegate(objTableView_);
    objTableView_->setItemDelegateForColumn(1, delegate);

    groupBoxLayout->setRowStretch(1,1);
    pageLayout->addWidget(groupBox);
  }


  void MultiObjectiveDefinitionPage::update(MultiObjectiveOptimizationAnalysis& analysis)
  {
    objTableModel_->updateTable(analysis);
    for (int i = 0; i < objTableModel_->rowCount(); ++i)
      objTableView_->openPersistentEditor(objTableModel_->index(i, 1));
  }

  Description MultiObjectiveDefinitionPage::getMinimization() const
  {
    return objTableModel_->getMinimization();
  }

  bool MultiObjectiveDefinitionPage::validatePage()
  {
    emit objectivesDefined();
    return QWizardPage::validatePage();
  }
  MultiObjectiveOptimizationWizard::MultiObjectiveOptimizationWizard(const Analysis& analysis, QWidget* parent)
    : AnalysisWizard(analysis, parent)
  {
    buildInterface();
  }

  MultiObjectiveOptimizationBoundsPage::MultiObjectiveOptimizationBoundsPage(QWidget* parent)
    : QWizardPage(parent)
  {
    setSubTitle(tr("Choose the evolving variables. If a variable is not evolving, deselect it and define its value."));

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

  void MultiObjectiveOptimizationBoundsPage::initialize(const Analysis& analysis)
  {
    const MultiObjectiveOptimizationAnalysis * analysis_ptr = dynamic_cast<const MultiObjectiveOptimizationAnalysis*>(analysis.getImplementation().get());

    if (!analysis_ptr)
      return;

    // fill table
    tableModel_ = new MultiObjectiveOptimizationTableModel(*analysis_ptr, this);
    connect(tableModel_, SIGNAL(errorMessageChanged(QString)), errorMessageLabel_, SLOT(setTemporaryErrorMessage(QString)));
    tableView_->setModel(tableModel_);

    // combobox delegate column 2
    ComboBoxDelegate * delegate = new ComboBoxDelegate(tableView_);
    tableView_->setItemDelegateForColumn(2, delegate);

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

  void MultiObjectiveOptimizationBoundsPage::updateTable()
  {
    for (int row=1; row<tableModel_->rowCount(); ++row) {
      if(tableModel_->getAnalysis().getVariableInputs().contains(tableModel_->getAnalysis().getPhysicalModel().getInputNames()[row - 1]))
        tableView_->openPersistentEditor(tableModel_->index(row, 2));
      else
        tableView_->closePersistentEditor(tableModel_->index(row, 2));
    }
  }

  bool MultiObjectiveOptimizationBoundsPage::validatePage()
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
        variablesIndices.add(i);
      }
    }
    // check
    if (!variablesIndices.getSize())
    {
      errorMessageLabel_->setErrorMessage(tr("At least one variable must vary"));
      return false;
    }

    MultiObjectiveOptimizationAnalysis dummyAnalysis = tableModel_->getAnalysis();
    dummyAnalysis.setBounds(getTableModel()->getAnalysis().getBounds());
    dummyAnalysis.setVariableInputs(getTableModel()->getAnalysis().getVariableInputs());
    dummyAnalysis.setVariablesType(getTableModel()->getAnalysis().getVariablesType());
    //analysis_.updateParameters();
    emit currentAnalysisChanged();

    return QWizardPage::validatePage();
  }

  MultiObjectiveOptimizationParameters::MultiObjectiveOptimizationParameters(QWidget* parent)
    : QGridLayout(parent)
  {
    // Number of generations
    QLabel * label = new QLabel(tr("Number of generations"));
    addWidget(label, 0, 0);
    generationSpinBox_ = new UIntSpinBox;
    label->setBuddy(generationSpinBox_);
    generationSpinBox_->setRange(1, 2e9);
    generationSpinBox_->setSingleStep(1);
    addWidget(generationSpinBox_, 0, 1);

    // Pop size
    label = new QLabel(tr("Initial population size"));
    addWidget(label, 1, 0);
    popSizeSpinBox_ = new UIntSpinBox;
    label->setBuddy(popSizeSpinBox_);
    popSizeSpinBox_->setRange(1, 2e9);
    popSizeSpinBox_->setSingleStep(1);
    addWidget(popSizeSpinBox_, 1, 1);

    // Seed
    label = new QLabel(tr("Seed"));
    addWidget(label, 2, 0);
    seedLineEdit_ = new ValueLineEdit;
    label->setBuddy(seedLineEdit_);
    addWidget(seedLineEdit_, 2, 1);

    // Constraint error
    label = new QLabel(tr("Constraint error"));
    addWidget(label, 3, 0);
    constraintErrSpinBox_ = new LogDoubleSpinBox;
    label->setBuddy(constraintErrSpinBox_);
    constraintErrSpinBox_->setRange(std::numeric_limits<double>::min(), 1.0);
    addWidget(constraintErrSpinBox_, 3, 1);

    setColumnStretch(0,1);

  }

  void MultiObjectiveOptimizationWizard::buildInterface()
  {
    setWindowTitle(tr("Multi-objective optimization"));
    docLink_ = "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#optimizationwizard";

    // -- 1st page: Inputs
    boundsPage_ = new MultiObjectiveOptimizationBoundsPage(this);
    setPage(0, boundsPage_);

    // -- 2nd page: Constraints
    cstrPage_ = new ConstraintsPage(this);
    setPage(1, cstrPage_);

    // -- 3rd page: Outputs and algo selection
    algoPage_ = new MultiObjectiveOptimizationAlgoPage(this);
    algoPage_->buildInterface();
    setPage(2, algoPage_);

    // -- 4thd page: Objectives
    objPage_ = new MultiObjectiveDefinitionPage(this);
    setPage(3, objPage_);

    // -- 5th page: Parameters
    QWizardPage * page = new QWizardPage(this);
    QVBoxLayout * pageLayout = new QVBoxLayout(page);

    QGroupBox * paramGroupBox = new QGroupBox(tr("Parameters"));
    parametersLayout_ = new MultiObjectiveOptimizationParameters(paramGroupBox);
    pageLayout->addWidget(paramGroupBox);
    setPage(4, page);

    initialize(analysis_);
    setStartId(0);

  }

  void MultiObjectiveOptimizationWizard::initialize(const Analysis& analysis)
  {
    MultiObjectiveOptimizationAnalysis * analysis_ptr = dynamic_cast<MultiObjectiveOptimizationAnalysis*>(analysis.getImplementation().get());

    if (!analysis_ptr)
      return;

    // Set selected outputs as interest variables if only one is defined (default ctor)
    if (analysis_ptr->getInterestVariables().getSize() == 1)
      analysis_ptr->setInterestVariables(analysis_ptr->getPhysicalModel().getSelectedOutputsNames());

    boundsPage_->initialize(*analysis_ptr);
    parametersLayout_->initialize(*analysis_ptr);

    connect(boundsPage_, &MultiObjectiveOptimizationBoundsPage::currentAnalysisChanged, [=](){
        analysis_ptr->setVariablesType(boundsPage_->getTableModel()->getAnalysis().getVariablesType());
        cstrPage_->initialize(*analysis_ptr);});

    connect(cstrPage_, &ConstraintsPage::constraintsDefined, [=](){
        analysis_ptr->resetConstraints();
        foreach( QString str, cstrPage_->getTableModel()->getConstraints() ) {
          analysis_ptr->addConstraint(str.toStdString());
        }
        algoPage_->initialize(*analysis_ptr);});

    connect(algoPage_, &MultiObjectiveOptimizationAlgoPage::outputSelected, [=]() {
        analysis_ptr->setInterestVariables(algoPage_->getInterestVariables());
        objPage_->update(*analysis_ptr);});

  }

  int MultiObjectiveOptimizationWizard::nextId() const
  {
    switch (currentId())
    {
    case 0: // Bounds
      return 1;
    case 1: // Constraints
      return 2;
    case 2: // Algo
      return 3;
    case 3: // Objectives
      return 4;
    default:  // Params
      return -1;
  }
}

  Analysis MultiObjectiveOptimizationWizard::getAnalysis() const
  {
    MultiObjectiveOptimizationAnalysis optim(analysis_.getName(), boundsPage_->getTableModel()->getAnalysis().getPhysicalModel());

    // Algo
    optim.setInterestVariables(algoPage_->getInterestVariables());
    optim.setSolverName(algoPage_->getSolverName());

    // Objectives
    Description isMinimization;
    const Description varNames = optim.getInterestVariables();
    for (UnsignedInteger i=0; i<varNames.getSize(); ++i)
      if (objPage_->getMinimization().contains(varNames[i]))
        isMinimization.add(varNames[i]);
    optim.setMinimization(isMinimization);

    // Constraints
    optim.resetConstraints();
    foreach( QString str, cstrPage_->getTableModel()->getConstraints() ) {
      optim.addConstraint(str.toStdString());
    }

    //Bounds
    optim.setVariableInputs(boundsPage_->getTableModel()->getAnalysis().getVariableInputs());
    optim.setStartingPoint(boundsPage_->getTableModel()->getAnalysis().getStartingPoint());
    optim.setBounds(boundsPage_->getTableModel()->getAnalysis().getBounds());
    optim.setVariablesType(boundsPage_->getTableModel()->getAnalysis().getVariablesType());

    // Parameters
    parametersLayout_->updateAlgorithm(optim);
    return optim;
  }

}
