//                                               -*- C++ -*-
/**
 *  @brief QWizard to define an evaluation of the model
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/SpinBoxDelegate.hxx"
#include "otgui/ModelEvaluation.hxx"

#include <QVBoxLayout>
#include <QTableView>
#include <QDoubleSpinBox>
#include <QGroupBox>

#include <limits>

using namespace OT;

namespace OTGUI {

ModelEvaluationWizard::ModelEvaluationWizard(OTStudy * otStudy, const PhysicalModel & physicalModel)
  : AnalysisWizard(ModelEvaluation(otStudy->getAvailableAnalysisName("evaluation_"), physicalModel))
{
  buildInterface();
}


ModelEvaluationWizard::ModelEvaluationWizard(const Analysis & analysis)
  : AnalysisWizard(analysis)
{
  dynamic_cast<ModelEvaluation*>(&*analysis_.getImplementation())->updateParameters();
  buildInterface();
}


void ModelEvaluationWizard::buildInterface()
{
  setWindowTitle("Model evaluation");

  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * pageLayout = new QVBoxLayout(page);

  QGroupBox * inputsBox = new QGroupBox(tr("Inputs"));
  QVBoxLayout * inputsLayout = new QVBoxLayout(inputsBox);

  PhysicalModel model(dynamic_cast<const PhysicalModelAnalysis*>(&*analysis_.getImplementation())->getPhysicalModel());

  table_ = new QTableWidget(model.getInputs().getSize(), 3);
  table_->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Description") << tr("Value"));

  for (UnsignedInteger i=0; i<model.getInputs().getSize(); ++i)
  {
    QTableWidgetItem * item = new QTableWidgetItem(QString::fromUtf8(model.getInputNames()[i].c_str()));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    table_->setItem(i, 0, item);
    item = new QTableWidgetItem(QString::fromUtf8(model.getInputs()[i].getDescription().c_str()));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    table_->setItem(i, 1, item);
      
    double defaultValue = dynamic_cast<ModelEvaluation*>(&*analysis_.getImplementation())->getInputValues()[i];
    double delta(0.1*fabs(defaultValue));
    double step(delta > 1e-12 ? 0.5*delta : 0.1);

    QDoubleSpinBox * valueSpinBox = new QDoubleSpinBox;
    valueSpinBox->setMinimum(-std::numeric_limits<double>::max());
    valueSpinBox->setMaximum(std::numeric_limits<double>::max());
    valueSpinBox->setValue(defaultValue);
    valueSpinBox->setSingleStep(step);
    table_->setCellWidget(i, 2, valueSpinBox);
    connect(valueSpinBox, SIGNAL(valueChanged(double)), this, SLOT(inputValueChanged(double)));
  }
  inputsLayout->addWidget(table_);
  pageLayout->addWidget(inputsBox);
  addPage(page);
}


void ModelEvaluationWizard::inputValueChanged(double value)
{
  QDoubleSpinBox * spinBox = qobject_cast<QDoubleSpinBox *>(sender());
  for (int i=0; i<table_->rowCount(); ++i)
  {
    QWidget * cellWidget = table_->cellWidget(i, 2);
    if (cellWidget == spinBox)
    {
      dynamic_cast<ModelEvaluation*>(&*analysis_.getImplementation())->setInputValue(i, value);
      return;
    }
  }
}
}
