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
#include "otgui/ParametersWidget.hxx"

#include <QVBoxLayout>
#include <QSortFilterProxyModel>

using namespace OT;

namespace OTGUI {

DesignOfExperimentInputWindow::DesignOfExperimentInputWindow(DesignOfExperimentDefinitionItem * item, QWidget * parent)
  : OTguiSubWindow(item, parent)
  , originalInputSample_(item->getOriginalInputSample())
{
  buildInterface();
}


void DesignOfExperimentInputWindow::buildInterface()
{
  setWindowTitle(tr("Design of experiment"));

  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainWidgetLayout = new QVBoxLayout(mainWidget);

  // parameters
  QStringList namesList;
  namesList << tr("Sample size");
  QStringList valuesList;
  valuesList << QString::number(originalInputSample_.getSize());

  ParametersWidget * table = new ParametersWidget("", namesList, valuesList, true, true);
  mainWidgetLayout->addWidget(table);

  // sample table
  ExportableTableView * tableView = new ExportableTableView;
  tableView->setSortingEnabled(true);

  SampleTableModel * tableModel = new SampleTableModel(originalInputSample_, tableView);
  QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(tableView);
  proxyModel->setSourceModel(tableModel);

  tableView->setModel(proxyModel);
  tableView->sortByColumn(0, Qt::AscendingOrder);
  mainWidgetLayout->addWidget(tableView, 1);

  setWidget(mainWidget);
}
}
