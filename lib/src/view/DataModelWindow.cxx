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
  , dataModel_(item->getDataModel())
{
  buildInterface();
}


void DataModelWindow::buildInterface()
{
  QTabWidget * tabWidget = new QTabWidget;

  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  NumericalSample sample(dataModel_.getInputSample());
  if (dataModel_.getResult().getOutputSample().getSize())
    sample.stack(dataModel_.getResult().getOutputSample());

  DataModelTableModel * model = new DataModelTableModel(sample, dataModel_, false);

  ExportableTableView * tableView = new ExportableTableView;
  tableView->horizontalHeader()->hide();
  tableView->setModel(model);
  const UnsignedInteger nbInputs(dataModel_.getInputSample().getDimension());
  tableView->setSpan(1, 0, 1, nbInputs);
  if (dataModel_.getResult().getOutputSample().getSize())
    tableView->setSpan(1, nbInputs, 1, sample.getDimension());

  tabLayout->addWidget(tableView);

  tabWidget->addTab(tab, tr("Table"));

  setWidget(tabWidget);
}
}