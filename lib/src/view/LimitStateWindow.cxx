//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define limit states
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#include "persalys/LimitStateWindow.hxx"

#include "persalys/QtTools.hxx"

#include <openturns/Greater.hxx>
#include <openturns/GreaterOrEqual.hxx>
#include <openturns/Less.hxx>
#include <openturns/LessOrEqual.hxx>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>

using namespace OT;

namespace PERSALYS
{

LimitStateWindow::LimitStateWindow(LimitStateItem * item, QWidget * parent)
  : SubWindow(item, parent)
  , limitState_(item->getLimitState())
{
  buildInterface();

  // connections
  connect(item, SIGNAL(outputListChanged()), this, SLOT(rebuildEventsTable()));
  connect(item, SIGNAL(operatorChanged()), this, SLOT(rebuildEventsTable()));
  connect(item, SIGNAL(thresholdChanged()), this, SLOT(rebuildEventsTable()));
  connect(item, SIGNAL(typeChanged()), this, SLOT(updateTypeWidget()));
}


void LimitStateWindow::buildInterface()
{
  auto * mainLayout = new QVBoxLayout(this);

  mainLayout->addWidget(
    new TitleLabel(
      tr("Limit state"),
      "user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#limit-state"
    )
  );

  // spacer
  mainLayout->addSpacing(30);

  auto * label = new QLabel(tr("Definition of the failure event(s)"));
  label->setStyleSheet("QLabel {font: bold;}");
  mainLayout->addWidget(label);

  // spacer
  mainLayout->addSpacing(5);

  // Type selector (Union/Intersection) – only visible for system limit states
  typeWidget_ = new QWidget;
  auto * typeLayout = new QHBoxLayout(typeWidget_);
  typeLayout->setContentsMargins(0, 0, 0, 0);
  typeLayout->addWidget(new QLabel(tr("Type:")));
  typeComboBox_ = new QComboBox;
  typeComboBox_->addItem(tr("Union"),        static_cast<int>(LimitStateImplementation::Union));
  typeComboBox_->addItem(tr("Intersection"), static_cast<int>(LimitStateImplementation::Intersection));
  connect(typeComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateType(int)));
  typeLayout->addWidget(typeComboBox_);
  typeLayout->addStretch();
  mainLayout->addWidget(typeWidget_);

  // Events table: Output | Operator | Threshold | (Remove)
  eventsTable_ = new QTableWidget(0, 4);
  eventsTable_->setHorizontalHeaderLabels({tr("Output"), tr("Operator"), tr("Threshold"), ""});
  eventsTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  eventsTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  eventsTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
  eventsTable_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
  eventsTable_->setColumnWidth(3, 30);
  eventsTable_->verticalHeader()->setVisible(false);
  eventsTable_->setSelectionMode(QAbstractItemView::NoSelection);
  mainLayout->addWidget(eventsTable_);

  // Add event button
  addEventButton_ = new QPushButton(tr("+ Add failure event"));
  connect(addEventButton_, SIGNAL(clicked()), this, SLOT(addFailureEvent()));
  auto * addBtnLayout = new QHBoxLayout;
  addBtnLayout->addWidget(addEventButton_);
  addBtnLayout->addStretch();
  mainLayout->addLayout(addBtnLayout);

  errorWidget_ = new ErrorWidget;
  mainLayout->addWidget(errorWidget_);

  mainLayout->addStretch(1);

  rebuildEventsTable();
}


void LimitStateWindow::rebuildEventsTable()
{
  errorWidget_->reset();

  const auto outputNames     = limitState_.getOutputNames();
  const auto operators       = limitState_.getOperators();
  const auto thresholds      = limitState_.getThresholds();
  const auto availableOutputs = limitState_.getPhysicalModel().getSelectedOutputsNames();

  QStringList availableList;
  for (UnsignedInteger i = 0; i < availableOutputs.getSize(); ++i)
    availableList << QString::fromUtf8(availableOutputs[i].c_str());

  const int nEvents = static_cast<int>(outputNames.getSize());
  eventsTable_->setRowCount(nEvents);

  for (int i = 0; i < nEvents; ++i)
  {
    // Output combo
    auto * outputCombo = new QComboBox;
    outputCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    outputCombo->addItems(availableList);
    const QString currentOutput = QString::fromUtf8(outputNames[i].c_str());
    const int outputIdx = availableList.indexOf(currentOutput);
    if (outputIdx == -1)
      errorWidget_->setFramelessErrorMessage(tr("The output name '%1' is not valid.").arg(currentOutput));
    outputCombo->setCurrentIndex(outputIdx);
    connect(outputCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this, i](int idx) { updateOutput(i, idx); });
    eventsTable_->setCellWidget(i, 0, outputCombo);

    // Operator combo
    auto * opCombo = new QComboBox;
    opCombo->addItem(tr("<"),  static_cast<int>(LimitStateWindow::LessOperator));
    opCombo->addItem(tr("<="), static_cast<int>(LimitStateWindow::LessOrEqualOperator));
    opCombo->addItem(tr(">"),  static_cast<int>(LimitStateWindow::GreaterOperator));
    opCombo->addItem(tr(">="), static_cast<int>(LimitStateWindow::GreaterOrEqualOperator));
    const String opName = operators[i].getImplementation()->getClassName();
    int opIdx = 0;
    if      (opName == "LessOrEqual")    opIdx = 1;
    else if (opName == "Greater")        opIdx = 2;
    else if (opName == "GreaterOrEqual") opIdx = 3;
    opCombo->setCurrentIndex(opIdx);
    connect(opCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this, i](int idx) { updateOperator(i, idx); });
    eventsTable_->setCellWidget(i, 1, opCombo);

    // Threshold edit
    auto * thresholdEdit = new ValueLineEdit(thresholds[i]);
    connect(thresholdEdit, &QLineEdit::editingFinished,
            this, [this, i, thresholdEdit]() { updateThreshold(i, thresholdEdit); });
    eventsTable_->setCellWidget(i, 2, thresholdEdit);

    // Remove button
    auto * removeButton = new QPushButton(tr("-"));
    removeButton->setFixedWidth(30);
    removeButton->setEnabled(nEvents > 1);
    connect(removeButton, &QPushButton::clicked,
            this, [this, i]() { removeFailureEvent(i); });
    eventsTable_->setCellWidget(i, 3, removeButton);
  }

  // Update type widget
  {
    SignalBlocker blocker(typeComboBox_);
    typeComboBox_->setCurrentIndex(limitState_.getType() == LimitStateImplementation::Union ? 0 : 1);
  }
  typeWidget_->setVisible(limitState_.isSystemLimitState());
}


void LimitStateWindow::updateTypeWidget()
{
  SignalBlocker blocker(typeComboBox_);
  typeComboBox_->setCurrentIndex(limitState_.getType() == LimitStateImplementation::Union ? 0 : 1);
}


void LimitStateWindow::addFailureEvent()
{
  if (limitState_.getPhysicalModel().getSelectedOutputsNames().isEmpty())
  {
    errorWidget_->setFramelessErrorMessage(tr("No output is available in the physical model. Please select at least one output in the physical model settings."));
    return;
  }
  
  limitState_.addFailureEvent(limitState_.getPhysicalModel().getSelectedOutputsNames()[0], Less(), 0.0);
  rebuildEventsTable();
}


void LimitStateWindow::removeFailureEvent(int row)
{
  if (row < 0 || static_cast<UnsignedInteger>(row) >= limitState_.getOutputNames().getSize())
    return;
  limitState_.removeFailureEvent(static_cast<UnsignedInteger>(row));
  rebuildEventsTable();
}


void LimitStateWindow::updateOutput(int row, int comboIndex)
{
  if (comboIndex < 0)
    return;
  const auto availableOutputs = limitState_.getPhysicalModel().getSelectedOutputsNames();
  if (static_cast<UnsignedInteger>(comboIndex) >= availableOutputs.getSize())
    return;
  errorWidget_->reset();
  limitState_.blockNotification("LimitStateItem");
  limitState_.setOutputName(static_cast<UnsignedInteger>(row), availableOutputs[comboIndex]);
  limitState_.blockNotification();
}


void LimitStateWindow::updateOperator(int row, int comboIndex)
{
  const auto * opCombo = qobject_cast<QComboBox *>(eventsTable_->cellWidget(row, 1));
  if (!opCombo)
    return;
  ComparisonOperator comparisonOperator;
  switch (LimitStateWindow::Operator(opCombo->itemData(comboIndex, Qt::UserRole).toInt()))
  {
    case LimitStateWindow::LessOperator:
      comparisonOperator = Less();
      break;
    case LimitStateWindow::LessOrEqualOperator:
      comparisonOperator = LessOrEqual();
      break;
    case LimitStateWindow::GreaterOperator:
      comparisonOperator = Greater();
      break;
    case LimitStateWindow::GreaterOrEqualOperator:
      comparisonOperator = GreaterOrEqual();
      break;
  }
  limitState_.blockNotification("LimitStateItem");
  limitState_.setOperator(static_cast<UnsignedInteger>(row), comparisonOperator);
  limitState_.blockNotification();
}


void LimitStateWindow::updateThreshold(int row, ValueLineEdit * edit)
{
  try
  {
    limitState_.blockNotification("LimitStateItem");
    limitState_.setThreshold(static_cast<UnsignedInteger>(row), edit->value());
    limitState_.blockNotification();
    errorWidget_->reset();
  }
  catch (const std::exception & ex)
  {
    SignalBlocker blocker(edit);
    edit->setValue(limitState_.getThresholds()[row]);
    errorWidget_->setFramelessErrorMessage(ex.what());
  }
}


void LimitStateWindow::updateType(int index)
{
  const auto type = (index == 0) ? LimitStateImplementation::Union : LimitStateImplementation::Intersection;
  limitState_.blockNotification("LimitStateItem");
  limitState_.setType(type);
  limitState_.blockNotification();
}
}

