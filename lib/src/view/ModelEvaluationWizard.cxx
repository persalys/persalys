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
#include "otgui/ModelEvaluationWizard.hxx"

#include "otgui/DoubleSpinBox.hxx"
#include "otgui/ModelEvaluation.hxx"

#include <QVBoxLayout>
#include <QHeaderView>

#include <limits>

using namespace OT;

namespace OTGUI {

ModelEvaluationWizard::ModelEvaluationWizard(const Analysis& analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
  , errorMessageLabel_(new QLabel)
{
  dynamic_cast<ModelEvaluation*>(analysis_.getImplementation().get())->updateParameters();
  buildInterface();
}


void ModelEvaluationWizard::buildInterface()
{
  setWindowTitle(tr("Model evaluation"));

  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * pageLayout = new QVBoxLayout(page);

  PhysicalModel model(dynamic_cast<const PhysicalModelAnalysis*>(analysis_.getImplementation().get())->getPhysicalModel());

  // output selection
  outputsGroupBox_ = new OutputsSelectionGroupBox(model.getSelectedOutputsNames(), analysis_.getImplementation()->getInterestVariables(), this);
  setInterestVariables(outputsGroupBox_->getSelectedOutputsNames());
  connect(outputsGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), this, SLOT(setInterestVariables(QStringList)));
  pageLayout->addWidget(outputsGroupBox_);

  const UnsignedInteger nbInputs = model.getInputs().getSize();
  QGroupBox * inputsBox = new QGroupBox(tr("Inputs"));
  QVBoxLayout * inputsLayout = new QVBoxLayout(inputsBox);

  table_ = new QTableWidget(nbInputs, 3);
  table_->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Description") << tr("Value"));

  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    // input name
    QTableWidgetItem * item = new QTableWidgetItem(QString::fromUtf8(model.getInputNames()[i].c_str()));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    table_->setItem(i, 0, item);
    // input description
    item = new QTableWidgetItem(QString::fromUtf8(model.getInputs()[i].getDescription().c_str()));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    table_->setItem(i, 1, item);
    // input value
    double defaultValue = dynamic_cast<ModelEvaluation*>(analysis_.getImplementation().get())->getInputValues()[i];
    double delta(0.1 * fabs(defaultValue));
    double step(delta > 1e-12 ? 0.5 * delta : 0.1);

    DoubleSpinBox * valueSpinBox = new DoubleSpinBox;
    valueSpinBox->setMinimum(-std::numeric_limits<double>::max());
    valueSpinBox->setMaximum(std::numeric_limits<double>::max());
    valueSpinBox->setValue(defaultValue);
    valueSpinBox->setSingleStep(step);
    table_->setCellWidget(i, 2, valueSpinBox);
    connect(valueSpinBox, SIGNAL(valueChanged(double)), this, SLOT(inputValueChanged(double)));
  }

#if QT_VERSION >= 0x050000
  table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
  table_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
  table_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
#else
  table_->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
  table_->horizontalHeader()->setResizeMode(1, QHeaderView::Interactive);
  table_->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
#endif

  inputsLayout->addWidget(table_);
  pageLayout->addWidget(inputsBox);

  /// -- error message
  errorMessageLabel_->setWordWrap(true);
  pageLayout->addStretch();
  pageLayout->addWidget(errorMessageLabel_);

  addPage(page);
}


void ModelEvaluationWizard::inputValueChanged(double value)
{
  DoubleSpinBox * spinBox = qobject_cast<DoubleSpinBox *>(sender());
  for (int i = 0; i < table_->rowCount(); ++i)
  {
    QWidget * cellWidget = table_->cellWidget(i, 2);
    if (cellWidget == spinBox)
    {
      dynamic_cast<ModelEvaluation*>(analysis_.getImplementation().get())->setInputValue(i, value);
      return;
    }
  }
}


void ModelEvaluationWizard::setInterestVariables(QStringList outputsList)
{
  errorMessageLabel_->setText("");

  Description desc(outputsList.size());
  for (int i = 0; i < outputsList.size(); ++i)
    desc[i] = outputsList[i].toUtf8().constData();

  try
  {
    analysis_.getImplementation()->setInterestVariables(desc);
  }
  catch (InvalidDimensionException exception)
  {
    // check in validateCurrentPage
  }
}


bool ModelEvaluationWizard::validateCurrentPage()
{
  errorMessageLabel_->setText("");
  if (!outputsGroupBox_->getSelectedOutputsNames().size())
  {
    errorMessageLabel_->setText(QString("<font color=red>%1</font>").arg(tr("At least one output must be selected")));
    return false;
  }

  return QWizard::validateCurrentPage();
}
}
