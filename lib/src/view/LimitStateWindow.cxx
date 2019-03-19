//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define limit states
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
#include "otgui/LimitStateWindow.hxx"

#include "otgui/QtTools.hxx"

#include <openturns/Greater.hxx>
#include <openturns/GreaterOrEqual.hxx>
#include <openturns/LessOrEqual.hxx>

#include <QGridLayout>

using namespace OT;

namespace OTGUI
{

LimitStateWindow::LimitStateWindow(LimitStateItem * item, QWidget * parent)
  : SubWindow(item, parent)
  , limitState_(item->getLimitState())
  , errorMessageLabel_(0)
{
  buildInterface();

  // connections
  connect(item, SIGNAL(outputListChanged()), this, SLOT(updateOutputsList()));
  connect(item, SIGNAL(operatorChanged()), this, SLOT(updateOperatorWidget()));
  connect(item, SIGNAL(thresholdChanged()), this, SLOT(updateThresholdWidget()));
}


void LimitStateWindow::buildInterface()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  mainLayout->addWidget(new TitleLabel(tr("Limit state"), "user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#limit-state"));

  // spacer
  mainLayout->addSpacing(30);

  QLabel * label = new QLabel(tr("Definition of the failure event"));
  label->setStyleSheet("QLabel {font: bold;}");
  mainLayout->addWidget(label);

  // spacer
  mainLayout->addSpacing(5);

  // definition widgets
  QGridLayout * gridLayout = new QGridLayout;
  int row = 0;

  label = new QLabel(tr("Output"));
  gridLayout->addWidget(label, row, 0);

  label = new QLabel(tr("Operator"));
  gridLayout->addWidget(label, row, 1);

  label = new QLabel(tr("Threshold"));
  gridLayout->addWidget(label, row, 2);

  outputsComboBox_ = new QComboBox;
  outputsComboBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  connect(outputsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateOutput(int)));
  gridLayout->addWidget(outputsComboBox_, ++row, 0);

  failureComboBox_ = new QComboBox;
  failureComboBox_->addItem(tr("<"), LimitStateWindow::LessOperator);
  failureComboBox_->addItem(tr("<="), LimitStateWindow::LessOrEqualOperator);
  failureComboBox_->addItem(tr(">"), LimitStateWindow::GreaterOperator);
  failureComboBox_->addItem(tr(">="), LimitStateWindow::GreaterOrEqualOperator);
  connect(failureComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateOperator(int)));
  gridLayout->addWidget(failureComboBox_, row, 1);

  thresholdLineEdit_ = new ValueLineEdit;
  connect(thresholdLineEdit_, SIGNAL(editingFinished()), this, SLOT(updateThreshold()));
  gridLayout->addWidget(thresholdLineEdit_, row, 2);

  errorMessageLabel_ = new TemporaryLabel;
  gridLayout->addWidget(errorMessageLabel_, ++row, 0, 1, 3);

  gridLayout->setRowStretch(++row, 2);

  mainLayout->addLayout(gridLayout, 1);

  updateOutputsList();
  updateOperatorWidget();
  updateThresholdWidget();
}



void LimitStateWindow::updateOutputsList()
{
  errorMessageLabel_->reset();
  SignalBlocker blocker(outputsComboBox_);
  outputsComboBox_->clear();
  QStringList items;
  for (UnsignedInteger i = 0; i < limitState_.getPhysicalModel().getSelectedOutputsNames().getSize(); ++i)
    items << QString::fromUtf8(limitState_.getPhysicalModel().getSelectedOutputsNames()[i].c_str());
  outputsComboBox_->addItems(items);

  const int index = items.indexOf(QString::fromUtf8(limitState_.getOutputName().c_str()));

  if (index == -1)
  {
    errorMessageLabel_->setErrorMessage(tr("The output name is not valid."));
  }
  outputsComboBox_->setCurrentIndex(index);
}


void LimitStateWindow::updateOutputWidget()
{
  SignalBlocker blocker(outputsComboBox_);
  const int index = outputsComboBox_->findText(QString::fromUtf8(limitState_.getOutputName().c_str()));
  outputsComboBox_->setCurrentIndex(index);
}


void LimitStateWindow::updateOperatorWidget()
{
  SignalBlocker blocker(failureComboBox_);
  const String operatorName = limitState_.getOperator().getImplementation()->getClassName();
  int indexOperator = 0;
  if (operatorName == "LessOrEqual")
    indexOperator = 1;
  else if (operatorName == "Greater")
    indexOperator = 2;
  else if (operatorName == "GreaterOrEqual")
    indexOperator = 3;
  failureComboBox_->setCurrentIndex(indexOperator);
}


void LimitStateWindow::updateThresholdWidget()
{
  SignalBlocker blocker(thresholdLineEdit_);
  thresholdLineEdit_->setValue(limitState_.getThreshold());
}


void LimitStateWindow::updateOutput(int index)
{
  errorMessageLabel_->reset();
  limitState_.blockNotification("LimitState");
  limitState_.setOutputName(outputsComboBox_->itemText(index).toStdString());
  limitState_.blockNotification();
}


void LimitStateWindow::updateOperator(int index)
{
  ComparisonOperator comparisonOperator;
  switch (LimitStateWindow::Operator(failureComboBox_->itemData(index, Qt::UserRole).toInt()))
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
  limitState_.blockNotification("LimitState");
  limitState_.setOperator(comparisonOperator);
  limitState_.blockNotification();
}


void LimitStateWindow::updateThreshold()
{
  try
  {
    limitState_.blockNotification("LimitState");
    limitState_.setThreshold(thresholdLineEdit_->value());
    limitState_.blockNotification();
    errorMessageLabel_->reset();
  }
  catch(std::exception & ex)
  {
    updateThresholdWidget();
    errorMessageLabel_->setTemporaryErrorMessage(ex.what());
  }
}
}
