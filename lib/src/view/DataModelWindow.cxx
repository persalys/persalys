//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the data model
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
#include "otgui/DataModelWindow.hxx"

#include "otgui/DataModelTableModel.hxx"
#include "otgui/ExportableTableView.hxx"

#include <QVBoxLayout>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {

DataModelWindow::DataModelWindow(DesignOfExperimentItem * item)
  : OTguiSubWindow(item)
  , designOfExperiment_(item->getDesignOfExperiment())
{
  buildInterface();
}


void DataModelWindow::buildInterface()
{
  setWindowTitle(tr("Data model"));

  QTabWidget * tabWidget = new QTabWidget;

  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  const UnsignedInteger nbInputs(designOfExperiment_.getInputSample().getDimension());

  // warning: sample to display is not the sample from File! -> create a new DataModel
  DataModel newDataModel("unnamed", designOfExperiment_.getInputSample(), designOfExperiment_.getOutputSample());

  // set table
  ExportableTableView * tableView = new ExportableTableView;
  tableView->horizontalHeader()->hide();

  DataModelTableModel * model = new DataModelTableModel(designOfExperiment_.getSample(), newDataModel, false, tableView);
  tableView->setModel(model);
  if (nbInputs > 1)
    tableView->setSpan(1, 0, 1, nbInputs);
  if (designOfExperiment_.getOutputSample().getDimension() > 1)
    tableView->setSpan(1, nbInputs, 1, designOfExperiment_.getSample().getDimension());

  tabLayout->addWidget(tableView);

  tabWidget->addTab(tab, tr("Table"));

  setWidget(tabWidget);
}
}