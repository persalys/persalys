//                                               -*- C++ -*-
/**
 *  @brief QTableView to display samples
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
#include "otgui/DataTableView.hxx"

#include "otgui/DataTableModel.hxx"
#include "otgui/QtTools.hxx"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QApplication>

namespace OTGUI {

DataTableView::DataTableView(QWidget* parent)
  : CopyableTableView(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
}


DataTableView::DataTableView(const OT::NumericalSample & sample, QWidget *parent)
  : CopyableTableView(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
  setModel(new DataTableModel(sample));
}


// show the context menu when right clicking
void DataTableView::contextMenu(const QPoint & pos)
{
  QMenu * contextMenu(new QMenu(this));
  QAction * exportData = new QAction(tr("Export"), this);
  exportData->setStatusTip(tr("Export the data"));
  connect(exportData, SIGNAL(triggered()), this, SLOT(exportData()));
  contextMenu->addAction(exportData);
  contextMenu->popup(this->mapToGlobal(pos));
}


void DataTableView::exportData()
{
  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();
  QString fileName = QFileDialog::getSaveFileName(this, tr("Export model as..."),
                     currentDir,
                     tr("CSV source files (*.csv)"));

  if (!fileName.isEmpty())
  {
    if (!fileName.endsWith(".csv"))
      fileName += ".csv";

    settings.setValue("currentDir", QFileInfo(fileName).absolutePath());

    try
    {
      if (!dynamic_cast<CustomStandardItemModel*>(model()))
        throw SimpleException(tr("Internal exception"));
      dynamic_cast<CustomStandardItemModel*>(model())->exportData(fileName);
    }
    catch (std::exception & ex)
    {
      QMessageBox::warning(QApplication::activeWindow(), tr("Warning"), tr("Impossible to export the data. ")+ex.what());
    }
  }
}
}