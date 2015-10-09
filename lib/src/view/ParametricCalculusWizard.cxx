// ParametricCalculusWizard.cxx

#include "ParametricCalculusWizard.hxx"

#include <QVBoxLayout>
#include <QTableView>

namespace OTGUI {

ParametricCalculusWizard::ParametricCalculusWizard(OTStudy * study, const PhysicalModel & physicalModel)
  : QWizard()
  , calculus_(ParametricCalculus("aCalculus", physicalModel))
  , OTStudy_(study)
{
  buildInterface();
}


ParametricCalculusWizard::ParametricCalculusWizard(const Calculus & calculus)
  : QWizard()
  , calculus_(calculus)
{
  dynamic_cast<ParametricCalculus*>(&*calculus_.getImplementation())->updateParameters();
  buildInterface();
}


ParametricCalculusWizard::~ParametricCalculusWizard()
{
}


void ParametricCalculusWizard::buildInterface()
{
  setWindowTitle("Parametric analysis");

  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * pageLayout = new QVBoxLayout;
  QTableView * tableView = new QTableView;

  model_ = new ParametricCalculusTableModel(calculus_);
  connect(model_, SIGNAL(dataChanged(Calculus &)), this, SLOT(setCalculus(Calculus &)));
  tableView->setModel(model_);

  pageLayout->addWidget(tableView);
  page->setLayout(pageLayout);
  addPage(page);
}


void ParametricCalculusWizard::setCalculus(Calculus & calculus)
{
  calculus_ = calculus;
}


void ParametricCalculusWizard::validate()
{
  OTStudy_->addCalculus(calculus_);
  calculus_.run();
}


}