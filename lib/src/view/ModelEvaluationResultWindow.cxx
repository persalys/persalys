// ModelEvaluationResultWindow.cxx

#include "otgui/ModelEvaluationResultWindow.hxx"
#include "otgui/ModelEvaluation.hxx"

#include <QHBoxLayout>
#include <QTableWidget>
#include <QGroupBox>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {

ModelEvaluationResultWindow::ModelEvaluationResultWindow(ModelEvaluationItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<ModelEvaluation*>(&*item->getAnalysis().getImplementation())->getResult())
{
  buildInterface();
}


void ModelEvaluationResultWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QHBoxLayout * tabLayout = new QHBoxLayout(tab);

  // inputs table
  QGroupBox * inputsBox = new QGroupBox(tr("Inputs"));
  QVBoxLayout * inputsLayout = new QVBoxLayout(inputsBox);

  QTableWidget * inputTable = new QTableWidget(result_.getInputSample().getDimension(), 2);
  inputTable->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
  inputTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

  for (UnsignedInteger i=0; i<result_.getInputSample().getDimension(); ++i)
  {
    QTableWidgetItem * item = new QTableWidgetItem(result_.getInputSample().getDescription()[i].c_str());
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    inputTable->setItem(i, 0, item);
    item = new QTableWidgetItem(QString::number(result_.getInputSample()[0][i]));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    inputTable->setItem(i, 1, item);
  }
  inputsLayout->addWidget(inputTable);
  tabLayout->addWidget(inputsBox);

  // outputs table
  QGroupBox * outputsBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayout = new QVBoxLayout(outputsBox);

  QTableWidget * outputTable = new QTableWidget(result_.getOutputSample().getDimension(), 2);
  outputTable->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
  outputTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

  for (UnsignedInteger i=0; i<result_.getOutputSample().getDimension(); ++i)
  {
    QTableWidgetItem * item = new QTableWidgetItem(result_.getOutputSample().getDescription()[i].c_str());
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    outputTable->setItem(i, 0, item);
    item = new QTableWidgetItem(QString::number(result_.getOutputSample()[0][i]));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    outputTable->setItem(i, 1, item);
  }
  outputsLayout->addWidget(outputTable);
  tabLayout->addWidget(outputsBox);

  tabWidget->addTab(tab, tr("Result"));

  setWidget(tabWidget);
}
}