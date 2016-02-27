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
  QString fileName = QFileDialog::getSaveFileName(this, tr("Export model as..."),
                     QDir::homePath(),
                     tr("CSV source files (*.csv)"));

  if (!fileName.isEmpty())
  {
    if (!fileName.endsWith(".csv"))
      fileName += ".csv";

    try
    {
      static_cast<OTguiTableModel*>(model())->exportData(fileName);
    }
    catch (std::exception & ex)
    {
      QMessageBox::warning(this, tr("Warning"), tr("Impossible to export the data."));
    }
  }
}
}