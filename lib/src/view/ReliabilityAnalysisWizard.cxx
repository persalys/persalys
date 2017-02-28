//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a reliability analysis
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
#include "otgui/ReliabilityAnalysisWizard.hxx"

#include "otgui/FORMImportanceSamplingAnalysis.hxx"
#include "otgui/MonteCarloReliabilityAnalysis.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI {

FORMPage::FORMPage(QWidget* parent)
  : QWizardPage(parent)
{
  setTitle(tr("FORM optimization algorithm configuration"));

  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  formWidget_ = new OptimizationWidget(this);
  mainLayout->addWidget(formWidget_);
}


void FORMPage::initialize(const Analysis& analysis)
{
  formWidget_->initialize(analysis);
}


OptimizationSolver FORMPage::getOptimizationAlgorithm() const
{
  return formWidget_->getOptimizationAlgorithm();
}


ReliabilityAnalysisWizard::ReliabilityAnalysisWizard(const OTStudy& otStudy, const LimitState& limitState, QWidget* parent)
  : AnalysisWizard(MonteCarloReliabilityAnalysis(otStudy.getAvailableAnalysisName("reliability_MC_"), limitState), parent)
{
  buildInterface();
}


ReliabilityAnalysisWizard::ReliabilityAnalysisWizard(const Analysis& analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
{
  buildInterface();
}


void ReliabilityAnalysisWizard::buildInterface()
{
  setWindowTitle(tr("Threshold exceedance"));

  // Second Page: simulation methods
  simulationPage_ = new SimulationReliabilityPage(this);
  simulationPage_->initialize(analysis_);
  connect(simulationPage_, SIGNAL(methodChanged(int)), this, SLOT(updateNextId(int)));
  setPage(Page_SimuMethod, simulationPage_);

  // third page: FORM page
  formPage_ = new FORMPage(this);
  formPage_->initialize(analysis_);
  setPage(Page_FORM, formPage_);

  setStartId(Page_SimuMethod);
}


void ReliabilityAnalysisWizard::updateNextId(int id)
{
  simulationPage_->setFinalPage(id == SimulationReliabilityPage::MonteCarlo);
}


int ReliabilityAnalysisWizard::nextId() const
{
  switch (currentId())
  {
    case Page_SimuMethod:
      return simulationPage_->nextId();
    case Page_FORM:
    default:
      return -1;
  }
}


bool ReliabilityAnalysisWizard::validateCurrentPage()
{
  if (currentId() == Page_SimuMethod)
  {
    LimitState limitState = dynamic_cast<SimulationReliabilityAnalysis*>(&*analysis_.getImplementation())->getLimitState();
    analysis_ = simulationPage_->getAnalysis(analysis_.getName(), limitState);
  }
  else if (currentId() == Page_FORM)
  {
    FORMImportanceSamplingAnalysis * formIS_ptr = dynamic_cast<FORMImportanceSamplingAnalysis*>(analysis_.getImplementation().get());
    if (formIS_ptr)
    {
      FORMImportanceSamplingAnalysis analysis(*formIS_ptr);
      analysis.setOptimizationAlgorithm(formPage_->getOptimizationAlgorithm());
      analysis.setPhysicalStartingPoint(formPage_->getOptimizationAlgorithm().getStartingPoint());
      analysis_ = analysis;
    }
  }

  return QWizard::validateCurrentPage();
}
}