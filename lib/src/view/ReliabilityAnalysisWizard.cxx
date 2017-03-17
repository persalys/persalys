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
#include <QRadioButton>

using namespace OT;

namespace OTGUI {

IntroReliabilityPage::IntroReliabilityPage(QWidget * parent)
  : QWizardPage(parent)
{
  setTitle(tr("Reliability methods"));

  QVBoxLayout * pageLayout = new QVBoxLayout(this);

  QGroupBox * methodBox = new QGroupBox(tr("Types of methods"));
  QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

  methodGroup_ = new QButtonGroup;

  QRadioButton * buttonToChooseMethod = new QRadioButton(tr("Simulation methods"));
  methodGroup_->addButton(buttonToChooseMethod, IntroReliabilityPage::SimuMethod);
  methodLayout->addWidget(buttonToChooseMethod);

  buttonToChooseMethod = new QRadioButton(tr("Approximation method"));
  methodGroup_->addButton(buttonToChooseMethod, IntroReliabilityPage::ApproxMethod);
  methodLayout->addWidget(buttonToChooseMethod);

  pageLayout->addWidget(methodBox);
}


void IntroReliabilityPage::initialize(const Analysis& analysis)
{
  const String analysisName = analysis.getImplementation()->getClassName();

  if (analysisName == "FORMImportanceSamplingAnalysis" || analysisName == "MonteCarloReliabilityAnalysis")
    methodGroup_->button(IntroReliabilityPage::SimuMethod)->click();
  else
    methodGroup_->button(IntroReliabilityPage::ApproxMethod)->click();
}


int IntroReliabilityPage::nextId() const
{
  switch (IntroReliabilityPage::Method(methodGroup_->checkedId()))
  {
    case IntroReliabilityPage::SimuMethod:
    {
      return ReliabilityAnalysisWizard::Page_SimuMethod;
    }
    case IntroReliabilityPage::ApproxMethod:
    {
      return ReliabilityAnalysisWizard::Page_ApproxMethod;
    }
    default:
      return -1;
      break;
  }
}


FORMPage::FORMPage(QWidget* parent)
  : QWizardPage(parent)
{
  setTitle(tr("Optimization algorithm configuration for FORM"));

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
  : AnalysisWizard(MonteCarloReliabilityAnalysis(otStudy.getAvailableAnalysisName("reliability_"), limitState), parent)
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

  // First Page: choose method of reliability analysis
  introPage_ = new IntroReliabilityPage(this);
  introPage_->initialize(analysis_);
  setPage(Page_Intro, introPage_);

  // Second Page: simulation methods
  simulationPage_ = new SimulationReliabilityPage(this);
  simulationPage_->initialize(analysis_);
  connect(simulationPage_, SIGNAL(methodChanged(int)), this, SLOT(updateNextId(int)));
  setPage(Page_SimuMethod, simulationPage_);

  // Second Page: approximation methods
  approximationPage_ = new ApproximationReliabilityPage(this);
  approximationPage_->initialize(analysis_);
  setPage(Page_ApproxMethod, approximationPage_);

  // third page: FORM page
  formPage_ = new FORMPage(this);
  formPage_->initialize(analysis_);
  setPage(Page_FORM, formPage_);

  setStartId(Page_Intro);
}


void ReliabilityAnalysisWizard::updateNextId(int id)
{
  simulationPage_->setFinalPage(id == SimulationReliabilityPage::MonteCarlo);
}


int ReliabilityAnalysisWizard::nextId() const
{
  switch (currentId())
  {
    case Page_Intro:
      return introPage_->nextId();
    case Page_SimuMethod:
      return simulationPage_->nextId();
    case Page_FORM:
    default:
      return -1;
  }
}


bool ReliabilityAnalysisWizard::validateCurrentPage()
{
  if (currentId() == Page_SimuMethod || currentId() == Page_ApproxMethod)
  {
    const LimitState limitState = dynamic_cast<ReliabilityAnalysis*>(analysis_.getImplementation().get())->getLimitState();

    if (currentId() == Page_SimuMethod)
      analysis_ = simulationPage_->getAnalysis(analysis_.getName(), limitState);
    else
      analysis_ = approximationPage_->getAnalysis(analysis_.getName(), limitState);
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