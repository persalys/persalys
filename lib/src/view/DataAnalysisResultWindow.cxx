//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of data analysis
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
#include "otgui/DataAnalysisResultWindow.hxx"

#include "otgui/DataAnalysis.hxx"

using namespace OT;

namespace OTGUI {

DataAnalysisResultWindow::DataAnalysisResultWindow(AnalysisItem * item)
  : DataAnalysisWindow(item)
{
  if (dynamic_cast<DataAnalysis*>(&*item->getAnalysis().getImplementation()))
    initialize(item);
  else
    throw InvalidArgumentException (HERE) << "Can NOT build the DataAnalysisResultWindow: The analysis of the item is not valid";

  buildInterface();

  setWindowTitle(tr("Data analysis results"));
}


void DataAnalysisResultWindow::initialize(AnalysisItem* item)
{
  isConfidenceIntervalRequired_ = dynamic_cast<DataAnalysis*>(&*item->getAnalysis().getImplementation())->isConfidenceIntervalRequired();
  levelConfidenceInterval_ = dynamic_cast<DataAnalysis*>(&*item->getAnalysis().getImplementation())->getLevelConfidenceInterval();

  result_ = dynamic_cast<DataAnalysis*>(&*item->getAnalysis().getImplementation())->getResult();

  // inputs
  for (UnsignedInteger i=0; i<result_.getInputSample().getDimension(); ++i)
  {
    const String inputName = result_.getInputSample().getDescription()[i];
    stochInputNames_ << QString::fromUtf8(inputName.c_str());
  }
  inAxisTitles_ = stochInputNames_;

  // outputs
  if (result_.getOutputSample().getSize())
  {
    for (UnsignedInteger i=0; i<result_.getOutputSample().getDimension(); ++i)
    {
      const String outputName = result_.getOutputSample().getDescription()[i];
      outputNames_ << QString::fromUtf8(outputName.c_str());
    }
    outAxisTitles_ = outputNames_;
  }
}
}