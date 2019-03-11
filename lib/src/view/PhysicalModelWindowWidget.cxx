//                                               -*- C++ -*-
/**
 *  @brief QWidget to visualize the variables of the physical model
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/PhysicalModelWindowWidget.hxx"

#include "otgui/ModelEvaluation.hxx"
#include "otgui/LineEditWithQValidatorDelegate.hxx"
#include "otgui/CheckableHeaderView.hxx"
#include "otgui/SpinBoxDelegate.hxx"
#include "otgui/InputTableModel.hxx"
#include "otgui/OutputTableModel.hxx"
#include "otgui/DifferentiationTableModel.hxx"
#include "otgui/FieldModelEvaluation.hxx"
#include "otgui/FieldModelEvaluationResultWindow.hxx"

#include <QHeaderView>
#include <QSplitter>
#include <QScrollBar>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QScrollArea>

using namespace OT;

namespace OTGUI
{

PhysicalModelWindowWidget::PhysicalModelWindowWidget(PhysicalModelDefinitionItem * item)
  : QTabWidget()
  , physicalModel_(item->getPhysicalModel())
  , isFirstPaint_(true)
  , inputTableView_(0)
  , outputTableView_(0)
  , errorMessageLabel_(0)
  , indexParamLabel_(0)
{
  connect(item, SIGNAL(numberInputsChanged()), this, SIGNAL(updateInputTableData()));
  connect(item, SIGNAL(inputListDefinitionChanged()), this, SIGNAL(updateInputTableData()));

  connect(item, SIGNAL(numberOutputsChanged()), this, SIGNAL(updateOutputTableData()));
  connect(item, SIGNAL(outputChanged()), this, SIGNAL(updateOutputTableData()));

  connect(item, SIGNAL(inputListDifferentiationChanged()), this, SIGNAL(updateDifferentiationTableData()));
  connect(item, SIGNAL(numberInputsChanged()), this, SIGNAL(updateDifferentiationTableData()));

  connect(item, SIGNAL(meshChanged()), this, SLOT(updateIndexParamLabel()));

  buildInterface();
}


void PhysicalModelWindowWidget::buildInterface()
{
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  QWidget * tab = new QWidget;
  QVBoxLayout * vbox = new QVBoxLayout(tab);
  scrollArea->setWidget(tab);

  QSplitter * verticalSplitter = new QSplitter(Qt::Vertical);

  // Table Inputs -------------------------------------------
  QGroupBox * inputsBox = new QGroupBox(tr("Inputs"));
  QVBoxLayout * inputsLayout = new QVBoxLayout(inputsBox);

  inputTableView_ = new CopyableTableView;
  inputTableView_->setEditTriggers(QTableView::AllEditTriggers);
  inputTableView_->setItemDelegateForColumn(0, new LineEditWithQValidatorDelegate(inputTableView_));
  inputTableView_->horizontalHeader()->setStretchLastSection(true);

  InputTableModel * inputTableModel = new InputTableModel(physicalModel_, inputTableView_);
  inputTableView_->setModel(inputTableModel);

  // connections
  connect(this, SIGNAL(updateInputTableData()), inputTableModel, SLOT(updateData()));

  inputsLayout->addWidget(inputTableView_);

  // buttons Add/Remove input
  if (physicalModel_.getImplementation()->getClassName().find("Symbolic") != std::string::npos)
  {
    QPushButton * addInputLineButton = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"));
    addInputLineButton->setToolTip(tr("Add an input"));
    connect(addInputLineButton, SIGNAL(clicked(bool)), inputTableModel, SLOT(addLine()));

    QPushButton * removeInputLineButton = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
    removeInputLineButton->setToolTip(tr("Remove the selected input"));
    connect(removeInputLineButton, SIGNAL(clicked(bool)), this, SLOT(removeInputLine()));
    connect(this, SIGNAL(removeInputLine(QModelIndex)), inputTableModel, SLOT(removeLine(QModelIndex)));

    QHBoxLayout * buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(addInputLineButton);
    buttonsLayout->addWidget(removeInputLineButton);
    inputsLayout->addLayout(buttonsLayout);
  }

  // index parameter label
  if (physicalModel_.hasMesh())
  {
    indexParamLabel_ = new QLabel;
    inputsLayout->addWidget(indexParamLabel_);
    updateIndexParamLabel();
  }

  verticalSplitter->addWidget(inputsBox);
  verticalSplitter->setStretchFactor(0, 5);

  // Table Outputs -------------------------------------------
  QGroupBox * outputsBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayout = new QVBoxLayout(outputsBox);

  // table view
  outputTableView_ = new CopyableTableView;
  outputTableView_->setEditTriggers(QTableView::AllEditTriggers);
  outputTableView_->setItemDelegateForColumn(0, new LineEditWithQValidatorDelegate(true, outputTableView_));

  OutputTableModel * outputTableModel = new OutputTableModel(physicalModel_, outputTableView_);
  outputTableView_->setModel(outputTableModel);

  // table header view
  CheckableHeaderView * outputTableHeaderView = new CheckableHeaderView;
  outputTableHeaderView->setStretchLastSection(true);
  outputTableView_->setHorizontalHeader(outputTableHeaderView);

  // connections
  connect(this, SIGNAL(updateOutputTableData()), outputTableModel, SLOT(updateData()));

  outputsLayout->addWidget(outputTableView_);

  // buttons
  QHBoxLayout * outputButtonsLayout = new QHBoxLayout;

  // button Evaluate outputs -------------------------------------------
  QPushButton * evaluateOutputsButton = new QPushButton(QIcon(":/images/system-run.png"), tr("Evaluate"));
  evaluateOutputsButton->setToolTip(tr("Evaluate the outputs"));
  connect(evaluateOutputsButton, SIGNAL(clicked(bool)), this, SLOT(evaluateOutputs()));
  outputButtonsLayout->addWidget(evaluateOutputsButton);

  outputsLayout->addLayout(outputButtonsLayout);
  outputButtonsLayout->addStretch();

  // buttons Add/Remove output
  if (physicalModel_.getImplementation()->getClassName().find("Symbolic") != std::string::npos)
  {
    QPushButton * addOutputLineButton = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"));
    addOutputLineButton->setToolTip(tr("Add an output"));
    connect(addOutputLineButton, SIGNAL(clicked(bool)), outputTableModel, SLOT(addLine()));

    QPushButton * removeOutputLineButton = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
    removeOutputLineButton->setToolTip(tr("Remove the selected output"));
    connect(removeOutputLineButton, SIGNAL(clicked(bool)), this, SLOT(removeOutputLine()));
    connect(this, SIGNAL(removeOutputLine(QModelIndex)), outputTableModel, SLOT(removeLine(QModelIndex)));

    outputButtonsLayout->addWidget(addOutputLineButton);
    outputButtonsLayout->addWidget(removeOutputLineButton);
  }

  verticalSplitter->addWidget(outputsBox);
  verticalSplitter->setStretchFactor(1, 3);

  vbox->addWidget(verticalSplitter);

  // - multiprocessing
  if (physicalModel_.getImplementation()->getClassName().find("Python") != std::string::npos)
  {
    QCheckBox * checkBox = new QCheckBox(tr("Enable multiprocessing"));
    checkBox->setChecked(physicalModel_.isParallel());
    checkBox->setToolTip(tr("Warning: the parallelization operation must be significantly faster than the code execution"));
    connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(updateMultiprocessingStatus(int)));
    vbox->addWidget(checkBox);
  }

  // - error message label
  errorMessageLabel_ = new TemporaryLabel;
  vbox->addWidget(errorMessageLabel_);
  connect(this, SIGNAL(resetMessageLabel()), errorMessageLabel_, SLOT(reset()));
  connect(inputTableModel, SIGNAL(errorMessageChanged(QString)), errorMessageLabel_, SLOT(setErrorMessage(QString)));
  connect(outputTableModel, SIGNAL(errorMessageChanged(QString)), errorMessageLabel_, SLOT(setErrorMessage(QString)));

  addTab(scrollArea, tr("Definition"));

  // differentiation
  if (physicalModel_.getImplementation()->getClassName() != "MetaModel")
  {
    tab = new QWidget;
    vbox = new QVBoxLayout(tab);

    QLabel * label = new QLabel(tr("Finite difference step definition"));
    label->setStyleSheet("QLabel {font: bold;}");
    vbox->addWidget(label);

    CopyableTableView * differentiationTableView = new CopyableTableView;
    differentiationTableView->horizontalHeader()->setStretchLastSection(true);

    SpinBoxDelegate * spinBoxDelegate = new SpinBoxDelegate(differentiationTableView);
    spinBoxDelegate->setSpinBoxType(SpinBoxDelegate::differentiationStep);
    differentiationTableView->setItemDelegateForColumn(1, spinBoxDelegate);
    differentiationTableView->setEditTriggers(QTableView::AllEditTriggers);

    DifferentiationTableModel * differentiationTableModel  = new DifferentiationTableModel(physicalModel_, differentiationTableView);
    differentiationTableView->setModel(differentiationTableModel);

    // connections
    connect(spinBoxDelegate, SIGNAL(applyToAllRequested(double)), differentiationTableModel, SLOT(applyValueToAll(double)));
    connect(this, SIGNAL(updateInputTableData()), differentiationTableModel, SLOT(updateData()));
    connect(inputTableModel, SIGNAL(inputNumberChanged()), differentiationTableModel, SLOT(updateData()));
    connect(inputTableModel, SIGNAL(inputNameChanged()), differentiationTableModel, SLOT(updateData()));

    vbox->addWidget(differentiationTableView);

    addTab(tab, tr("Differentiation"));
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


void PhysicalModelWindowWidget::paintEvent(QPaintEvent * event)
{
  QTabWidget::paintEvent(event);

  // initialize tables size
  if (isFirstPaint_)
  {
    resizeInputTable();
    resizeOutputTable();
    isFirstPaint_ = false;
  }
}


void PhysicalModelWindowWidget::resizeInputTable()
{
  const int width = inputTableView_->horizontalHeader()->width();
  const int minSectionSize = outputTableView_->horizontalHeader()->minimumSectionSize();
  inputTableView_->horizontalHeader()->resizeSection(0, minSectionSize);
  inputTableView_->horizontalHeader()->resizeSection(1, width - 2.4 * minSectionSize);
  inputTableView_->horizontalHeader()->resizeSection(2, minSectionSize);
}


void PhysicalModelWindowWidget::resizeOutputTable()
{
  const int width = outputTableView_->horizontalHeader()->width();

  const int minSectionSize = outputTableView_->horizontalHeader()->minimumSectionSize();
  outputTableView_->horizontalHeader()->resizeSection(0, minSectionSize);

  if (physicalModel_.getImplementation()->getClassName().find("Symbolic") == std::string::npos)
  {
    outputTableView_->setColumnHidden(2, true); // hide formula section
    outputTableView_->horizontalHeader()->resizeSection(1, width - 2 * minSectionSize);
    if (!physicalModel_.hasMesh())
      outputTableView_->horizontalHeader()->resizeSection(3, minSectionSize);
  }
  else
  {
    const int width2 = width - 2 * minSectionSize;
    outputTableView_->horizontalHeader()->resizeSection(1, width2 * 4 / 10);
    outputTableView_->horizontalHeader()->resizeSection(2, width2 * 5 / 10);
  }
}


void PhysicalModelWindowWidget::removeInputLine()
{
  if (inputTableView_->selectionModel()->hasSelection())
  {
    QModelIndex index = inputTableView_->selectionModel()->currentIndex();
    emit removeInputLine(index);
    const int lastRow = inputTableView_->model()->rowCount() - 1;

    if (lastRow + 1)
      inputTableView_->selectRow(lastRow);
  }
}


void PhysicalModelWindowWidget::removeOutputLine()
{
  if (outputTableView_->selectionModel()->hasSelection())
    emit removeOutputLine(outputTableView_->selectionModel()->currentIndex());
}


void PhysicalModelWindowWidget::evaluateOutputs()
{
  // if no outputs do nothing
  if (!physicalModel_.getSelectedOutputsNames().getSize())
    return;
  removeTab(2);

  // evaluate
  if (!physicalModel_.hasMesh())
  {
    ModelEvaluation eval("anEval", physicalModel_);
    try
    {
      eval.run();
    }
    catch (std::exception& ex)
    {
      // do nothing
    }

    // get result
    Sample outputSample(eval.getResult().getDesignOfExperiment().getOutputSample());

    // check
    if (!eval.getErrorMessage().empty())
    {
      errorMessageLabel_->setErrorMessage(eval.getErrorMessage().c_str());
      return;
    }
    if (!outputSample.getSize())
    {
      errorMessageLabel_->setErrorMessage(tr("Not possible to evaluate the outputs"));
      return;
    }

    // set output value
    for (UnsignedInteger i = 0; i < outputSample.getDimension(); ++ i)
      physicalModel_.setOutputValue(outputSample.getDescription()[i], outputSample(0, i));
  }
  else
  {
    FieldModelEvaluation eval("anEval", physicalModel_);
    try
    {
      eval.run();

      Sample inputSample(1, eval.getValues());
      inputSample.setDescription(physicalModel_.getInputNames());
      FieldModelEvaluationResultWidget * widget = new FieldModelEvaluationResultWidget(inputSample, eval.getProcessSample(), this);
      addTab(widget, tr("Evaluation"));
      setCurrentIndex(2);
    }
    catch (std::exception& ex)
    {
      errorMessageLabel_->setErrorMessage(tr("Not possible to evaluate the outputs %1").arg(ex.what()));
      return;
    }
  }
  errorMessageLabel_->reset();
}


void PhysicalModelWindowWidget::updateMultiprocessingStatus(int state)
{
  physicalModel_.setParallel(state == Qt::Checked);
}


void PhysicalModelWindowWidget::updateIndexParamLabel()
{
  indexParamLabel_->setText(tr("Index parameter : %1").arg(physicalModel_.getMeshModel().getIndexParameters()[0].getName().c_str()));
}
}
