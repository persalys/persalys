//                                               -*- C++ -*-
/**
 *  @brief Exportable QTableView
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
#include "otgui/ExportableTableView.hxx"

#include "otgui/SampleTableModel.hxx"
#include "otgui/QtTools.hxx"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QApplication>
#include <QSortFilterProxyModel>

namespace OTGUI
{

ExportableTableView::ExportableTableView(QWidget* parent)
  : CopyableTableView(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
}


// show the context menu when right clicking
void ExportableTableView::contextMenu(const QPoint & pos)
{
  QMenu * contextMenu(new QMenu(this));
  QAction * exportData = new QAction(QIcon(":/images/document-export-table.png"), tr("Export"), this);
  exportData->setStatusTip(tr("Export the data"));
  connect(exportData, SIGNAL(triggered()), this, SLOT(exportData()));
  contextMenu->addAction(exportData);
  contextMenu->popup(this->mapToGlobal(pos));
}


void ExportableTableView::exportData()
{
  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();
  QString fileName = QFileDialog::getSaveFileName(this,
                     tr("Export model as..."),
                     currentDir + QDir::separator() + tr("data"),
                     tr("CSV source files (*.csv)"));

  if (!fileName.isEmpty())
  {
    if (!fileName.endsWith(".csv"))
      fileName += ".csv";

    settings.setValue("currentDir", QFileInfo(fileName).absolutePath());

    try
    {
      if (dynamic_cast<SampleTableModel*>(model()))
      {
        dynamic_cast<SampleTableModel*>(model())->exportData(fileName);
      }
      else if (dynamic_cast<QSortFilterProxyModel*>(model()))
      {
        QAbstractItemModel * sourceModel = dynamic_cast<QSortFilterProxyModel*>(model())->sourceModel();
        if (dynamic_cast<SampleTableModel*>(sourceModel))
          dynamic_cast<SampleTableModel*>(sourceModel)->exportData(fileName);
        else
          throw SimpleException(tr("Internal exception"));
      }
      else
      {
        throw SimpleException(tr("Internal exception"));
      }
    }
    catch (std::exception & ex)
    {
      QMessageBox::warning(QApplication::activeWindow(), tr("Warning"), tr("Impossible to export the data. ") + ex.what());
    }
  }
}
}