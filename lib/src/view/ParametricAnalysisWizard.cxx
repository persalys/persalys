// ParametricAnalysisWizard.cxx

#include "otgui/ParametricAnalysisWizard.hxx"
#include "otgui/ParametricAnalysis.hxx"

#include <QVBoxLayout>
#include <QTableView>

namespace OTGUI {

ParametricAnalysisWizard::ParametricAnalysisWizard(OTStudy * study, const PhysicalModel & physicalModel)
  : QWizard()
  , analysis_(ParametricAnalysis("aAnalysis", physicalModel))
  , OTStudy_(study)
{
  buildInterface();
}


ParametricAnalysisWizard::ParametricAnalysisWizard(const Analysis & analysis)
  : QWizard()
  , analysis_(analysis)
{
  dynamic_cast<ParametricAnalysis*>(&*analysis_.getImplementation())->updateParameters();
  buildInterface();
}


void ParametricAnalysisWizard::buildInterface()
{
  setWindowTitle("Parametric analysis");

  QWizardPage * page = new QWizardPage(this);
  QVBoxLayout * pageLayout = new QVBoxLayout(page);
  QTableView * tableView = new QTableView;

  model_ = new ParametricAnalysisTableModel(analysis_);
  connect(model_, SIGNAL(dataChanged(Analysis &)), this, SLOT(setAnalysis(Analysis &)));
  tableView->setModel(model_);

  pageLayout->addWidget(tableView);
  addPage(page);
}


void ParametricAnalysisWizard::setAnalysis(Analysis & analysis)
{
  analysis_ = analysis;
}


void ParametricAnalysisWizard::validate()
{
  OTStudy_->addAnalysis(analysis_);
  analysis_.run();
}


}