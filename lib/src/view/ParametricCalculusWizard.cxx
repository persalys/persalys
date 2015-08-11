// ParametricCalculusWizard.cxx

#include "ParametricCalculusWizard.hxx"

#include <QVBoxLayout>

namespace OTGUI {

ParametricCalculusWizard::ParametricCalculusWizard()
  : QWizard()
  , item_(0)
{
  buildInterface();
}

ParametricCalculusWizard::ParametricCalculusWizard(ParametricCalculusItem * item)
  : QWizard()
  , item_(0)
{
  buildInterface();
  completeModel(item);
}


ParametricCalculusWizard::~ParametricCalculusWizard()
{
  
}


void ParametricCalculusWizard::buildInterface()
{
  QWizardPage * page = new QWizardPage(this);
  setWindowTitle("Parametric analysis");

  QVBoxLayout * pageLayout = new QVBoxLayout;

  tableView_ = new QTableView;
  pageLayout->addWidget(tableView_);
  page->setLayout(pageLayout);

  addPage(page);
}


void ParametricCalculusWizard::completeModel(ParametricCalculusItem* item)
{
  setItem(item);
  model_ = new ParametricCalculusTableModel(item_->getCalculus());
  connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(parametrizationChanged(const QModelIndex&,const QModelIndex&)));
  tableView_->setModel(model_);
}


ParametricCalculusItem* ParametricCalculusWizard::getItem() const
{
  return item_;
}


void ParametricCalculusWizard::setItem(ParametricCalculusItem* item)
{
  item_ = item;
}


void ParametricCalculusWizard::parametrizationChanged(const QModelIndex&,const QModelIndex&)
{
  item_->setCalculus(model_->getCalculus());
}

}