//                                               -*- C++ -*-
/**
 *  @brief QTableView to display samples
 *
 *  Copyright 2015-2016 EDF
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
#include "otgui/OTguiTableView.hxx"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QApplication>

namespace OTGUI {

OTguiTableView::OTguiTableView(QWidget* parent)
  : QTableView(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
}


OTguiTableView::OTguiTableView(const OT::NumericalSample & sample, QWidget *parent)
  : QTableView(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
  setModel(new OTguiTableModel(sample));
}


// show the context menu when right clicking
void OTguiTableView::contextMenu(const QPoint & pos)
{
  QMenu * contextMenu(new QMenu(this));
  QAction * exportData = new QAction(tr("Export"), this);
  exportData->setStatusTip(tr("Export the data"));
  connect(exportData, SIGNAL(triggered()), this, SLOT(exportData()));
  contextMenu->addAction(exportData);
  contextMenu->popup(this->mapToGlobal(pos));
}


void OTguiTableView::exportData()
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

    QFile file(fileName);
    settings.setValue("currentDir", QFileInfo(fileName).absolutePath());

    // check
    if (!file.open(QFile::WriteOnly))
    {
      QMessageBox::warning(QApplication::activeWindow(), tr("Warning"),
                           tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    }
    try
    {
      static_cast<OTguiTableModel*>(model())->exportData(fileName);
    }
    catch (std::exception & ex)
    {
      QMessageBox::warning(QApplication::activeWindow(), tr("Warning"), tr("Impossible to export the data."));
    }
  }
}
}