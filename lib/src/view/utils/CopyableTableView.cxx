//                                               -*- C++ -*-
/**
 *  @brief Generic data table
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/CopyableTableView.hxx"

#include <QApplication>
#include <QAction>
#include <QClipboard>
#include <QHeaderView>
#include <QScrollBar>

namespace PERSALYS
{

CopyableTableView::CopyableTableView(QWidget * parent)
  : QTableView(parent)
{
  buildActions();
}


QSize CopyableTableView::sizeHint() const
{
  if (verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff &&
      horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff)
    return minimumSize();
  return QTableView::sizeHint();
}


QSize CopyableTableView::minimumSizeHint() const
{
  if (verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff &&
      horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff)
    return minimumSize();
  return QTableView::minimumSizeHint();
}


void CopyableTableView::resizeToContents()
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

  // resize columns To Contents: set optimal size
  resizeColumnsToContents();
  verticalHeader()->resizeSections(QHeaderView::ResizeToContents);

  // resize table
  const int w = horizontalHeader()->length() + (verticalHeader()->isHidden() ? 0 : verticalHeader()->sizeHint().width());
  const int h = verticalHeader()->length() + horizontalHeader()->height();
  const QMargins margins(contentsMargins());
  setFixedSize(w + margins.left() + margins.right(), h + margins.top() + margins.bottom());
}


void CopyableTableView::resizeWithOptimalHeight()
{
  int h = verticalHeader()->length() + horizontalHeader()->height();
  // if there is an horizontal scroll bar, add its height
  if (horizontalScrollBar()->maximum())
    h += horizontalScrollBar()->sizeHint().height();
  const QMargins margins(contentsMargins());
  setFixedHeight(h + margins.top() + margins.bottom());
}


QString CopyableTableView::getFormattedText() const
{
  QString result;
  QModelIndexList list = selectionModel()->selectedIndexes();

  if(list.size() < 1)
    return result;

  std::sort(list.begin(), list.end());

  QModelIndex previous = list.first();

  list.removeFirst();

  if (list.isEmpty())
    result.append(model()->data(previous).toString());

  for (int i = 0; i < list.size(); ++i)
  {
    QVariant data = model()->data(previous);
    QString text = data.toString();
    result.append(text);

    QModelIndex index = list.at(i);
    if (index.row() != previous.row())
      result.append('\n');
    else
      result.append('\t');
    previous = index;
  }

  if (!list.isEmpty())
    result.append(model()->data(list.last()).toString());
  result.append('\n');

  return result;
}


void CopyableTableView::buildActions()
{
  copyAction_ = new QAction(tr("&Copy"), this);
  copyAction_->setShortcut(tr("Ctrl+C"));
  addAction(copyAction_);
  connect(copyAction_, SIGNAL(triggered()), this, SLOT(copy()));

  foreach(QAction* action, actions())
    action->setShortcutContext(Qt::WidgetShortcut);
}


void CopyableTableView::copy()
{
  QApplication::clipboard()->setText(getFormattedText());
}
}
