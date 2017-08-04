//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of data analysis
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
#include "otgui/DataAnalysisResultWindow.hxx"

#include "otgui/DataAnalysis.hxx"
#include "otgui/QtTools.hxx"

using namespace OT;

namespace OTGUI {

DataAnalysisResultWindow::DataAnalysisResultWindow(AnalysisItem * item, QWidget * parent)
  : DataAnalysisWindow(item, parent)
{
  if (!dynamic_cast<DataAnalysis*>(item->getAnalysis().getImplementation().get()))
    throw InvalidArgumentException (HERE) << "Can NOT build the DataAnalysisResultWindow: The analysis of the item is not valid";

  initialize(item);

  buildInterface();

  setWindowTitle(tr("Data analysis results"));
}


void DataAnalysisResultWindow::initialize(AnalysisItem* item)
{
  isConfidenceIntervalRequired_ = dynamic_cast<DataAnalysis*>(item->getAnalysis().getImplementation().get())->isConfidenceIntervalRequired();
  levelConfidenceInterval_ = dynamic_cast<DataAnalysis*>(item->getAnalysis().getImplementation().get())->getLevelConfidenceInterval();

  result_ = dynamic_cast<DataAnalysis*>(item->getAnalysis().getImplementation().get())->getResult();

  // inputs
  if (result_.getInputSample().getSize())
  {
    stochInputNames_ = QtOT::DescriptionToStringList(result_.getInputSample().getDescription());
    inAxisTitles_ = stochInputNames_;
  }

  // outputs
  if (result_.getOutputSample().getSize())
  {
    outputNames_ = QtOT::DescriptionToStringList(result_.getOutputSample().getDescription());
    outAxisTitles_ = outputNames_;
  }
}
}
