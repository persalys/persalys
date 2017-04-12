//                                               -*- C++ -*-
/**
 *  @brief QWidget to visualize the variables of the physical model
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
#include "otgui/PhysicalModelWindowWidget.hxx"

#include "otgui/SymbolicPhysicalModel.hxx"
#include "otgui/ModelEvaluation.hxx"
#include "otgui/LineEditWithQValidatorDelegate.hxx"
#include "otgui/CheckableHeaderView.hxx"
#include "otgui/SpinBoxDelegate.hxx"

#include <QHeaderView>
#include <QSplitter>
#include <QScrollBar>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>

using namespace OT;

namespace OTGUI {

PhysicalModelWindowWidget::PhysicalModelWindowWidget(PhysicalModelItem * item)
  : QTabWidget()
  , physicalModel_(item->getPhysicalModel())
  , inputTableView_(0)
  , inputTableModel_(0)
  , addInputLineButton_(0)
  , removeInputLineButton_(0)
  , differentiationTableView_(0)
  , differentiationTableModel_(0)
  , outputTableView_(0)
  , outputTableModel_(0)
  , addOutputLineButton_(0)
  , removeOutputLineButton_(0)
  , evaluateOutputsButton_(0)
{
  connect(item, SIGNAL(outputChanged()), this, SLOT(updateOutputTableModel()));
  connect(item, SIGNAL(inputChanged()), this, SLOT(updateInputTableModel()));
  connect(item, SIGNAL(inputChanged()), this, SLOT(updateDifferentiationTableModel()));
  connect(item, SIGNAL(modelInputsChanged()), this, SLOT(updateDifferentiationTableModel()));
  buildInterface();
}


void PhysicalModelWindowWidget::buildInterface()
{
  QWidget * tab = new QWidget;
  QVBoxLayout * vbox = new QVBoxLayout(tab);

  QSplitter * verticalSplitter = new QSplitter(Qt::Vertical);

  // Table Inputs
  QGroupBox * inputsBox = new QGroupBox(tr("Inputs"));
  QVBoxLayout * inputsLayout = new QVBoxLayout(inputsBox);

  inputTableView_ = new CopyableTableView;
  inputTableView_->setEditTriggers(QTableView::AllEditTriggers);
  LineEditWithQValidatorDelegate * delegate = new LineEditWithQValidatorDelegate;
  delegate->setParent(this);
  inputTableView_->setItemDelegateForColumn(0, delegate);
  inputsLayout->addWidget(inputTableView_);

  // buttons Add/Remove input
  if (physicalModel_.getImplementation()->getClassName() == "SymbolicPhysicalModel")
  {
    addInputLineButton_ = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"));
    addInputLineButton_->setToolTip(tr("Add an input"));
    connect(addInputLineButton_, SIGNAL(clicked(bool)), this, SLOT(addInputLine()));

    removeInputLineButton_ = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
    removeInputLineButton_->setToolTip(tr("Remove the selected input"));
    connect(removeInputLineButton_, SIGNAL(clicked(bool)), this, SLOT(removeInputLine()));

    QHBoxLayout * buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(addInputLineButton_);
    buttonsLayout->addWidget(removeInputLineButton_);
    inputsLayout->addLayout(buttonsLayout);
  }

  verticalSplitter->addWidget(inputsBox);
  verticalSplitter->setStretchFactor(0, 5);

  // Table Outputs
  QGroupBox * outputsBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayout = new QVBoxLayout(outputsBox);

  outputTableView_ = new CopyableTableView;
  outputTableView_->setEditTriggers(QTableView::AllEditTriggers);
  outputTableHeaderView_ = new CheckableHeaderView;
  outputTableView_->setHorizontalHeader(outputTableHeaderView_);
  delegate = new LineEditWithQValidatorDelegate(true);
  delegate->setParent(this);
  outputTableView_->setItemDelegateForColumn(0, delegate);
  outputsLayout->addWidget(outputTableView_);

  // buttons Add/Remove output
  QHBoxLayout * outputButtonsLayout = new QHBoxLayout;
  outputButtonsLayout->addStretch();

  if (physicalModel_.getImplementation()->getClassName() == "SymbolicPhysicalModel")
  {
    addOutputLineButton_ = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"));
    addOutputLineButton_->setToolTip(tr("Add an output"));
    connect(addOutputLineButton_, SIGNAL(clicked(bool)), this, SLOT(addOutputLine()));

    removeOutputLineButton_ = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
    removeOutputLineButton_->setToolTip(tr("Remove the selected output"));
    connect(removeOutputLineButton_, SIGNAL(clicked(bool)), this, SLOT(removeOutputLine()));

    outputButtonsLayout->addWidget(addOutputLineButton_);
    outputButtonsLayout->addWidget(removeOutputLineButton_);
  }

  // button Evaluate outputs
  evaluateOutputsButton_ = new QPushButton(QIcon(":/images/run-build.png"), tr("Evaluate"));
  evaluateOutputsButton_->setToolTip(tr("Evaluate the value of the outputs"));
  connect(evaluateOutputsButton_, SIGNAL(clicked(bool)), this, SLOT(evaluateOutputs()));
  outputButtonsLayout->addWidget(evaluateOutputsButton_);

  outputsLayout->addLayout(outputButtonsLayout);

  verticalSplitter->addWidget(outputsBox);
  verticalSplitter->setStretchFactor(1, 3);

  vbox->addWidget(verticalSplitter);

  updateInputTableModel();
  updateOutputTableModel();

  addTab(tab, tr("Definition"));

  // differentiation
  tab = new QWidget;
  vbox = new QVBoxLayout(tab);

  QLabel * label = new QLabel(tr("Finite difference step definition"));
  label->setStyleSheet("font: bold;");
  vbox->addWidget(label);

  differentiationTableView_ = new CopyableTableView;
  SpinBoxDelegate * spinBoxDelegate = new SpinBoxDelegate;
  spinBoxDelegate->setSpinBoxType(SpinBoxDelegate::differentiationStep);
  connect(spinBoxDelegate, SIGNAL(applyToAllRequested(double)), this, SLOT(applyDifferentiationStepToAllInputs(double)));
  differentiationTableView_->setItemDelegateForColumn(1, spinBoxDelegate);
  differentiationTableView_->setEditTriggers(QTableView::AllEditTriggers);
  vbox->addWidget(differentiationTableView_);

  updateDifferentiationTableModel();

  addTab(tab, tr("Differentiation"));
}


void PhysicalModelWindowWidget::applyDifferentiationStepToAllInputs(double value)
{
  if (differentiationTableModel_)
  {
    for (int i=0; i<differentiationTableModel_->rowCount(); ++i)
    {
      differentiationTableModel_->setData(differentiationTableModel_->index(i, 1), value, Qt::EditRole);
    }
  }
}


void PhysicalModelWindowWidget::resizeEvent(QResizeEvent* event)
{
  QTabWidget::resizeEvent(event);

  if (event->oldSize().width() > 0)
  {
    resizeInputTable();
    resizeOutputTable();
  }
}


void PhysicalModelWindowWidget::resizeInputTable()
{
  const int width = inputTableView_->horizontalHeader()->width();
  inputTableView_->horizontalHeader()->resizeSection(0, width*1/5);
  inputTableView_->horizontalHeader()->resizeSection(1, width*3/5);
}


void PhysicalModelWindowWidget::resizeOutputTable()
{
  const int width = outputTableView_->horizontalHeader()->width();
  if (physicalModel_.getImplementation()->getClassName() != "SymbolicPhysicalModel")
  {
    outputTableView_->setColumnHidden(2, true);
    outputTableView_->horizontalHeader()->resizeSection(0, outputTableHeaderView_->minimumSectionSize());
    outputTableView_->horizontalHeader()->resizeSection(1, width - 2*(outputTableHeaderView_->minimumSectionSize()));
  }
  else
  {
    outputTableView_->horizontalHeader()->resizeSection(0, outputTableHeaderView_->minimumSectionSize());
    outputTableView_->horizontalHeader()->resizeSection(1, width*3/10);
    outputTableView_->horizontalHeader()->resizeSection(2, width*4/10);
  }
}


void PhysicalModelWindowWidget::updateInputTableModel()
{
  if (inputTableModel_)
    delete inputTableModel_;
  inputTableModel_ = new InputTableModel(physicalModel_, inputTableView_);
  inputTableView_->setModel(inputTableModel_);
#if QT_VERSION >= 0x050000
  inputTableView_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
  inputTableView_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
  inputTableView_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
#else
  inputTableView_->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
  inputTableView_->horizontalHeader()->setResizeMode(1, QHeaderView::Interactive);
  inputTableView_->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
#endif
  resizeInputTable();
  connect(inputTableModel_, SIGNAL(errorMessageChanged(QString)), this, SIGNAL(errorMessageChanged(QString)));
}


void PhysicalModelWindowWidget::updateDifferentiationTableModel()
{
  if (differentiationTableModel_)
    delete differentiationTableModel_;
  differentiationTableModel_  = new DifferentiationTableModel(physicalModel_, differentiationTableView_);
  differentiationTableView_->setModel(differentiationTableModel_);
#if QT_VERSION >= 0x050000
  differentiationTableView_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
  differentiationTableView_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
#else
  differentiationTableView_->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
  differentiationTableView_->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
#endif
}


void PhysicalModelWindowWidget::updateOutputTableModel()
{
  if (outputTableModel_)
    delete outputTableModel_;
  outputTableModel_ = new OutputTableModel(physicalModel_, outputTableView_);
  // table view
  outputTableView_->setModel(outputTableModel_);
#if QT_VERSION >= 0x050000
  outputTableView_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
  outputTableView_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
  outputTableView_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
  outputTableView_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
#else
  outputTableView_->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
  outputTableView_->horizontalHeader()->setResizeMode(1, QHeaderView::Interactive);
  outputTableView_->horizontalHeader()->setResizeMode(2, QHeaderView::Interactive);
  outputTableView_->horizontalHeader()->setResizeMode(3, QHeaderView::Stretch); 
#endif
  resizeOutputTable();
  // table header view
  const UnsignedInteger nbOutputs = physicalModel_.getOutputNames().getSize();
  const bool allChecked = (nbOutputs && (nbOutputs == physicalModel_.getSelectedOutputsNames().getSize()));
  outputTableHeaderView_->setChecked(allChecked);
  connect(outputTableModel_, SIGNAL(errorMessageChanged(QString)), this, SIGNAL(errorMessageChanged(QString)));
  connect(outputTableModel_, SIGNAL(checked(bool)), outputTableHeaderView_, SLOT(setChecked(bool)));
}


void PhysicalModelWindowWidget::addInputLine()
{
  inputTableModel_->addLine();
}


void PhysicalModelWindowWidget::addOutputLine()
{
  outputTableModel_->addLine();
}


void PhysicalModelWindowWidget::removeInputLine()
{
  if (inputTableView_->selectionModel()->hasSelection())
  {
    QModelIndex index = inputTableView_->selectionModel()->currentIndex();
    inputTableModel_->removeLine(index);
    const int lastRow = inputTableModel_->rowCount()-1;

    if (lastRow+1)
      inputTableView_->selectRow(lastRow);
  }
}


void PhysicalModelWindowWidget::removeOutputLine()
{
  if (outputTableView_->selectionModel()->hasSelection())
    outputTableModel_->removeLine(outputTableView_->selectionModel()->currentIndex());
}


void PhysicalModelWindowWidget::evaluateOutputs()
{
  // if no outputs do nothing
  if (!physicalModel_.getSelectedOutputsNames().getSize())
    return;

  // evaluate
  ModelEvaluation eval("anEval", physicalModel_);
  eval.run();

  // get result
  NumericalSample outputSample(eval.getResult().getOutputSample());

  // check
  if (!eval.getErrorMessage().empty())
  {
    emit errorMessageChanged(eval.getErrorMessage().c_str());
    return;
  }
  if (!outputSample.getSize())
  {
    emit errorMessageChanged(tr("Not possible to evaluate the outputs"));
    return;
  }

  // set output value
  for (UnsignedInteger i = 0; i < outputSample.getDimension(); ++ i)
    physicalModel_.setOutputValue(outputSample.getDescription()[i], outputSample[0][i]);

  emit errorMessageChanged("");
}
}
