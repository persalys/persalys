//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the input sample of the designs of experiments
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
#include "otgui/DesignOfExperimentInputWindow.hxx"

#include "otgui/SampleTableModel.hxx"
#include "otgui/ExportableTableView.hxx"

using namespace OT;

namespace OTGUI {

DesignOfExperimentInputWindow::DesignOfExperimentInputWindow(DesignOfExperimentItem * item)
  : OTguiSubWindow(item)
  , designOfExperiment_(item->getDesignOfExperiment())
{
  buildInterface();
}


void DesignOfExperimentInputWindow::buildInterface()
{
  setWindowTitle(tr("Design of experiment"));

  ExportableTableView * tableView_ = new ExportableTableView;
  SampleTableModel * tableModel = new SampleTableModel(designOfExperiment_.getInputSample(), tableView_);
  tableView_->setModel(tableModel);

  setWidget(tableView_);
}
}