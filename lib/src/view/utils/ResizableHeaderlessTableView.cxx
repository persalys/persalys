//                                               -*- C++ -*-
/**
 *  @brief QTableView without header that allows resizing of columns
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
#include "persalys/ResizableHeaderlessTableView.hxx"

#include <QScrollBar>

using namespace OT;

namespace PERSALYS
{
// ------- HeaderlessTableView

HeaderlessTableView::HeaderlessTableView(QWidget *parent)
  : CopyableTableView(parent)
  , boundary_width_(10)
  , column_index_(-1)
{
  viewport()->setMouseTracking(true);
  viewport()->installEventFilter(this);
  horizontalHeader()->hide();
}


/*
  * @return The index of the column whose right hand boundary the cursor lies
  *         on or -1 if not on a boundary.
  */
int HeaderlessTableView::column_index() const
{
  return column_index_;
}


bool HeaderlessTableView::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::MouseMove)
  {
    if (QMouseEvent *e = dynamic_cast<QMouseEvent *>(event))
    {
      const int currentRow = rowAt(e->pos().y());
      if (currentRow == 0)
      {
        const int col_left = columnAt(e->pos().x() - boundary_width_ / 2);
        const int col_right = columnAt(e->pos().x() + boundary_width_ / 2);
        const bool was_on_boundary = column_index_ != -1;
        if (col_left != col_right)
        {
          if (column_index_ == -1)
          {
            if (col_left != -1)
            {
              column_index_ = col_left;
            }
          }
        }
        else
        {
          column_index_ = -1;
        }
        const bool is_on_boundary = column_index_ != -1;
        if (is_on_boundary != was_on_boundary)
        {
          entered_column_boundary(is_on_boundary);
        }
      }
      else
      {
        column_index_ = -1;
        entered_column_boundary(false);
      }
    }
  }
  return QTableView::eventFilter(obj, event);
}


/*
  * Called whenever the cursor enters or leaves a column boundary.  if
  * `entered' is true then the index of the column can be obtained using
  * `column_index()'.
  */
void HeaderlessTableView::entered_column_boundary(bool /*entered*/)
{
}


// ------- ResizableHeaderlessTableView


ResizableHeaderlessTableView::ResizableHeaderlessTableView(QWidget *parent)
  : HeaderlessTableView(parent)
  , dragging_(false)
  , mouse_pos_()
  , cursor_()
  , minimumSectionsSizes_()
{
  viewport()->installEventFilter(this);
}


void ResizableHeaderlessTableView::resizeWithOptimalWidth()
{
  // resize to contents
  resizeColumnsToContents();

  // compute and store optimal sections sizes
  if (model())
  {
    minimumSectionsSizes_ = Indices(model()->columnCount());
    for (int i = 0; i < model()->columnCount(); ++i)
    {
      minimumSectionsSizes_[i] = columnWidth(i);
    }
  }
  // set width
  int w = horizontalHeader()->length() + verticalHeader()->width();
  if (verticalScrollBar()->maximum())
    w += verticalScrollBar()->sizeHint().width();
  const QMargins margins(contentsMargins());
  setFixedWidth(w + margins.left() + margins.right());
}


bool ResizableHeaderlessTableView::eventFilter(QObject *obj, QEvent *event)
{
  if (QMouseEvent *e = dynamic_cast<QMouseEvent *>(event))
  {
    if (event->type() == QEvent::MouseButtonPress)
    {
      if (column_index() != -1)
      {
        mouse_pos_ = e->pos();
        dragging_ = true;
        return true;
      }
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
      dragging_ = false;
    }
    else if (event->type() == QEvent::MouseMove)
    {
      if (dragging_)
      {
        const int delta = e->pos().x() - mouse_pos_.x();
        if (column_index() < (int)minimumSectionsSizes_.getSize() && column_index() >= 0)
        {
          if (columnWidth(column_index()) + delta < (int)minimumSectionsSizes_[column_index()])
            return false;
        }
        // avoid to hide a column when resizing it
        if (columnWidth(column_index()) + delta < 10)
          return false;
        setColumnWidth(column_index(), columnWidth(column_index()) + delta);
        mouse_pos_ = e->pos();
        return true;
      }
    }
  }
  return HeaderlessTableView::eventFilter(obj, event);
}


/*
  * Override entered_column_boundary to update the cursor sprite when
  * entering/leaving a column boundary.
  */
void ResizableHeaderlessTableView::entered_column_boundary(bool entered)
{
  if (entered)
  {
    cursor_ = viewport()->cursor();
    viewport()->setCursor(QCursor(Qt::SplitHCursor));
  }
  else
  {
    viewport()->setCursor(cursor_);
  }
}
}
