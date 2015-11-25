// LimitStateWindow.cxx

#include "otgui/LimitStateWindow.hxx"

#include "Greater.hxx"

#include <QHBoxLayout>
#include <QLineEdit>

#include <limits>

using namespace OT;

namespace OTGUI {

LimitStateWindow::LimitStateWindow(LimitStateItem * item)
 : OTguiSubWindow(item)
 , limitState_(item->getLimitState())
{
  buildInterface();
  connect(item, SIGNAL(outputChanged()), this, SLOT(updateOutputsList()));
  connect(item, SIGNAL(outputNameChanged()), this, SLOT(updateOutputWidget()));
  connect(item, SIGNAL(operatorChanged()), this, SLOT(updateOperatorWidget()));
  connect(item, SIGNAL(thresholdChanged()), this, SLOT(updateThresholdWidget()));
}


void LimitStateWindow::buildInterface()
{
  setWindowTitle("Limit state");

  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  QHBoxLayout * layout = new QHBoxLayout;

  outputsComboBox_ = new QComboBox;
  updateOutputsList();
  connect(outputsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateOutput(int)));
  layout->addWidget(outputsComboBox_);

  failureComboBox_ = new QComboBox;
  failureComboBox_->addItem(tr("<"), LimitStateWindow::LessOperator);
  failureComboBox_->addItem(tr(">"), LimitStateWindow::GreaterOperator);
  updateOperatorWidget();
  connect(failureComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateOperator(int)));
  layout->addWidget(failureComboBox_);

  thresholdSpinBox_ = new QDoubleSpinBox;
  double doubleMax = std::numeric_limits<double>::max();
  thresholdSpinBox_->setRange(-doubleMax, doubleMax);
  thresholdSpinBox_->setDecimals(4);
  thresholdSpinBox_->setSingleStep(0.1);
  updateThresholdWidget();
  connect(thresholdSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(updateThreshold(double)));
  layout->addWidget(thresholdSpinBox_);

  layout->addStretch();
  mainLayout->addLayout(layout);
  mainLayout->addStretch();
  setWidget(mainWidget);
}



void LimitStateWindow::updateOutputsList()
{
  outputsComboBox_->blockSignals(true);
  outputsComboBox_->clear();
  QStringList items = QStringList();
  for (int i=0; i<limitState_.getPhysicalModel().getOutputs().getSize(); ++i)
    items<<QString::fromStdString(limitState_.getPhysicalModel().getOutputs()[i].getName());
  outputsComboBox_->addItems(items);

  const int index = items.indexOf(limitState_.getOutputName().c_str());
//   if (index != -1)
//     outputsComboBox_->setCurrentIndex( index + 1 );
  outputsComboBox_->setCurrentIndex(index);
  outputsComboBox_->blockSignals(false);
}


void LimitStateWindow::updateOutputWidget()
{
  outputsComboBox_->blockSignals(true);
  const int index = outputsComboBox_->findText(limitState_.getOutputName().c_str());
  outputsComboBox_->setCurrentIndex(index);
  outputsComboBox_->blockSignals(false);
}


void LimitStateWindow::updateOperatorWidget()
{
  failureComboBox_->blockSignals(true);
  int index = 0;
  if (limitState_.getOperator().getImplementation()->getClassName() == "Greater")
    index = 1;
  failureComboBox_->setCurrentIndex(index);
  failureComboBox_->blockSignals(false);
}


void LimitStateWindow::updateThresholdWidget()
{
  thresholdSpinBox_->blockSignals(true);
  thresholdSpinBox_->setValue(limitState_.getThreshold());
  thresholdSpinBox_->blockSignals(false);
}


void LimitStateWindow::updateOutput(int index)
{
  limitState_.blockNotification(true);
  limitState_.setOutputName(outputsComboBox_->itemText(index).toStdString());
  limitState_.blockNotification(false);
}


void LimitStateWindow::updateOperator(int index)
{
  ComparisonOperator comparisonOperator;
  switch (LimitStateWindow::Operator(failureComboBox_->itemData(index, Qt::UserRole).toInt()))
  {
    case LimitStateWindow::LessOperator:
      comparisonOperator = Less();
      break;
    case LimitStateWindow::GreaterOperator:
      comparisonOperator = Greater();
      break;
  }
  limitState_.blockNotification(true);
  limitState_.setOperator(comparisonOperator);
  limitState_.blockNotification(false);
}


void LimitStateWindow::updateThreshold(double value)
{
  limitState_.blockNotification(true);
  limitState_.setThreshold(value);
  limitState_.blockNotification(false);
}
}