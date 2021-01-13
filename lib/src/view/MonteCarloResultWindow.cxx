//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of Monte Carlo
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/MonteCarloResultWindow.hxx"

#include "persalys/MonteCarloAnalysis.hxx"

using namespace OT;

namespace PERSALYS
{

MonteCarloResultWindow::MonteCarloResultWindow(AnalysisItem * item, QWidget * parent)
  : DataAnalysisWindow(item, parent)
{
  hasMaximumCV_ = true;
  if (dynamic_cast<MonteCarloAnalysis*>(item->getAnalysis().getImplementation().get()))
    initialize(item);
  else
    throw InvalidArgumentException (HERE) << "Can NOT build the MonteCarloResultWindow: The analysis of the item is not valid";

  buildInterface();
}


void MonteCarloResultWindow::initialize(AnalysisItem* item)
{
  MonteCarloAnalysis analysis(*dynamic_cast<MonteCarloAnalysis*>(item->getAnalysis().getImplementation().get()));

  result_ = analysis.getResult();
  designOfExperiment_ = result_.getDesignOfExperiment();
  failedInputSample_ = analysis.getFailedInputSample();

  // set stop criteria message
  if (result_.getElapsedTime() >= analysis.getMaximumElapsedTime())
    analysisStopCriteriaMessage_ = tr("Maximum elapsed time reached");
  if (designOfExperiment_.getOutputSample().getSize() == analysis.getMaximumCalls())
    analysisStopCriteriaMessage_ = tr("Maximum calls reached");
  if (!analysis.getWarningMessage().empty())
    analysisStopCriteriaMessage_ = tr("An error has occurred during the execution of the analysis");
  if (designOfExperiment_.getInputSample().getSize() > 1)
  {
    const UnsignedInteger nbInputs =  designOfExperiment_.getInputSample().getDimension();
    const Scalar sqrtSampleSize = sqrt(designOfExperiment_.getInputSample().getSize());
    for (UnsignedInteger i = nbInputs; i < result_.getCoefficientOfVariation().getSize(); ++i)
    {
      if (result_.getCoefficientOfVariation()[i].getSize() == 1 &&
          result_.getCoefficientOfVariation()[i][0] / sqrtSampleSize <= analysis.getMaximumCoefficientOfVariation())
        analysisStopCriteriaMessage_ = tr("Maximum coefficient of variation reached");
    }
  }

  // if not one of the previous criteria
  if (analysisStopCriteriaMessage_.isEmpty())
    analysisStopCriteriaMessage_ = tr("Stop requested");

  analysisErrorMessage_ = analysis.getWarningMessage().c_str();

  isConfidenceIntervalRequired_ = analysis.isConfidenceIntervalRequired();
  levelConfidenceInterval_ = analysis.getLevelConfidenceInterval();

  sampleSizeTitle_ = tr("Number of calls") + " ";

  titleLabel_->setText(tr("Monte Carlo method"));
  titleLabel_->setDocLink("user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#montecarloresult");

  // parameters widget
  setParameters(item->getAnalysis(), tr("Central tendency parameters"));

  showTable_ = true;
}
}
