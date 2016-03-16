//                                               -*- C++ -*-
/**
 *  @brief Generic data table
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
#include "otgui/OTguiTableView.hxx"

#include <QtGui>

namespace OTGUI {

OTguiTableView::OTguiTableView(QWidget * parent)
  : QTableView(parent)
{
  buildActions();
}


QString OTguiTableView::getFormattedText() const
{
  QString result;
  QModelIndexList list = selectionModel()->selectedIndexes();

  if(list.size() < 1)
    return result;

  qSort(list);

  QModelIndex last = list.last();
  QModelIndex previous = list.first();

  list.removeFirst();

  for(int i = 0; i < list.size(); ++i)
  {
    QVariant data = model()->data(previous);
    QString text = data.toString();
    QModelIndex index = list.at(i);
    result.append(text);

    if(index.row() != previous.row())
      result.append('\n');
    else
      result.append('\t');
    previous = index;
  }

  result.append(model()->data(list.last()).toString());
  result.append('\n');

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(result);
  return result;
}


void OTguiTableView::buildActions()
{
  copyAction_ = new QAction(tr("&Copy"), this);
  copyAction_->setShortcut(tr("Ctrl+C"));
  addAction(copyAction_);
  connect(copyAction_, SIGNAL(triggered()), this, SLOT(copy()));

  foreach(QAction* action, actions())
    action->setShortcutContext(Qt::WidgetShortcut);
}


void OTguiTableView::copy()
{
  QApplication::clipboard()->setText(getFormattedText());
}
}