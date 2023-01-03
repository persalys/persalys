//                                               -*- C++ -*-
/**
 *  @brief QTreeView to list studies
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_STUDYTREEVIEW_HXX
#define PERSALYS_STUDYTREEVIEW_HXX

#include "persalys/StudyTreeViewModel.hxx"

#include <QTreeView>
#include <QMouseEvent>

namespace PERSALYS
{
class PERSALYS_VIEW_API StudyTreeView : public QTreeView
{
  Q_OBJECT

public:
  StudyTreeView(QWidget * parent = 0);

  Item * getCurrentItem() const;
  StudyItem * getItem(const int row) const;

  virtual void mousePressEvent(QMouseEvent*);

public slots:
  void onCustomContextMenu(const QPoint& point);
  void selectedItemChanged(const QModelIndex& currentIndex, const QModelIndex& previousIndex);

  void modifyStudySubItemsExpansion(StudyItem* item);

signals:
  void windowRequested(Item*);
  void itemSelectionChanged(QStandardItem*);

private:
  StudyTreeViewModel * treeViewModel_;
};
}
#endif
