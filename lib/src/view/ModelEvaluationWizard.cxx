// ModelEvaluationWizard.cxx

#include "otgui/ModelEvaluationWizard.hxx"
#include "otgui/SpinBoxDelegate.hxx"
#include "otgui/ModelEvaluation.hxx"

#include <QVBoxLayout>
#include <QTableView>
#include <QDoubleSpinBox>
#include <QGroupBox>

#include <limits>

namespace OTGUI {

ModelEvaluationWizard::ModelEvaluationWizard(OTStudy * otStudy, const PhysicalModel & physicalModel)
  : QWizard()
  , analysis_(ModelEvaluation(otStudy->getAvailableAnalysisName("evaluation_"), physicalModel))
  , otStudy_(otStudy)
{
  buildInterface();
}


ModelEvaluationWizard::ModelEvaluationWizard(const Analysis & analysis)
  : QWizard()
  , analysis_(analysis)
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

  table_ = new QTableWidget(analysis_.getPhysicalModel().getInputs().getSize(), 3);
  table_->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Description") << tr("Value"));

  for (int i=0; i<analysis_.getPhysicalModel().getInputs().getSize(); ++i)
  {
    QTableWidgetItem * item = new QTableWidgetItem(analysis_.getPhysicalModel().getInputNames()[i].c_str());
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    table_->setItem(i, 0, item);
    item = new QTableWidgetItem(analysis_.getPhysicalModel().getInputs()[i].getDescription().c_str());
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    table_->setItem(i, 1, item);
      
    double defaultValue = dynamic_cast<ModelEvaluation*>(&*analysis_.getImplementation())->getInputsValues()[i];
    double delta(0.1*fabs(defaultValue));
    double step(delta > 1e-12 ? 0.5*delta : 0.1);

    QDoubleSpinBox * valueSpinBox = new QDoubleSpinBox;
    valueSpinBox->setMinimum(std::numeric_limits<int>::min());
    valueSpinBox->setMaximum(std::numeric_limits<int>::max());
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


void ModelEvaluationWizard::validate()
{
  otStudy_->addAnalysis(analysis_);
  analysis_.run();
}
}