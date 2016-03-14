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
#include "otgui/DataTableWidget.hxx"

#include <QtGui>
#include <iostream>

namespace OTGUI {

DataTableWidget::DataTableWidget(QWidget * parent)
: QTableWidget(parent)
{
  initialize();
}


DataTableWidget::DataTableWidget(int rowCount, int colCount, QWidget* parent)
: QTableWidget(rowCount, colCount, parent)
{
  initialize();
}


void DataTableWidget::initialize()
{
//   precision_ = 4;
  verticalHeader()->hide();
#if QT_VERSION >= 0x050000
  horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
  horizontalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
  
  
//   horizontalHeader()->setResizeMode(QHeaderView::Stretch);
//   setEditTriggers(QAbstractItemView::NoEditTriggers);
//   setAlternatingRowColors(true);
//   setSortingEnabled(false);
  
//   setDragDropMode(QAbstractItemView::DragOnly);
  buildActions();
}





// void DataTableWidget::mousePressEvent(QMouseEvent *event)
// {
//   if (event->button() == Qt::LeftButton) {
//     dragStartPosition_ = event->pos();
//   }
//   QTableWidget::mousePressEvent(event);
// }


// void DataTableWidget::mouseMoveEvent(QMouseEvent *event)
// {
//   if (!(event->buttons() & Qt::LeftButton))
//       return;
//   if ((event->pos() - dragStartPosition_).manhattanLength()
//       < QApplication::startDragDistance())
//       return;
// 
//   QDrag *drag = new QDrag((QWidget*)this);
//   QMimeData *mimeData = new QMimeData;
//   mimeData->setText(getFormattedText());
//   drag->setMimeData(mimeData);
//   drag->exec(Qt::CopyAction);
// }


QTableWidgetSelectionRange DataTableWidget::selectedRange() const {
  QList<QTableWidgetSelectionRange> ranges = selectedRanges();
  QTableWidgetSelectionRange result;

  if (ranges.size() == 1) {
    result = ranges.first();
  } else if (ranges.size() > 1) {
    int top = ranges.first().topRow();
    int left = ranges.first().leftColumn();
    int bottom = ranges.last().bottomRow();
    int right = ranges.last().rightColumn();
    result = QTableWidgetSelectionRange(top, left, bottom, right);
  }
  return result;
}


QString DataTableWidget::getFormattedText() const {
  QTableWidgetSelectionRange range = selectedRange();
  QString result;
  for (int i = 0; i < range.rowCount(); ++ i) {
    if (i >0)
      result += "\n";
    for (int j = 0; j < range.columnCount(); ++ j) {
      if (j > 0)
        result += "\t";
      int row = range.topRow() + i;
      int column = range.leftColumn() + j;
      QTableWidgetItem *item = this->item(row, column);

      if (item) {
        result += item->text();
      }
    }
  }
  return result;
}


void DataTableWidget::buildActions()
{
  copyAction_ = new QAction(tr("&Copy"), this);
  copyAction_->setShortcut(tr("Ctrl+C"));
  addAction(copyAction_);
  connect(copyAction_, SIGNAL(triggered()), this, SLOT(copy()));

  foreach(QAction* action, actions())
    action->setShortcutContext(Qt::WidgetShortcut);
}

void DataTableWidget::copy()
{
  QApplication::clipboard()->setText(getFormattedText());
}

}
