//                                               -*- C++ -*-
/**
 *  @brief Editable Exportable QTableView
 *
 *  Copyright 2015-2020 EDF-Phimeca
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

#include "persalys/EditableExportableTableView.hxx"

#include <QMenu>
#include <QSortFilterProxyModel>

using namespace OT;

namespace PERSALYS
{

  EditableExportableTableView::EditableExportableTableView(QWidget* parent)
    : ExportableTableView(parent)
    , addRowAction_(0)
    , removeRowAction_(0)
    , cleanAction_(0)
  {

    addRowAction_ = new QAction(QIcon(":/images/list-add.png"), tr("Add row"), this);
    removeRowAction_ = new QAction(QIcon(":/images/list-remove.png"), tr("Remove row(s)"), this);
    // https://store.kde.org/p/1283153
    cleanAction_ = new QAction(QIcon(":/images/clean.png"), tr("Clean"), this);

    connect(addRowAction_, SIGNAL(triggered()), this, SLOT(addRow()));
    connect(removeRowAction_, SIGNAL(triggered()), this, SLOT(removeRows()));
    connect(cleanAction_, &QAction::triggered, [=]() {
        emit cleanRequested();});
    setContextMenuPolicy(Qt::CustomContextMenu);
  }

  // show the context menu when right clicking
  void EditableExportableTableView::contextMenu(const QPoint & pos)
  {
    QMenu * contextMenu(new QMenu(this));
    contextMenu->addAction(exportAction_);
    contextMenu->addAction(cleanAction_);
    contextMenu->addAction(addRowAction_);
    contextMenu->addAction(removeRowAction_);
    if (exportableAsImageAction_)
      contextMenu->addAction(exportableAsImageAction_);
    contextMenu->popup(this->mapToGlobal(pos));
  }

  void EditableExportableTableView::addRow()
  {
    QAbstractItemModel * sourceModel = dynamic_cast<QSortFilterProxyModel*>(model())->sourceModel();
    sourceModel->insertRows(sourceModel->rowCount()-1, 1);

  }

  void EditableExportableTableView::removeRows()
  {
    QSortFilterProxyModel* myProxy = dynamic_cast<QSortFilterProxyModel*>(model());
    QAbstractItemModel * sourceModel = myProxy->sourceModel();
    QItemSelection selection(selectionModel()->selection());
    QList<int> rows;
    foreach( const QModelIndex & index, myProxy->mapSelectionToSource(selection).indexes() ) {
      rows.append( index.row() );
    }
    std::sort( rows.begin(), rows.end() );
    int prev = -1;
    for( int i = rows.count() - 1; i >= 0; i -= 1 ) {
      int current = rows[i];
      if( current != prev ) {
        sourceModel->removeRows( current, 1 );
        prev = current;
      }
    }
  }
}
