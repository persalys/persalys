//                                               -*- C++ -*-
/**
 *  @brief QWizard to define an evaluation of the model
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
#include "otgui/ModelEvaluationWizard.hxx"

#include "otgui/DoubleSpinBox.hxx"
#include "otgui/ModelEvaluation.hxx"
#include "otgui/QtTools.hxx"

#include <QVBoxLayout>
#include <QHeaderView>

#include <limits>

using namespace OT;

namespace OTGUI
{

ModelEvaluationWizard::ModelEvaluationWizard(const Analysis& analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
  , table_(0)
  , outputsGroupBox_(0)
  , errorMessageLabel_(new TemporaryLabel)
{
  buildInterface();
}


void ModelEvaluationWizard::buildInterface()
{
  setWindowTitle(tr("Model evaluation"));

  // get data
  ModelEvaluation * analysis_ptr = dynamic_cast<ModelEvaluation*>(analysis_.getImplementation()->clone());
  analysis_ptr->updateParameters();

  const PhysicalModel model(analysis_ptr->getPhysicalModel());

  // build page
  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * pageLayout = new QVBoxLayout(page);

  // output selection
  outputsGroupBox_ = new OutputsSelectionGroupBox(model.getSelectedOutputsNames(), analysis_ptr->getInterestVariables(), this);
  pageLayout->addWidget(outputsGroupBox_, 0, Qt::AlignTop);

  // table
  const UnsignedInteger nbInputs = model.getInputDimension();
  QGroupBox * inputsBox = new QGroupBox(tr("Inputs"));
  QVBoxLayout * inputsLayout = new QVBoxLayout(inputsBox);

  table_ = new QTableWidget(nbInputs, 3);
  table_->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Description") << tr("Value"));
  table_->horizontalHeader()->setStretchLastSection(true);

  // fill table_
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
    const double defaultValue = analysis_ptr->getInputValues()[i];
    const double delta(0.1 * fabs(defaultValue));
    const double step(delta > 1e-12 ? 0.5 * delta : 0.1);

    DoubleSpinBox * valueSpinBox = new DoubleSpinBox(table_);
    valueSpinBox->setMinimum(-std::numeric_limits<double>::max());
    valueSpinBox->setMaximum(std::numeric_limits<double>::max());
    valueSpinBox->setValue(defaultValue);
    valueSpinBox->setSingleStep(step);
    table_->setCellWidget(i, 2, valueSpinBox);
  }
  table_->resizeColumnsToContents();

  // if too many variables: no fixed height + use scrollbar
  if (table_->model()->rowCount() < 12)
  {
    int x1, y1, x2, y2;
    table_->getContentsMargins(&x1, &y1, &x2, &y2);
    const int h = table_->verticalHeader()->length() + table_->horizontalHeader()->height();
    table_->setFixedHeight(h + y1 + y2);
  }

  inputsLayout->addWidget(table_);
  inputsLayout->setSizeConstraint(QLayout::SetMaximumSize);
  pageLayout->addWidget(inputsBox, 1);
  pageLayout->addStretch();

  /// -- error message
  pageLayout->addWidget(errorMessageLabel_, 0, Qt::AlignBottom);

  addPage(page);
}


bool ModelEvaluationWizard::validateCurrentPage()
{
  if (!outputsGroupBox_->getSelectedOutputsNames().size())
  {
    errorMessageLabel_->setTemporaryErrorMessage(tr("At least one output must be selected"));
    return false;
  }

  // get the physical model
  PhysicalModel model = dynamic_cast<const PhysicalModelAnalysis*>(analysis_.getImplementation().get())->getPhysicalModel();

  // update analysis_
  ModelEvaluation eval(analysis_.getName(), model);
  eval.setInterestVariables(QtOT::StringListToDescription(outputsGroupBox_->getSelectedOutputsNames()));
  for (int i = 0; i < table_->rowCount(); ++i)
  {
    QWidget * cellWidget = table_->cellWidget(i, 2);
    DoubleSpinBox * spinBox = qobject_cast<DoubleSpinBox *>(cellWidget);
    eval.setInputValue(i, spinBox->value());
  }
  analysis_ = eval;

  return QWizard::validateCurrentPage();
}
}
