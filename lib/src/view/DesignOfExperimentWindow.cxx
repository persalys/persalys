//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the designs of experiments
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/DesignOfExperimentWindow.hxx"

#include "persalys/DesignOfExperimentEvaluation.hxx"

using namespace OT;

namespace PERSALYS
{

DesignOfExperimentWindow::DesignOfExperimentWindow(AnalysisItem* item, QWidget * parent)
  : DataAnalysisWindow(item, parent)
{
  initialize(item);

  buildInterface();
}


void DesignOfExperimentWindow::initialize(AnalysisItem* item)
{
  DesignOfExperimentEvaluation analysis(*dynamic_cast<DesignOfExperimentEvaluation*>(item->getAnalysis().getImplementation().get()));

  result_ = analysis.getResult();
  designOfExperiment_ = analysis.getResult().getDesignOfExperiment();
  failedInputSample_ = analysis.getFailedInputSample();
  errorDescription_ = analysis.getErrorDescription();
  notEvaluatedInputSample_ = analysis.getNotEvaluatedInputSample();

  titleLabel_->setText(tr("Design of experiments"));
  titleLabel_->setDocLink("user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#doeevalresult");

  // parameters widget
  setParameters(item->getAnalysis(), tr("Design of experiments evaluation parameters"));
}
}
