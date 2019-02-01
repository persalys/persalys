//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of data analysis
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
#include "otgui/DataAnalysisResultWindow.hxx"

#include "otgui/DataAnalysis.hxx"

using namespace OT;

namespace OTGUI
{

DataAnalysisResultWindow::DataAnalysisResultWindow(AnalysisItem * item, QWidget * parent)
  : DataAnalysisWindow(item, parent)
{
  if (!dynamic_cast<DataAnalysis*>(item->getAnalysis().getImplementation().get()))
    throw InvalidArgumentException (HERE) << "Can NOT build the DataAnalysisResultWindow: The analysis of the item is not valid";

  initialize(item);

  buildInterface();
}


void DataAnalysisResultWindow::initialize(AnalysisItem* item)
{
  DataAnalysis analysis(*dynamic_cast<DataAnalysis*>(item->getAnalysis().getImplementation().get()));

  isConfidenceIntervalRequired_ = analysis.isConfidenceIntervalRequired();
  levelConfidenceInterval_ = analysis.getLevelConfidenceInterval();

  result_ = analysis.getResult();
  designOfExperiment_ = result_.getDesignOfExperiment();

  titleLabel_->setText(tr("Data analysis"));
}
}
