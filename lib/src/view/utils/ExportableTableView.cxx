//                                               -*- C++ -*-
/**
 *  @brief Exportable QTableView
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
#include "otgui/ExportableTableView.hxx"

#include "otgui/SampleTableModel.hxx"
#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/QtTools.hxx"
#include "otgui/FileTools.hxx"

#include <QMenu>
#include <QSortFilterProxyModel>

namespace OTGUI
{

ExportableTableView::ExportableTableView(QAction * exportAction, QWidget* parent)
  : CopyableTableView(parent)
  , exportAction_(exportAction)
{
  // default action
  if (!exportAction)
  {
    exportAction_ = new QAction(QIcon(":/images/document-export-table.png"), tr("Export"), this);
    connect(exportAction_, SIGNAL(triggered()), this, SLOT(exportData()));
  }
  exportAction_->setStatusTip(tr("Export the data"));

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
}


// show the context menu when right clicking
void ExportableTableView::contextMenu(const QPoint & pos)
{
  QMenu * contextMenu(new QMenu(this));
  contextMenu->addAction(exportAction_);
  contextMenu->popup(this->mapToGlobal(pos));
}


void ExportableTableView::exportData()
{
  OT::Sample sample;
  QString text;

  if (dynamic_cast<SampleTableModel*>(model()))
  {
    sample = dynamic_cast<SampleTableModel*>(model())->getSample();
  }
  else if (dynamic_cast<QSortFilterProxyModel*>(model()))
  {
    QAbstractItemModel * sourceModel = dynamic_cast<QSortFilterProxyModel*>(model())->sourceModel();
    if (dynamic_cast<SampleTableModel*>(sourceModel))
      sample = dynamic_cast<SampleTableModel*>(sourceModel)->getSample();
  }
  else if (dynamic_cast<CustomStandardItemModel*>(model()))
  {
    text = dynamic_cast<CustomStandardItemModel*>(model())->getFormattedText();
  }

  if (sample.getSize())
    FileTools::ExportData(sample, this);
  else if (!text.isEmpty())
    FileTools::ExportData(text, this);
  else
    throw SimpleException(tr("Internal exception: can not get the sample"));
}
}
