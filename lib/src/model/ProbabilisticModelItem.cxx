//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a probabilistic model
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/ProbabilisticModelItem.hxx"

#include "otgui/LimitState.hxx"
#include "otgui/SobolAnalysis.hxx"
#include "otgui/MonteCarloAnalysis.hxx"
#include "otgui/StudyItem.hxx"

using namespace OT;

namespace OTGUI
{

ProbabilisticModelItem::ProbabilisticModelItem(const PhysicalModel & physicalModel)
  : PhysicalModelItem(physicalModel, "ProbabilisticModel")
  , newDesignOfExperiment_(0)
  , newLimitState_(0)
  , newCentralTendency_(0)
  , newSensitivityAnalysis_(0)

{
  setData(tr("Probabilistic model"), Qt::DisplayRole);
  QFont font;
  font.setWeight(font.weight() + 10);
  setData(font, Qt::FontRole);
  setEditable(false);

  buildActions();
}


void ProbabilisticModelItem::buildActions()
{
  // new design of experiments action
  newDesignOfExperiment_ = new QAction(QIcon(":/images/designOfExperiment.png"), tr("Design of experiments"), this);
  newDesignOfExperiment_->setStatusTip(tr("Create a new design of experiments"));
  connect(newDesignOfExperiment_, SIGNAL(triggered()), this, SIGNAL(designOfExperimentRequested()));

  // new limit state action
  newLimitState_ = new QAction(QIcon(":/images/limitstate.png"), tr("Limit state"), this);
  newLimitState_->setStatusTip(tr("Create a new limit state"));
  connect(newLimitState_, SIGNAL(triggered()), this, SLOT(createLimitState()));

  // new analysis action
  newCentralTendency_ = new QAction(QIcon(":/images/centralTendency.png"), tr("Central tendency"), this);
  newCentralTendency_->setStatusTip(tr("Create a new central tendency"));
  connect(newCentralTendency_, SIGNAL(triggered()), this, SLOT(createCentralTendency()));

  newSensitivityAnalysis_ = new QAction(QIcon(":/images/sensitivity.png"), tr("Sensitivity"), this);
  newSensitivityAnalysis_->setStatusTip(tr("Create a new sensitivity analysis"));
  connect(newSensitivityAnalysis_, SIGNAL(triggered()), this, SLOT(createSensitivityAnalysis()));

  // add actions
  appendAction(newDesignOfExperiment_);
  appendAction(newLimitState_);
  appendSeparator(tr("Analysis"));
  appendAction(newCentralTendency_);
  appendAction(newSensitivityAnalysis_);
}


void ProbabilisticModelItem::update(Observable* source, const String & message)
{
  // emit signals to ProbabilisticModelWindow

  if (message == "inputNameChanged" ||
      message == "inputNumberChanged" ||
      message == "inputDistributionChanged"
     )
  {
    emit stochasticInputListChanged();
  }
  else if (message == "copulaChanged")
  {
    emit copulaChanged();
  }
  else if (message == "inputValueChanged")
  {
    emit inputListDefinitionChanged();
  }
  else if (message == "probabilisticModelRemoved")
  {
    emit removeRequested(row());
  }
}


bool ProbabilisticModelItem::physicalModelValid()
{
  if (!physicalModel_.isValid())
  {
    emit showErrorMessageRequested(tr("The physical model must have inputs AND at least one selected output."));
    return false;
  }
  if (!physicalModel_.hasStochasticInputs())
  {
    emit showErrorMessageRequested(tr("The physical model must have stochastic inputs."));
    return false;
  }
  return true;
}


void ProbabilisticModelItem::createLimitState()
{
  // check
  if (!physicalModelValid())
    return;

  // new limit state
  LimitState newLimitState(getParentStudyItem()->getStudy().getAvailableLimitStateName(tr("limitState_").toStdString()), physicalModel_, physicalModel_.getSelectedOutputsNames()[0], OT::Less(), 0.);
  getParentStudyItem()->getStudy().add(newLimitState);
}


void ProbabilisticModelItem::createCentralTendency()
{
  // check
  if (!physicalModelValid())
    return;

  // new analysis
  const String analysisName(getParentStudyItem()->getStudy().getAvailableAnalysisName(tr("centralTendency_").toStdString()));
  MonteCarloAnalysis analysis(analysisName, physicalModel_);
  // emit signal to StudyTreeView to open a wizard
  emit analysisRequested(this, analysis);
}


void ProbabilisticModelItem::createSensitivityAnalysis()
{
  // check
  if (!physicalModelValid())
    return;

  // check if the model has an independent copula
  if (!physicalModel_.getCopula().hasIndependentCopula())
  {
    emit showErrorMessageRequested(tr("The model must have an independent copula to compute a sensitivity analysis but inputs are dependent."));
    return;
  }

  // new analysis
  const String analysisName(getParentStudyItem()->getStudy().getAvailableAnalysisName(tr("sensitivity_").toStdString()));
  SobolAnalysis analysis(analysisName, physicalModel_);
  // emit signal to StudyTreeView to open a wizard
  emit analysisRequested(this, analysis);
}
}
