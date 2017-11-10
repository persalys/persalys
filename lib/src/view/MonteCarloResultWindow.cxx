//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of Monte Carlo
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/MonteCarloResultWindow.hxx"

#include "otgui/MonteCarloAnalysis.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI
{

MonteCarloResultWindow::MonteCarloResultWindow(AnalysisItem * item, QWidget * parent)
  : DataAnalysisWindow(item, parent)
{
  if (dynamic_cast<MonteCarloAnalysis*>(item->getAnalysis().getImplementation().get()))
    initialize(item);
  else
    throw InvalidArgumentException (HERE) << "Can NOT build the MonteCarloResultWindow: The analysis of the item is not valid";

  buildInterface();

  setWindowTitle(tr("Monte Carlo analysis results"));
}


void MonteCarloResultWindow::initialize(AnalysisItem* item)
{
  MonteCarloAnalysis analysis(*dynamic_cast<MonteCarloAnalysis*>(item->getAnalysis().getImplementation().get()));

  result_ = analysis.getResult();
  designOfExperiment_ = result_.getDesignOfExperiment();
  failedInputSample_ = analysis.getFailedInputSample();

  if (result_.getElapsedTime() >= analysis.getMaximumElapsedTime())
    analysisStopCriteriaMessage_ = tr("Maximum elapsed time reached");
  if (designOfExperiment_.getOutputSample().getSize() == analysis.getMaximumCalls())
    analysisStopCriteriaMessage_ = tr("Maximum calls reached");
  if (!analysis.getErrorMessage().empty())
    analysisStopCriteriaMessage_ = tr("An error has occured during the execution of the analysis");

  // if not one of the previous criteria
  if (analysisStopCriteriaMessage_.isEmpty())
    analysisStopCriteriaMessage_ = tr("Maximum coefficient of variation reached");

  analysisErrorMessage_ = analysis.getErrorMessage().c_str();

  isConfidenceIntervalRequired_ = analysis.isConfidenceIntervalRequired();
  levelConfidenceInterval_ = analysis.getLevelConfidenceInterval();

  sampleSizeTitle_ = tr("Number of calls") + " ";

  // parameters widget
  setParameters(item->getAnalysis(), tr("Central tendency parameters"));

  showTable_ = true;
}
}
