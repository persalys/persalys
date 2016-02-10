// LimitStateWindow.cxx

#include "otgui/LimitStateWindow.hxx"

#include "Greater.hxx"
#include "GreaterOrEqual.hxx"
#include "LessOrEqual.hxx"

#include <QGridLayout>

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
  QGridLayout * gridLayout = new QGridLayout(mainWidget);

  QLabel * label = new QLabel(tr("Definition of the failure event :"));
  gridLayout->addWidget(label, 0, 0, 1, 3);

  label = new QLabel(tr("Output"));
  gridLayout->addWidget(label, 1, 0);

  label = new QLabel(tr("Operator"));
  gridLayout->addWidget(label, 1, 1);

  label = new QLabel(tr("Threshold"));
  gridLayout->addWidget(label, 1, 2);

  outputsComboBox_ = new QComboBox;
  updateOutputsList();
  connect(outputsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateOutput(int)));
  gridLayout->addWidget(outputsComboBox_, 2, 0);

  failureComboBox_ = new QComboBox;
  failureComboBox_->addItem(tr("<"), LimitStateWindow::LessOperator);
  failureComboBox_->addItem(tr("<="), LimitStateWindow::LessOrEqualOperator);
  failureComboBox_->addItem(tr(">"), LimitStateWindow::GreaterOperator);
  failureComboBox_->addItem(tr(">="), LimitStateWindow::GreaterOrEqualOperator);
  updateOperatorWidget();
  connect(failureComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateOperator(int)));
  gridLayout->addWidget(failureComboBox_, 2, 1);

  thresholdSpinBox_ = new QDoubleSpinBox;
  double doubleMax = std::numeric_limits<double>::max();
  thresholdSpinBox_->setRange(-doubleMax, doubleMax);
  thresholdSpinBox_->setDecimals(4);
  thresholdSpinBox_->setSingleStep(0.1);
  updateThresholdWidget();
  connect(thresholdSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(updateThreshold(double)));
  gridLayout->addWidget(thresholdSpinBox_, 2, 2);

  gridLayout->setRowStretch(3, 2);
  setWidget(mainWidget);
}



void LimitStateWindow::updateOutputsList()
{
  outputsComboBox_->blockSignals(true);
  outputsComboBox_->clear();
  QStringList items;
  for (UnsignedInteger i=0; i<limitState_.getPhysicalModel().getOutputs().getSize(); ++i)
    items << limitState_.getPhysicalModel().getOutputs()[i].getName().c_str();
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
  const String operatorName = limitState_.getOperator().getImplementation()->getClassName();
  int indexOperator = 0;
  if (operatorName == "LessOrEqual")
    indexOperator = 1;
  else if (operatorName == "Greater")
    indexOperator = 2;
  else if (operatorName == "GreaterOrEqual")
    indexOperator = 3;
  failureComboBox_->setCurrentIndex(indexOperator);
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