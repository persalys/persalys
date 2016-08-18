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

DataModelWindow::DataModelWindow(DataModelItem * item)
  : OTguiSubWindow(item)
  , dataModel_(item->getDataModel().getImplementation()->clone())
{
  buildInterface();
}


void DataModelWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  const UnsignedInteger nbInputs(dataModel_.getInputSample().getDimension());

  NumericalSample sample(dataModel_.getInputSample());
  if (dataModel_.getOutputSample().getSize())
    sample.stack(dataModel_.getOutputSample());

  // warning: sample is not the sample from File!!!
  // need to set inputcolumns and outputcolumns
  Indices inIndices(nbInputs);
  inIndices.fill();
  Indices outIndices(dataModel_.getOutputSample().getDimension());
  outIndices.fill(nbInputs);
  dataModel_.setColumns(inIndices, outIndices);

  DataModelTableModel * model = new DataModelTableModel(sample, dataModel_, false);

  ExportableTableView * tableView = new ExportableTableView;
  tableView->horizontalHeader()->hide();
  tableView->setModel(model);
  tableView->setSpan(1, 0, 1, nbInputs);
  if (dataModel_.getOutputSample().getSize())
    tableView->setSpan(1, nbInputs, 1, sample.getDimension());

  tabLayout->addWidget(tableView);

  tabWidget->addTab(tab, tr("Table"));

  setWidget(tabWidget);
}
}