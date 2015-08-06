// ParametricCalculusWizard.cxx

#include "ParametricCalculusWizard.hxx"

#include <QVBoxLayout>
#include <QTableView>

namespace OTGUI {

ParametricCalculusWizard::ParametricCalculusWizard(ParametricCalculusItem * item)
  : QWizard()
  , item_(item)
{
  buildInterface();
}


ParametricCalculusWizard::~ParametricCalculusWizard()
{
  
}


void ParametricCalculusWizard::buildInterface()
{
  QWizardPage * page = new QWizardPage(this);
  setWindowTitle("Parametric analysis");

  QVBoxLayout * pageLayout = new QVBoxLayout;

  QTableView * tableView = new QTableView;
  model_ = new ParametricCalculusTableModel(item_->getCalculus<ParametricCalculus>());
  connect(model_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(parametrizationChanged(QModelIndex,QModelIndex)));

  tableView->setModel(model_);
  pageLayout->addWidget(tableView);
  page->setLayout(pageLayout);

  addPage(page);
}


void ParametricCalculusWizard::parametrizationChanged(QModelIndex, QModelIndex)
{
  item_->setCalculus(model_->getCalculus());
}

}