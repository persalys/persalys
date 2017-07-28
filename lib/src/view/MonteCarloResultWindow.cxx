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

namespace OTGUI {

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

  isConfidenceIntervalRequired_ = analysis.isConfidenceIntervalRequired();
  levelConfidenceInterval_ = analysis.getLevelConfidenceInterval();

  PhysicalModel model(analysis.getPhysicalModel());

  // inputs
  for (UnsignedInteger i=0; i<result_.getInputSample().getDimension(); ++i)
  {
    const String inputName = result_.getInputSample().getDescription()[i];
    stochInputNames_ << QString::fromUtf8(inputName.c_str());
    QString inputDescription;
    try
    {
      inputDescription = QString::fromUtf8(model.getInputByName(inputName).getDescription().c_str());
    }
    catch (std::exception) // maybe the variable does not exist anymore
    {
      //do nothing
    }
    if (!inputDescription.isEmpty())
      inAxisTitles_ << inputDescription;
    else
      inAxisTitles_ << stochInputNames_.last();
  }

  // outputs
  for (UnsignedInteger i=0; i<result_.getOutputSample().getDimension(); ++i)
  {
    const String outputName = result_.getOutputSample().getDescription()[i];
    outputNames_ << QString::fromUtf8(outputName.c_str());
    QString outputDescription;
    try
    {
      outputDescription = QString::fromUtf8(model.getOutputByName(outputName).getDescription().c_str());
    }
    catch (std::exception) // maybe the variable does not exist anymore
    {
      //do nothing
    }
    if (!outputDescription.isEmpty())
      outAxisTitles_ << outputDescription;
    else
      outAxisTitles_ << outputNames_.last();
  }

  sampleSizeTitle_ = tr("Number of calls") + " ";

  // parameters widget
  setParameters(item->getAnalysis(), tr("Central tendency parameters"));

  showTable_ = true;

  for (UnsignedInteger j=0; j<result_.getSample().getSize(); ++j)
  {
    for (UnsignedInteger i=0; i<result_.getSample().getDimension(); ++i)
    {
      if (std::isnan(result_.getSample()[j][i]))
      {
        resultsSampleIsValid_ = false;
        break;
      }
    }
  }
}
}
